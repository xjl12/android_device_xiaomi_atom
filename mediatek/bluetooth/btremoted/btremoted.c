/*
 * Copyright (C) 2014 Amazon.com Inc. or its affiliates.  All rights reserved.
 *
 * btremoted: the deamon creates server socket (called "bluetooth") and waits until new
 * client is connected. Client of the server socket is 'VoiceRecorder.java' app. Once a
 * client is connected, the deamon opens connection with SCO socket and waits for incoming
 * data from the socket, expected type of the data is mSBC audio. When data exists on SOC
 * socket, it reads the data, decodes it to PCM and stores in main buffer. The data will be
 * send out ot client per client request.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <sys/poll.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <fcntl.h>
#include <sbc.h>
#include <android/log.h>
#include "sbcplc.h"
#include "btremoted.h"
#include <cutils/sockets.h>

#ifdef _METRICS
#include <limits.h>
#include <cutils/mlog.h>
#endif
#include <unistd.h>

//Writing to file to debug now requires SELinux enforcing
//to be off. Use adb shell setenforce 0 if you want to
//uncomment DEBUG
//#define DEBUG
//#define DEBUG_PRINT_SCO_PACKET

/* Globals */
static sbc_t sbc;

/* There are configurable android properties defined for this daemon:
 *     name of property     default value
 *     ----------------     -------------
 * [btremoted.decode.enabled]: [1]
 *     - When set to true, it decodes the msbc data
 *
 * [btremoted.plc.enabled]:    [1]
 *     - When set to true, it performs packet loss concealment.
 *
 * [btremoted.trim_length]:    [40]
 *     - This trimming was added to compensate for the transition from digital
 *       silence (what is outputted by Audience before it is active) to the
 *       first non-zero sample outputted by Audience. This caused an audible
 *       click which confused the ASR.  Default is set at 40ms.
 *     - At 40ms, this translates to 1280 bytes. btremoted waits
 *       until it receives more than 1280 bytes before sending the first
 *       packet stream data to the search agent.
 */
static int decode_enabled = 0;
static int plc_enabled = 0;
static int ntrim = 0;

static struct PLC_State plc_state;
static int last_seq_num = 0;
static char main_buf[BUFFER_SIZE] = { 0 };
static unsigned int head = 0;
static unsigned int tail = 0;
static int first_write = 0;

#ifdef _METRICS
static unsigned int mtotal_written = 0;
#endif

#ifdef DEBUG
/* Output files */
static const char *wav_file  = "/sdcard/dbg_audio.wv";
static const char *raw_file  = "/sdcard/dbg_audio.raw";
static const char *msbc_file = "/sdcard/dbg_audio.msbc";
static int wav_fd = -1;
static int raw_fd = -1;
static int msbc_fd = -1;

/* Wave File Header Layout */
/* RIFF files consist of 'chunks'.  All chunks have the following format
 * 4 bytes: an ASCII identifier for this chunk (examples are "fmt " and
 *          "data"; note the space in "fmt ")
 * 4 bytes: an unsigned, little-endian 32-bit integer with the length
 *          of this chunk
 * variable-sized field: the chunk data itself, of the size given in the
 *          previous field.
 * a pad byte, if the chunk's length is not even.
 */
#define WAV_HEADER_SIZE 44
static unsigned char CHUNK_ID[] = { 'R', 'I', 'F', 'F' };
static unsigned char chunk_size[4] = { 0 };
static unsigned char FORMAT[] = { 'W', 'A', 'V', 'E' };
static unsigned char SUBCHUNK1_ID[] = { 'f', 'm', 't', ' ' };
static unsigned char SUBCHUNK1_SIZE[] = { 0x10, 0x00, 0x00, 0x00 };
static unsigned char AUDIO_FORMAT[] = { 0x01, 0x00 };     // PCM
static unsigned char NUM_CHANNELS[] = { 0x01, 0x00 };     // Mono
static unsigned char SAMPLE_RATE[] = { 0x80, 0x3E, 0x00, 0x00 };  // 16kHz
static unsigned char BYTE_RATE[] = { 0x00, 0x7D, 0x00, 0x00 };    // 32kBps
static unsigned char BLOCK_ALIGN[] = { 0x02, 0x00 };
static unsigned char BITS_PER_SAMPLE[] = { 0x10, 0x00 };      // 16-bit
static unsigned char SUBCHUNK2_ID[] = { 'd', 'a', 't', 'a' };
static unsigned char subchunk2_size[4] = { 0 };
#endif /* DEBUG */

