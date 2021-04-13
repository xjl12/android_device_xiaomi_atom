/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2014. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <cutils/properties.h>

/* use nvram */
#include "CFG_BT_File.h"
#include "CFG_BT_Default.h"
#include "CFG_file_lid.h"
#include "libnvram.h"

#include "bt_mtk.h"
#include "cust_bt.h"

/**************************************************************************
 *                    G L O B A L   D E F I N E S                         *
***************************************************************************/

#define COMBO_IOC_MAGIC         0xb0
#define COMBO_IOCTL_FW_ASSERT   _IOWR(COMBO_IOC_MAGIC, 0, int)
#define COMBO_IOCTL_BT_SET_PSM  _IOWR(COMBO_IOC_MAGIC, 1, bool)
/**************************************************************************
 *                  G L O B A L   V A R I A B L E S                       *
***************************************************************************/

const bt_vendor_callbacks_t *bt_vnd_cbacks = NULL;
static int  bt_fd = -1;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S                 *
***************************************************************************/

extern BOOL BT_InitDevice(UINT32 chipId, PUCHAR pucNvRamData);
extern BOOL BT_DeinitDevice(VOID);
extern VOID BT_Cleanup(VOID);

/**************************************************************************
 *                          F U N C T I O N S                             *
***************************************************************************/

static BOOL is_memzero(unsigned char *buf, int size)
{
    int i;
    for (i = 0; i < size; i++) {
        if (*(buf+i) != 0) return FALSE;
    }
    return TRUE;
}

/* Register callback functions to libbt-hci.so */
void set_callbacks(const bt_vendor_callbacks_t *p_cb)
{
    bt_vnd_cbacks = p_cb;
}

/* Cleanup callback functions previously registered */
void clean_callbacks(void)
{
    bt_vnd_cbacks = NULL;
}

