/*
 * Copyright (C) 2012 Amazon.com Inc. or its affiliates.  All rights reserved.
 *
 */
#define DAEMON_NAME         "btremoted"

#define SCO_SEQ_OFFSET      1
#define MSBC_SYNC_OFFSET    2
#define MSBC_SYNC_CODE      0xAD
#define BUFFER_SIZE         320000 /* 10 seconds PCM at 16-bit, 16kHz, Mono */
#define DECODE_BUF_SIZE     256
#define SAMPLING_FREQ       16000
#define BYTES_PER_SAMPLE    2

#define SOCK_PATH_DEFAULT       "bluetooth"
#define SYSTEM_PROPERTY_SOCKET  "btremoted.socket"
#define SYSTEM_PROPERTY_DECODE  "btremoted.decode.enabled"
#define SYSTEM_PROPERTY_PLC     "btremoted.plc.enabled"
#define SYSTEM_PROPERTY_TRIM    "btremoted.trim_length"

/* Bluedroid SCO socket definitions */
#define SCO_AUDIO_PATH          "/data/misc/bluedroid/.btmic_data"
#define SCO_MAX_BUFFER_SZ       60

/*	Logging macros */
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, DAEMON_NAME, __VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, DAEMON_NAME, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, DAEMON_NAME, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN, DAEMON_NAME, __VA_ARGS__)