/* ZIR indices */
static short plc_in[FS] = { 0 };
static short plc_out[FS] = { 0 };
static unsigned char indices0[] =
    { 0xad, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, 0x00, 0x77, 0x6d,
      0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6d, 0xdd, 0xb6,
      0xdb, 0x77, 0x6d, 0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb,
      0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d, 0xb6, 0xdd, 0xdb, 0x6d,
      0xb7, 0x76, 0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d, 0xb6,
      0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6c, 0x0 };

/* SCO packet sequence codes */
#define NUM_SEQ_CODES 4
static unsigned char SEQ_CODE_0 = 0x0;
static unsigned char SEQ_CODE_1 = 0x3;
static unsigned char SEQ_CODE_2 = 0xc;
static unsigned char SEQ_CODE_3 = 0xf;

/* Sockets IDs */
typedef enum {
    ID_SCO      = 0,
    ID_SERVER   = 1,
    ID_CLIENT   = 2,
    ID_ALL      = 3
} ID_SOCKETS;

/* Socket handlers status */
typedef enum {
    STATUS_SUCCESS          =  0,
    STATUS_CLIENT_FAILED    = -1,
    STATUS_SCO_CLOSED       = -2,
    STATUS_SCO_FAILED       = -3,
    STATUS_SCO_NO_DATA      = -4,
    STATUS_SCO_DATA_FAILED  = -5

} STATUS;

static void uchar_to_short_array(unsigned char *cbuf, short *sbuf, int len) {
    int i;
    for (i = 0; i < len / 2; i++)
    {
        sbuf[i] = ( ((short)(char)cbuf[2*i + 1]) << 8) + (short)cbuf[2*i];
    }
}

static void short_to_uchar_array(short *sbuf, unsigned char *cbuf, int len) {
    /* TODO: only correct when len is multiple of two */
    int i;
    for (i = 0; i < len / 2; i++)
    {
        cbuf[2*i] = (unsigned char)(sbuf[i] & 0x00ff);
        cbuf[2*i + 1] = (unsigned char)(sbuf[i] >> 8);
    }
}

static void uint32_to_le_array(unsigned char *buffer, unsigned int val) {
    unsigned int i;
    unsigned int mask = 0x000000FF;
    for (i = 0; i < sizeof(val); i++)
    {
        buffer[i] = (unsigned char)(val & mask);
        val = val >> 8;
    }
}

static void be_to_le(unsigned char *buf, int len) {
    int i;
    unsigned char tmp;
    for( i=0 ; i < len ; i+=2)
    {
        tmp = buf[i];
        buf[i] = buf[i+1];
        buf[i+1] = tmp;
    }
}

static int seq_code_to_num(int code) {
    if (code == SEQ_CODE_0)
        return 0;
    else if (code == SEQ_CODE_1)
        return 1;
    else if (code == SEQ_CODE_2)
        return 2;
    else if (code == SEQ_CODE_3)
        return 3;
    else
        return -1;
}

#ifdef DEBUG_PRINT_SCO_PACKET
/*
 * Debug function to print a buffer in hexadecimal form with adding ASCII
 * codes at the end.
 */
static void dump_buffer(const unsigned char* buf, unsigned n) {
    /* Number of ASCII characters following HEX data */
    #define DUMP_BUFFER_CHAR_LEN    16

    /* Length of the buffer for HEX data:
          3 - two charcaters for single HEX plus one space */
    #define DUMP_BUFFER_HEX_LEN     (DUMP_BUFFER_CHAR_LEN * 3)

    /* Size of the buffer/string to hold full line (HEX + ASCII):
         2 - space between HEX and ASCII data in the string;
         1 - placeholder for null terminated character */
    #define DUMP_BUFFER_LINE_LEN    (DUMP_BUFFER_HEX_LEN + 2 + \
                                     DUMP_BUFFER_CHAR_LEN + 1)

    if (buf != NULL && n > 0){
        char str[DUMP_BUFFER_LINE_LEN] = { 0 };
        unsigned int i = 0;
        unsigned int pos_hex = 0;
        unsigned int pos_char = 0;

        memset(str, ' ', DUMP_BUFFER_LINE_LEN);

        for (i = 0; i < n; i++){
            pos_hex += sprintf(str + pos_hex, "%02X ", buf[i]);
            pos_char += sprintf(str + DUMP_BUFFER_HEX_LEN + 1 + pos_char,
                                "%c",
                                isprint(buf[i]) ? buf[i] : '.');

            if (((i+1) % DUMP_BUFFER_CHAR_LEN) == 0 || (i+1) == n) {
                str[pos_hex] = ' ';
                LOGI("%s\n", str);
                memset(str, ' ', DUMP_BUFFER_LINE_LEN);
                pos_hex = pos_char = 0;
            }
        }
    }
}
#endif /* DEBUG_PRINT_SCO_PACKET */