/* Initialize UART port */
int init_uart(void)
{
    LOG_TRC();

    bt_fd = open(CUST_BT_SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (bt_fd < 0) {
        LOG_ERR("Can't open %s (%s), errno[%d]\n", CUST_BT_SERIAL_PORT, strerror(errno), errno);
        return -1;
    }

    return bt_fd;
}

/* Close UART port previously opened */
void close_uart(void)
{
    if (bt_fd >= 0) close(bt_fd);
    bt_fd = -1;
}


static int bt_get_combo_id(unsigned int *pChipId)
{
    int  chipId_ready_retry = 0;
    char chipId_val[PROPERTY_VALUE_MAX];

    do {
        if (property_get("persist.mtk.wcn.combo.chipid", chipId_val, NULL) &&
            0 != strcmp(chipId_val, "-1")) {
            *pChipId = (unsigned int)strtoul(chipId_val, NULL, 16);
            break;
        }
        else {
            chipId_ready_retry ++;
            usleep(500000);
        }
    } while(chipId_ready_retry < 10);

    LOG_DBG("Get combo chip id retry %d\n", chipId_ready_retry);
    if (chipId_ready_retry >= 10) {
        LOG_DBG("Invalid combo chip id!\n");
        return -1;
    }
    else {
        LOG_DBG("Combo chip id %x\n", *pChipId);
        return 0;
    }
}

static int bt_read_nvram(unsigned char *pucNvRamData)
{
    F_ID bt_nvram_fd;
    int rec_size = 0;
    int rec_num = 0;
    ap_nvram_btradio_struct bt_nvram;

    int nvram_ready_retry = 0;
    char nvram_init_val[PROPERTY_VALUE_MAX];

    LOG_TRC();

    /* Sync with Nvram daemon ready */
    do {
        if (property_get("service.nvram_init", nvram_init_val, NULL) &&
            0 == strcmp(nvram_init_val, "Ready"))
            break;
        else {
            nvram_ready_retry ++;
            usleep(500000);
        }
    } while(nvram_ready_retry < 10);

    LOG_DBG("Get NVRAM ready retry %d\n", nvram_ready_retry);
    if (nvram_ready_retry >= 10) {
        LOG_ERR("Get NVRAM restore ready fails!\n");
        return -1;
    }

    bt_nvram_fd = NVM_GetFileDesc(AP_CFG_RDEB_FILE_BT_ADDR_LID, &rec_size, &rec_num, ISREAD);
    if (bt_nvram_fd.iFileDesc < 0) {
        LOG_WAN("Open BT NVRAM fails errno %d\n", errno);
        return -1;
    }

    if (rec_num != 1) {
        LOG_ERR("Unexpected record num %d", rec_num);
        NVM_CloseFileDesc(bt_nvram_fd);
        return -1;
    }

    if (rec_size != sizeof(ap_nvram_btradio_struct)) {
        LOG_ERR("Unexpected record size %d ap_nvram_btradio_struct %zu",
                rec_size, sizeof(ap_nvram_btradio_struct));
        NVM_CloseFileDesc(bt_nvram_fd);
        return -1;
    }

    if (read(bt_nvram_fd.iFileDesc, &bt_nvram, rec_num*rec_size) < 0) {
        LOG_ERR("Read NVRAM fails errno %d\n", errno);
        NVM_CloseFileDesc(bt_nvram_fd);
        return -1;
    }

    NVM_CloseFileDesc(bt_nvram_fd);
    memcpy(pucNvRamData, &bt_nvram, sizeof(ap_nvram_btradio_struct));

    return 0;
}

/* MTK specific chip initialize process */
int mtk_fw_cfg(void)
{
    unsigned int chipId = 0;
    unsigned char ucNvRamData[sizeof(ap_nvram_btradio_struct)] = {0};

    LOG_TRC();

    /* Get combo chip id */
    if (bt_get_combo_id(&chipId) < 0) {
        LOG_ERR("Get combo chip id fails\n");
        return -1;
    }

    /* Read NVRAM data */
    if ((bt_read_nvram(ucNvRamData) < 0) ||
          is_memzero(ucNvRamData, sizeof(ap_nvram_btradio_struct))) {
        LOG_WAN("Read NVRAM data fails or NVRAM data all zero!!\n");
        LOG_WAN("Use %04x default value\n", chipId);
        switch (chipId) {
          case 0x6628:
            /* Use MT6628 default value */
            memcpy(ucNvRamData, &stBtDefault_6628, sizeof(ap_nvram_btradio_struct));
            break;
          case 0x6630:
            /* Use MT6630 default value */
            memcpy(ucNvRamData, &stBtDefault_6630, sizeof(ap_nvram_btradio_struct));
            break;
          case 0x6632:
            /* Use MT6632 default value */
            memcpy(ucNvRamData, &stBtDefault_6632, sizeof(ap_nvram_btradio_struct));
            break;
          case 0x8163:
          case 0x8127:
          case 0x8167:
          case 0x6582:
          case 0x6592:
          case 0x6752:
          case 0x0321:
          case 0x0335:
          case 0x0337:
          case 0x6580:
          case 0x6570:
          case 0x6755:
          case 0x6797:
          case 0x6757:
          case 0x6759:
          case 0x6763:
          case 0x6758:
          case 0x6739:
          case 0x6771:
          case 0x6775:
            /* Use A-D die default value */
            memcpy(ucNvRamData, &stBtDefault_consys, sizeof(ap_nvram_btradio_struct));
            break;
          default:
            LOG_WAN("Unknown combo chip id: %04x\n", chipId);
            return -1;
        }
    }

    LOG_WAN("[BDAddr %02x-%02x-%02x-%02x-%02x-%02x][Voice %02x %02x][Codec %02x %02x %02x %02x] \
            [Radio %02x %02x %02x %02x %02x %02x][Sleep %02x %02x %02x %02x %02x %02x %02x][BtFTR %02x %02x] \
            [TxPWOffset %02x %02x %02x][CoexAdjust %02x %02x %02x %02x %02x %02x] \
            [Radio_ext %02x %02x][TxPWOffset_ext %02x %02x %02x]\n",
            ucNvRamData[0], ucNvRamData[1], ucNvRamData[2], ucNvRamData[3], ucNvRamData[4], ucNvRamData[5],
            ucNvRamData[6], ucNvRamData[7],
            ucNvRamData[8], ucNvRamData[9], ucNvRamData[10], ucNvRamData[11],
            ucNvRamData[12], ucNvRamData[13], ucNvRamData[14], ucNvRamData[15], ucNvRamData[16], ucNvRamData[17],
            ucNvRamData[18], ucNvRamData[19], ucNvRamData[20], ucNvRamData[21], ucNvRamData[22], ucNvRamData[23], ucNvRamData[24],
            ucNvRamData[25], ucNvRamData[26],
            ucNvRamData[27], ucNvRamData[28], ucNvRamData[29],
            ucNvRamData[30], ucNvRamData[31], ucNvRamData[32], ucNvRamData[33], ucNvRamData[34], ucNvRamData[35],
            ucNvRamData[36], ucNvRamData[37],
            ucNvRamData[38], ucNvRamData[39], ucNvRamData[40]);


    return (BT_InitDevice(chipId, ucNvRamData) == TRUE ? 0 : -1);
}

/* MTK specific deinitialize process */
int mtk_prepare_off(void)
{
    /*
    * On KK, BlueDroid adds BT_VND_OP_EPILOG procedure when BT disable:
    *   - 1. BT_VND_OP_EPILOG;
    *   - 2. In vendor epilog_cb, send EXIT event to bt_hc_worker_thread;
    *   - 3. Wait for bt_hc_worker_thread exit;
    *   - 4. userial close;
    *   - 5. vendor cleanup;
    *   - 6. Set power off.
    * On L, the disable flow is modified as below:
    *   - 1. userial Rx thread exit;
    *   - 2. BT_VND_OP_EPILOG;
    *   - 3. Write reactor->event_fd to trigger bt_hc_worker_thread exit
    *        (not wait to vendor epilog_cb and do nothing in epilog_cb);
    *   - 4. Wait for bt_hc_worker_thread exit;
    *   - 5. userial close;
    *   - 6. Set power off;
    *   - 7. vendor cleanup.
    *
    * It seems BlueDroid does not expect Tx/Rx interaction with chip during
    * BT_VND_OP_EPILOG procedure, and also does not need to do it in a new
    * thread context (NE may occur in __pthread_start if bt_hc_worker_thread
    * has already exited).
    * So BT_VND_OP_EPILOG procedure may be not for chip deinitialization,
    * do nothing, just notify success.
    *
    * [FIXME!!]How to do if chip deinit is needed?
    */
    //return (BT_DeinitDevice() == TRUE ? 0 : -1);
    if (bt_vnd_cbacks) {
        bt_vnd_cbacks->epilog_cb(BT_VND_OP_RESULT_SUCCESS);
    }
    return 0;
}
#if 0
int mtk_set_fw_assert(uint32_t reason)
{
    if (bt_fd < 0) {
        LOG_ERR("Invalid bt fd! BT is not opened yet\n");
        return -1;
    }
    if (ioctl(bt_fd, COMBO_IOCTL_FW_ASSERT, reason) < 0) {
        LOG_ERR("Set FW assert fails! %s(%d)\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}

int mtk_set_psm_control(bool enable)
{
    if (bt_fd < 0) {
        LOG_ERR("Invalid bt fd!\n");
        return -1;
    }
    if (ioctl(bt_fd, COMBO_IOCTL_BT_SET_PSM, enable) < 0) {
        LOG_ERR("Set COMBO_IOCTL_BT_SET_PSM fails enable = %d, %s, errno[%d]\n", enable, strerror(errno), errno);
        return errno;
    }
    return 0;
}
#endif
/* Cleanup driver resources, e.g thread exit */
void clean_resource(void)
{
    BT_Cleanup();
}
