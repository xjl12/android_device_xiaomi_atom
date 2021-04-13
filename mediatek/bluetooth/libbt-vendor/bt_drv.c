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

#include "bt_vendor_lib.h"
#include "bt_mtk.h"

//=============== I N T E R F A C E S =======================

int mtk_bt_init(const bt_vendor_callbacks_t* p_cb, UNUSED_ATTR unsigned char *local_bdaddr)
{
    LOG_TRC();
    set_callbacks(p_cb);
    return 0;
}

int mtk_bt_op(bt_vendor_opcode_t opcode, void *param)
{
    int ret = 0;

    switch(opcode)
    {
      case BT_VND_OP_POWER_CTRL:
	LOG_DBG("BT_VND_OP_POWER_CTRL %d\n", *((int*)param));
	/* DO NOTHING on combo chip */
	break;

      case BT_VND_OP_USERIAL_OPEN:
	LOG_DBG("BT_VND_OP_USERIAL_OPEN\n");

	((int*)param)[0] = init_uart();
	ret = 1; /* CMD/EVT/ACL-In/ACL-Out via the same fd */
	break;

      case BT_VND_OP_USERIAL_CLOSE:
	LOG_DBG("BT_VND_OP_USERIAL_CLOSE\n");
	close_uart();
	break;

      case BT_VND_OP_FW_CFG:
	LOG_DBG("BT_VND_OP_FW_CFG\n");
	ret = mtk_fw_cfg();
	break;

      case BT_VND_OP_GET_LPM_IDLE_TIMEOUT:
	LOG_DBG("BT_VND_OP_GET_LPM_IDLE_TIMEOUT\n");
	*((uint32_t*)param) = 5000; //ms
	break;

      case BT_VND_OP_LPM_SET_MODE:
	LOG_DBG("BT_VND_OP_LPM_SET_MODE %d\n", *((uint8_t*)param));
	break;

      case BT_VND_OP_LPM_WAKE_SET_STATE:
	LOG_DBG("BT_VND_OP_LPM_WAKE_SET_STATE\n");
	break;

      case BT_VND_OP_EPILOG:
	LOG_DBG("BT_VND_OP_EPILOG\n");
	ret = mtk_prepare_off();
	break;

      default:
	LOG_DBG("Unknown operation %d\n", opcode);
	ret = -1;
	break;
    }

    return ret;
}

void mtk_bt_cleanup()
{
    LOG_TRC();
    clean_resource();
    clean_callbacks();
    return;
}

const bt_vendor_interface_t BLUETOOTH_VENDOR_LIB_INTERFACE = {
    sizeof(bt_vendor_interface_t),
    mtk_bt_init,
    mtk_bt_op,
    mtk_bt_cleanup
};