#ifdef DEBUG
/*
 * Debug function to dump decoded PCM data into a wav file, so it can be played back on PC
 */
static void write_wav_file() {
    unsigned char *stream = NULL;
    struct stat st;
    int i, streamlen;
    unsigned char header[WAV_HEADER_SIZE] = { 0 };
    unsigned int size;

    do {
        if (stat(raw_file, &st) < 0) {
            LOGE("Can't get size of file %s: %s\n", raw_file, strerror(errno));
            break;
        }

        stream = malloc(st.st_size);

        if (!stream) {
            LOGE("Can't allocate memory for %s: %s\n", raw_file, strerror(errno));
            break;
        }

        raw_fd = open(raw_file, O_RDONLY);
        if (raw_fd < 0) {
            LOGE("Can't open file %s: %s\n", raw_file, strerror(errno));
            break;
        }

        if (read(raw_fd, stream, st.st_size) != st.st_size) {
            LOGE("Can't read content of %s: %s\n", raw_file, strerror(errno));
            close(raw_fd); raw_fd = -1;
            break;
        }

        close(raw_fd);
        raw_fd = -1;

        streamlen = st.st_size;

        wav_fd = open(wav_file, O_WRONLY | O_CREAT | O_TRUNC, O_WRONLY);
        if (wav_fd < 0) {
            LOGE("Can't allocate memory for %s: %s\n", wav_file, strerror(errno));
            break;
        }

        /* Compute chunk sizes for wav header */
        size = streamlen + WAV_HEADER_SIZE - sizeof(CHUNK_ID) - sizeof(chunk_size);
        uint32_to_le_array(chunk_size, size);

        size = streamlen;
        uint32_to_le_array(subchunk2_size, size);

        i = 0;

        memcpy(header + i, CHUNK_ID, sizeof(CHUNK_ID));
        i += sizeof(CHUNK_ID);

        memcpy(header + i, chunk_size, sizeof(chunk_size));
        i += sizeof(chunk_size);

        memcpy(header + i, FORMAT, sizeof(FORMAT));
        i += sizeof(FORMAT);

        memcpy(header + i, SUBCHUNK1_ID, sizeof(SUBCHUNK1_ID));
        i += sizeof(SUBCHUNK1_ID);

        memcpy(header + i, SUBCHUNK1_SIZE, sizeof(SUBCHUNK1_SIZE));
        i += sizeof(SUBCHUNK1_SIZE);

        memcpy(header + i, AUDIO_FORMAT, sizeof(AUDIO_FORMAT));
        i += sizeof(AUDIO_FORMAT);

        memcpy(header + i, NUM_CHANNELS, sizeof(NUM_CHANNELS));
        i += sizeof(NUM_CHANNELS);

        memcpy(header + i, SAMPLE_RATE, sizeof(SAMPLE_RATE));
        i += sizeof(SAMPLE_RATE);

        memcpy(header + i, BYTE_RATE, sizeof(BYTE_RATE));
        i += sizeof(BYTE_RATE);

        memcpy(header + i, BLOCK_ALIGN, sizeof(BLOCK_ALIGN));
        i += sizeof(BLOCK_ALIGN);

        memcpy(header + i, BITS_PER_SAMPLE, sizeof(BITS_PER_SAMPLE));
        i += sizeof(BITS_PER_SAMPLE);

        memcpy(header + i, SUBCHUNK2_ID, sizeof(SUBCHUNK2_ID));
        i += sizeof(SUBCHUNK2_ID);

        memcpy(header + i, subchunk2_size, sizeof(subchunk2_size));
        i += sizeof(subchunk2_size);

        if (write(wav_fd, header, WAV_HEADER_SIZE) < WAV_HEADER_SIZE) {
            LOGE("Failed to write wav header\n");
            break;
        }

        if (write(wav_fd, stream, streamlen) < streamlen) {
            LOGE("Failed to write wav data\n");
            break;
        }
    } while (0);

    if (wav_fd >= 0) {
        close(wav_fd);
        wav_fd = -1;
    }

    free(stream);
    stream = NULL;
}
#endif  /* DEBUG */

