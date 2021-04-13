/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2008
 *
 *  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 *  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
 *  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 *  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 *  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 *  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
 *  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
 *  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 *  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 *  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 *  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
 *  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
 *  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
 *  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
 *  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
 *  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   Libnvram_sec.h
 *
 * Project:
 * --------
 *   YuSu
 *
 * Description:
 * ------------
 *
 *
 *
 * Author:
 * -------
 *  Jian lin (mtk81139)
 *
 ****************************************************************************/


#ifndef __NVRAM_LIB_SEC_H
#define __NVRAM_LIB_SEC_H

#include "libnvram.h"
#include "CFG_file_lid.h"
#include "Custom_NvRam_LID.h"
//#include "sec_nvram.h"



#ifdef __cplusplus
extern "C"
{
#endif


#define BOOT_MODE_FILE "/sys/class/BOOT/BOOT/boot/boot_mode"
#define META_MODE '1'
#define ADVMETA_MODE '5'
#define ENCODE  true
#define DECODE  false

static bool NVM_InProtectList(int file_lid,int protect_type);
static bool NVM_CheckBootMode();
bool NVM_Enc_Dnc_File(int file_lid, bool IS_end);
bool NVM_CheckWritePermission(bool IsRead, int file_lid);
static const TCFG_FILE* NVM_GetCfgFileTableForSec(int file_lid);


typedef struct
{
   int lid;
   unsigned char meta_mode_write;
   unsigned char security_enable;
} TCFG_FILE_PROTECT;




/* used for META library */
#define NVRAM_CIPHER_LEN (16)

/******************************************************************************
 *  MODEM CONTEXT FOR BOTH USER SPACE PROGRAM AND KERNEL MODULE
 ******************************************************************************/
typedef struct
{
    unsigned char data[NVRAM_CIPHER_LEN];
    unsigned int ret;
    
} META_CONTEXT;

/******************************************************************************
 *  EXPORT FUNCTIONS
 ******************************************************************************/
int __attribute__((weak)) sec_nvram_enc (META_CONTEXT *meta_ctx);
int __attribute__((weak)) sec_nvram_dec (META_CONTEXT *meta_ctx);


#ifdef __cplusplus
}
#endif


#endif //__NVRAM_LIB_H