/*
 * Clears main buffer, should be called when all data is sent to client
 */
static void clear_main_buf() {
    /* Trim only if decode enabled */
    if (decode_enabled)
        head = ntrim;
    else
        head = 0;

    LOGI("Clear buffer\n");

    tail = 0;

#ifdef _METRICS
    /* Reset metrics statistics */
    mtotal_written = 0;
#endif

}

/*
 * Writes given data to main buffer
 */
static void write_to_main_buf(unsigned char *buf, unsigned int len, int good)
{
    if (decode_enabled) {
        /* Decoded audio is in big-endian format. Convert to little endian. */
        be_to_le(buf, len);

        if (plc_enabled) {
            uchar_to_short_array(buf, plc_in, len);

            if (good) {
                PLC_good_frame(&plc_state, plc_in, plc_out);
            } else {
                PLC_bad_frame(&plc_state, plc_in, plc_out);
            }

            short_to_uchar_array(plc_out, buf, len);
        }

#ifdef DEBUG
        /* Dump to file */
        if (raw_fd >= 0) {
            if (write(raw_fd, buf, len) < 0) {
                LOGE("Failed to write to %s\n", raw_file);
                close(raw_fd);
                raw_fd = -1;
            }
        }
#endif /* DEBUG */
    }

    /* Truncate audio if buffer full */
    if (tail + len <= sizeof(main_buf)) {
        memcpy(main_buf + tail, buf, len);
        tail += len;
    }
}

/*
 * Opens SCO socket to get mSBC data from.
 * Returns file descriptor of the socket (positive number) if the socket was opened successfully,
 * or STATUS_SCO_FAILED [-3] otherwise.
 */
static int open_sco_socket(const char *path) {
    int ret = -1;
    int sco_fd = -1;
    int len = SCO_MAX_BUFFER_SZ;
    int retry = 0;

    LOGW("SCO_UART: connecting to %s\n", path);

    sco_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    if (sco_fd < 0) {
	    LOGE("SCO_UART: failed to create the socket (%d)\n", strerror(errno));
	    return STATUS_SCO_FAILED;
    }

    while (socket_local_client_connect(sco_fd, path,
            ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM) < 0) {
        LOGE("SCO_UART: failed to connect (%s) retry %d\n", strerror(errno), ++retry);
        usleep(100000);
        if (retry == 10) {
#ifdef _METRICS
            ALOG(LOG_INFO, "metrics.VSBTRD", "MMSW:VSBTRD:OpenSCOSocketConnErr=1;CT;1:NR");
#endif
            LOGE("SCO_UART: failed to connect (%s)\n", strerror(errno));
            close(sco_fd);
            return STATUS_SCO_FAILED;
        }
    }

    ret = setsockopt(sco_fd, SOL_SOCKET, SO_RCVBUF, (char*)&len, (int)sizeof(len));

    /* Only issue warning if failed */
    if (ret < 0)
        LOGW("SCO_UART: setsockopt failed (%s)\n", strerror(errno));

    LOGW("SCO_UART: connected to stack fd = %d\n", sco_fd);

    return sco_fd;
}

/*
 * Handler for mSBC data from SCO socket. The function decodes mSBC to PCM if decoder is enabled,
 * applies PLC (if enabled) and writes decoded data to main buffer.
 * Returns:
 *      STATUS_SUCCESS - data was handled successfully
 *      STATUS_SCO_DATA_FAILED - data handling failed
 */
static inline int handle_msbc_data(unsigned char *sco_packet, unsigned int data_len) {
    int ret = STATUS_SUCCESS;
    unsigned char *msbc_data = NULL;
    uint32_t msbc_data_len   = 0;
 
    msbc_data = sco_packet + MSBC_SYNC_OFFSET;
    msbc_data_len = data_len - MSBC_SYNC_OFFSET;

#ifdef DEBUG
    /* Dump to file */
    if (msbc_fd >= 0) {
	if (msbc_data == NULL) {
	    LOGE("msbc_data is NULL!");
	} else if (msbc_data_len <= 0) {
	    LOGE("msbc_data_len <= 0");
	} else if (write(msbc_fd, msbc_data, msbc_data_len) < 0) {
            LOGE("Failed to write (len=%d) to %s, fd=%d\n", msbc_data_len, msbc_file, msbc_fd);
            close(msbc_fd);
            msbc_fd = -1;
        }
    }
#endif /* DEBUG */

    /* Only handle frames containing mSBC audio */
    if (msbc_data == NULL || msbc_data_len <= 0 || msbc_data[0] != MSBC_SYNC_CODE) {
        LOGW("Skip non mSBC frame!\n");

	// for testing script 
	if (msbc_data[0] != MSBC_SYNC_CODE) {
	    LOGE("Bad frame received: doesn't contain MSBC audio, drop packet!");
	}

        return STATUS_SCO_DATA_FAILED;
    }


    if (!decode_enabled) {
        write_to_main_buf(msbc_data, msbc_data_len, 0);
    } else {
        unsigned char pcm_buf[DECODE_BUF_SIZE] = { 0 };
        unsigned int len = 0;
        int framelen = 0;

        if (plc_enabled) {
            int seq_num = 0;
            int num_lost, seq_code, i;
            seq_code = *((uint8_t *)sco_packet + SCO_SEQ_OFFSET) >> 4;
            seq_num = seq_code_to_num(seq_code);

            if (seq_num < 0) {
                LOGE("Bad sequence code!: %x\n", seq_code);
                return STATUS_SCO_DATA_FAILED;
            }

            /* Perform PLC for lost packets */
            num_lost = (seq_num - last_seq_num - 1 + NUM_SEQ_CODES) % NUM_SEQ_CODES;
            for (i = 0; i < num_lost; i++) {
                LOGI("Lost packet detected! Performing packet loss concealment...\n");
                framelen = sbc_decode(&sbc, indices0, sizeof(indices0), pcm_buf, sizeof(pcm_buf), &len);
                write_to_main_buf(pcm_buf, len, 0);
            }

            /* Update seq num */
            last_seq_num = seq_num;
        }

        /* Process received good frame */
        framelen = sbc_decode(&sbc, msbc_data, msbc_data_len, pcm_buf, sizeof(pcm_buf), &len);
        if (framelen > 0) {
            write_to_main_buf(pcm_buf, len, 1);
        } else {
            LOGW("Bad frame encountered during decoding\n");
            ret = STATUS_SCO_DATA_FAILED;
        }
    }

    return ret;
}

/*
 * Handler for incoming data from SCO socket.
 * Returns:
 *      STATUS_SCO_NO_DATA - there is no data to handle
 *      STATUS_SCO_DATA_FAILED - failed to process the data
 *      STATUS_SUCCESS - data was processed successfully
 */
static inline int handle_sco_packet(int sco_fd) {
    int ret = STATUS_SUCCESS;

    unsigned char packet[SCO_MAX_BUFFER_SZ * 2] = { 0 };
    int len = recv(sco_fd, packet, SCO_MAX_BUFFER_SZ, MSG_WAITALL);

    if (len < 0 || len < MSBC_SYNC_OFFSET) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            ret = STATUS_SCO_NO_DATA;
        } else {
            LOGE("Receive from SCO failed\n");
            ret = STATUS_SCO_DATA_FAILED;
        }
    } else {
        LOGD("Received SCO data [%d bytes]\n", len);
#ifdef DEBUG_PRINT_SCO_PACKET
        dump_buffer(packet, len);
#endif /* DEBUG_PRINT_SCO_PACKET */

        ret = handle_msbc_data(packet, len);
    }

    return ret;
}

/*
 * Writes data from main buffer to given Client file descriptor
 * Returns:
 *      STATUS_SUCCESS - data was written successfully
 *      STATUS_CLIENT_FAILED - client is not ready
 */
 static inline int write_to_client(int client_fd) {
    int ret = STATUS_SUCCESS;
    int written;

    if (client_fd < 0)
        return STATUS_CLIENT_FAILED;

    /* There is no data ready to write to client */
    if (tail < head)
        return STATUS_SUCCESS;

    if (first_write == 0) {
        LOGI("Sending first data stream to client, socket=%d\n", client_fd);
        first_write = 1;
    }

    written = send(client_fd, main_buf + head, tail - head, 0);

    if (written >= 0) {
        LOGD("Written to client [%d bytes]\n", written);
        head += written;
#ifdef _METRICS
        if (UINT_MAX - written >= mtotal_written) {
            /* No overflow could happen - so add up */
            mtotal_written += written;
        }
#endif
        if (head >= tail) {
            /* Reset position to avoid overflow */
            head = tail = 0;
        }
    } else {
        LOGW("Write to client failed\n");
        ret = STATUS_CLIENT_FAILED;
    }

    return ret;
}

/*
 * Closes given socket and resets its file descriptor
 */
static void close_socket(struct pollfd* socket_fd){
    if (socket_fd != NULL && socket_fd->fd != -1) {
        LOGI("Closing %d\n", socket_fd->fd);

        shutdown(socket_fd->fd, SHUT_RDWR);
        close(socket_fd->fd);
        socket_fd->fd = -1;
    }
}

/*
 * Closes all sockets in the given sockets array
 */
static void close_all_sockets(struct pollfd *fds, int nfds) {
    int j;

    for (j = 0; j < nfds; j++){
        close_socket(&fds[j]);
    }
}

/*
 * Handler for socket disconnection event.
 *
 * If client is disconnected then Client file descriptor will be closed, new one will be created
 * when new client is connected.
 * If SCO server is disconnected then current SCO file descriptor will be closed, new one will be
 * created when next client is connected.
 *
 * Returns:
 *      STATUS_SCO_CLOSED - SCO socket was closed, it should be reopened when new client
 *                          is connected
 *     STATUS_SUCCESS - disconnection event was handled successfully
 */
static int handle_disconnection_event(int id, struct pollfd fds[]){

    LOGI("handle_disconnection_event:fds[%d].fd=%d\n", id, fds[id].fd);

    /* SCO Connection broken */
    if (fds[id].fd == fds[ID_SCO].fd) {
        LOGE("SCO server disconnected,  fd=%d\n", fds[ID_SCO].fd);

        /* Closing SCO socket. It must be created on next Client connection */
        close_socket(&fds[ID_SCO]);
        return STATUS_SCO_CLOSED;
    } else if (fds[id].fd == fds[ID_CLIENT].fd) {
        LOGI("Client disconnected, fd=%d\n", fds[ID_CLIENT].fd);
#ifdef _METRICS
            ALOG(LOG_INFO, "metrics.VSBTRD", "MMSW:VSBTRD:ClientDisconnectedTotalWritten=%u;CT;1:NR", mtotal_written);
        if (mtotal_written > 0) {
            ALOG(LOG_INFO, "metrics.VSBTRD", "MMSW:VSBTRD:ClientValidData=1;CT;1:NR");
        } else {
            ALOG(LOG_INFO, "metrics.VSBTRD", "MMSW:VSBTRD:ClientInvalidData=1;CT;1:NR");
        }
#endif
        close_socket(&fds[ID_CLIENT]);
        first_write = 0;


        /* We know that the connection has terminated, clean up the server socket
         * as well. This is re-opened at beginning of each voice session.
         */
        close_socket(&fds[ID_SCO]);

        clear_main_buf();
    }

#ifdef DEBUG
    if (raw_fd >= 0) {
        close(raw_fd);
        raw_fd = -1;
    }

    if (msbc_fd >= 0) {
        close(msbc_fd);
        msbc_fd = -1;
    }

    if (decode_enabled)
        write_wav_file();
#endif  /* DEBUG */

    return STATUS_SUCCESS;
}

/*
 * Handler for socket connection event.
 *
 * The function accepts new client connection and initialises Client file descriptor, connection
 * with previous client will be closed if it's currently established.
 * New SCO socket will be opened if it doesn't exist.
 *
 * Returns:
 *      STATUS_CLIENT_FAILED - incoming client connection cannot be accepted
 *      STATUS_SCO_FAILED - SCO socket cannot be opened, critical error!
 *      STATUS_SUCCESS - connection event was handled successfully
 */
static int handle_connection_event(int id, struct pollfd fds[]){
    /* Disconnect current client, if exists */
    if (fds[ID_CLIENT].fd >= 0) {
        close_socket(&fds[ID_CLIENT]);
        LOGI("Incoming connection. Old client disconnected.\n");
    }

    /* Accept incoming connection */
    int client_fd = accept(fds[ID_SERVER].fd, NULL, NULL);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            LOGE("Accept failed\n");
        } else {
            LOGI("Cannot accept incoming connection [%s]\n", strerror(errno));
        }
        return STATUS_CLIENT_FAILED;
    }

    LOGI("New client connected. fd=%d \n", client_fd);

    /* Set socket as non-blocking */
    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    fds[ID_CLIENT].fd = client_fd;
    fds[ID_CLIENT].revents = 0;

    LOGI("handle_connection_event: fds[ID_SCO].fd=%d \n", fds[ID_SCO].fd);
    if (fds[ID_SCO].fd == -1) {
        int sco_fd = open_sco_socket(SCO_AUDIO_PATH);

        if (sco_fd < 0) {
            LOGE("SCO server connection error!\n");
            return STATUS_SCO_FAILED;
        }

        LOGE("Opened new SCO %d\n", sco_fd);

        fds[ID_SCO].fd = sco_fd;
        fds[ID_SCO].events = POLLIN;
        fds[ID_SCO].revents = 0;

        clear_main_buf();
    }

#ifdef DEBUG
    raw_fd = open(raw_file, O_WRONLY | O_CREAT | O_TRUNC, O_WRONLY);
    if (raw_fd < 0) {
        LOGW("Can't open %s for writing\n", raw_file);
    }

    msbc_fd = open(msbc_file, O_WRONLY | O_CREAT | O_TRUNC, O_WRONLY);
    if (msbc_fd < 0) {
        LOGW("Can't open %s for writing\n", msbc_file);
    }
#endif  /* DEBUG */

    /* Initialize PLC state*/
    InitPLC(&plc_state);
    last_seq_num = 3;

    return STATUS_SUCCESS;
}

/*
 * Implements main loop of the daemon to handle sockets events.
 */
static int start_main_loop(int server_fd) {
    /* 3 sockets to poll: SCO, Server, and one Client at a time*/
    struct pollfd fds[ID_ALL];

    /* Placeholder for SCO socket,  will be created when first client is connected */
    fds[ID_SCO].fd = -1; fds[ID_SCO].events = POLLIN; fds[ID_SCO].revents = 0;

    /* Placeholder for Server */
    fds[ID_SERVER].fd = server_fd; fds[ID_SERVER].events = POLLIN; fds[ID_SERVER].revents = 0;

    /* Placeholder for Client, will be created when a client is connected  */
    fds[ID_CLIENT].fd = -1; fds[ID_CLIENT].events = POLLIN; fds[ID_CLIENT].revents = 0;

    for (;;) {
        int i, n, ret = STATUS_SUCCESS;

        n = poll(fds, ID_ALL, -1);

        if (n <= 0)
            continue;

        for (i = 0; i < ID_ALL; i++) {
            if (fds[i].fd == -1 || fds[i].revents == 0)
                continue;

            if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                ret = handle_disconnection_event(i, fds);

                if (ret != STATUS_SUCCESS)
                    break;
            } else if ((fds[i].fd == server_fd) && (fds[i].revents & POLLIN)) {
                ret = handle_connection_event(i, fds);
                break;
            } else if (fds[i].fd == fds[ID_SCO].fd) {
                ret = handle_sco_packet(fds[i].fd);
            }
            else if (fds[i].fd == fds[ID_CLIENT].fd)
            {
                if (tail > 0) {
                    write_to_client(fds[ID_CLIENT].fd);
                }
            }

            if (fds[ID_CLIENT].fd >= 0) {
                if (tail > 0) {
                    /* Set POLLOUT flag if buffered data exists */
                    fds[ID_CLIENT].events = POLLOUT;
                } else {
                    /* Clear POLLOUT flag if no buffered data */
                    fds[ID_CLIENT].events = 0;
                }
            }
        }

        if (ret == STATUS_SCO_FAILED || ret == STATUS_SCO_CLOSED) {
            LOGE("SCO socket failure - restart the main loop");
            /* TODO: is this really the right and safe
            sequence for this scenario? */
            if (fds[ID_CLIENT].fd >= 0) {
                close_socket(&fds[ID_CLIENT]);
                fds[ID_CLIENT].events = 0;
                first_write = 0;
                clear_main_buf();
                InitPLC(&plc_state);
            }
            usleep(10000);
        }
    }

    close_all_sockets(fds, ID_ALL);

    return 0;
}

static void daemonize(void) {
    pid_t pid, sid;

    /* Check if already a daemon */
    if (getppid() == 1)
        return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        LOGE("Fork failed\n");
        exit(EXIT_FAILURE);
    }

    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        LOGI("Bluetooth Daemon started: PID = %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        LOGE("Set SID failed\n");
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

static void read_prop_str(const char *prop, const char * value) {
    int len;
    char buf[PROP_VALUE_MAX + 1] = { 0 };

    len = __system_property_get(prop, buf);
    buf[len] = '\0';
    if (len > 0) {
        strcpy(value, buf);
    }
}

static int read_prop(const char *prop) {
    int len;
    char buf[PROP_VALUE_MAX + 1] = { 0 };

    len = __system_property_get(prop, buf);
    buf[len] = '\0';

    LOGI("Property %s = %d\n", prop, atoi(buf));
    return atoi(buf);
}

/*
 * Creates Server sockets and binds it to an abstract address provided.
 * Returns the Server file descriptor if socket was created successfully or -1 otherwise.
 */
static int open_server_socket(const char *address) {
    int server_fd;
    unsigned int len, addr_len;
    struct sockaddr_un local;

    addr_len = 1 + strlen(address);

    if (addr_len > UNIX_PATH_MAX) {
        LOGE("Invalid address length\n");
        return -1;
    }

    /* Create server socket */
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        LOGE("Create UNIX socket failed\n");
        return -1;
    }

    /* Set socket as non-blocking */
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    local.sun_family = AF_UNIX;
    memset(local.sun_path, 0, UNIX_PATH_MAX);

    /* Use abstract socket address to avoid permissions issue for now.
       May have to use filesystem namespace to only allow connections from
       Android services and not directly from apps. */
    local.sun_path[0] = 0;
    memcpy(local.sun_path + 1, address, addr_len - 1);
    unlink(local.sun_path);
    len = addr_len + sizeof(local.sun_family);

    /* Bind socket to file path */
    if (bind(server_fd, (struct sockaddr *)&local, len) == -1) {
        LOGE("Bind to server address failed\n");
        return -1;
    }

    if (listen(server_fd, 1) == -1) {
        LOGE("Listen failed\n");
        return -1;
    }

    return server_fd;
}

int main(int argc, char *argv[]) {
    int server_fd = -1;
    int trim_length = 0;
    int ret = -1;
    char sock_path[PROP_VALUE_MAX + 1] = SOCK_PATH_DEFAULT;


    /* Open Server socket */
    read_prop_str(SYSTEM_PROPERTY_SOCKET, sock_path);
    server_fd = open_server_socket(sock_path);
    if (server_fd < 0) {
        LOGE("Failed to create SOCK_PATH [%s]\n", sock_path);
        return -1;
    }

    LOGI("Created SOCK_PATH [%s]\n", sock_path);

    daemonize();

    decode_enabled = read_prop(SYSTEM_PROPERTY_DECODE);
    plc_enabled = read_prop(SYSTEM_PROPERTY_PLC);
    trim_length = read_prop(SYSTEM_PROPERTY_TRIM);
    ntrim = trim_length * (SAMPLING_FREQ / 1000) * BYTES_PER_SAMPLE;

    LOGI("btremoted started:\n");
    LOGI("   Decode enabled: %s\n", decode_enabled ? "YES" : "NO");
    LOGI("      PLC enabled: %s\n", plc_enabled ? "YES" : "NO");
    LOGI("      Trim length: %d ms\n", trim_length);

    /* Initialize SBC decoder */
    sbc_init(&sbc, 0L);
    sbc.endian = SBC_BE;

    ret = start_main_loop(server_fd);

    sbc_finish(&sbc);

    LOGI("btremoted is finished!\n");

    return ret;
}
