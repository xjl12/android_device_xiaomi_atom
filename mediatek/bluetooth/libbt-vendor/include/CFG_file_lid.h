/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

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


/*******************************************************************************
 *
 * Filename:
 * ---------
 *   cfg_file_lid.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *    header file of main function
 *
 * Author:
 * -------
 *   Ning.F (MTK08139) 09/11/2008
 *
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 12 29 2012 donglei.ji
 * [ALPS00425279] [Need Patch] [Volunteer Patch] voice ui and password unlock feature check in
 * voice ui - NVRAM .
 *
 * 01 14 2011 hao.lin
 * [ALPS00138458] [Need Patch] [Volunteer Patch] OMA DM USB and SD format
 * <saved by Perforce>
 *
 * 12 24 2010 hao.lin
 * [ALPS00138458] [Need Patch] [Volunteer Patch] OMA DM USB and SD format
 * <saved by Perforce>
 *
 * 11 05 2010 renbang.jiang
 * [ALPS00134025] [Wi-Fi] move Wi-Fi NVRAM definition source file to project folder from common folder
 * .
 *
 * 07 10 2010 renbang.jiang
 * [ALPS00121785][Need Patch] [Volunteer Patch] use NVRAM to save Wi-Fi custom data
 * .
 *
 * 05 05 2010 saker.hsia
 * [ALPS00001913][BWCS] BT WiFi Single Antenna Feature check in Android 2.1
 * .
 *
 * 04 26 2010 renbang.jiang
 * [ALPS00005487][Wi-Fi] change Wi-Fi nvram LID
 * [ALPS00005487] [Wi-Fi] change Wi-Fi nvram LID
 * .
 *
 *    mtk02556
 * [DUMA00128675] [NVRAM] Add NVRAM SIM struct
 * Add NVRAM SIM LID
 *
 *    mtk02556
 * [DUMA00128426] [NVRAM] Reconstrcut ADC struct and add UUID struct
 * Add NVRAM UUID VER
 *
 * Jul 9 2009 mtk80306
 * [DUMA00122953] optimize nvram and change meta clean boot flag.
 * modify wifi version
 *
 * Apr 29 2009 mtk80306
 * [DUMA00116080] revise the customization of nvram
 * revise nvram customization
 *
 * Apr 15 2009 mtk80306
 * [DUMA00204156] Power on_it display start in mobile screen
 * add uart file
 *
 * Mar 21 2009 mtk80306
 * [DUMA00112158] fix the code convention.
 * fix the codeing convention.
 *
 * Mar 15 2009 mtk80306
 * [DUMA00111629] add camera nvram files
 * add camera nvram files
 *
 * Mar 9 2009 mtk80306
 * [DUMA00111088] nvram customization
 * nvram customization
 *
 * Feb 23 2009 mtk80306
 * [DUMA00109277] add meta _battery mode.
 *
 *
 * Feb 19 2009 mtk80306
 * [DUMA00109277] add meta _battery mode.
 * add meta_battery
 *
 * Jan 13 2009 mtk80306
 * [DUMA00107334] add nvram dll code
 *
 *
 * Jan 13 2009 mtk80306
 * [DUMA00107334] add nvram dll code
 * add nvram dll code.
 *
 * Dec 17 2008 mbj08139
 * [DUMA00105099] create meta code
 *
 *
 * Dec 8 2008 mbj08139
 * [DUMA00105099] create meta code
 *
 *
 * Nov 24 2008 mbj08139
 * [DUMA00105099] create meta code
 *
 *
 * Oct 29 2008 mbj08139
 * [DUMA00105099] create meta code
 *
 *
 *
 *
 *******************************************************************************/


#ifndef CFG_FILE_LID_H
#define CFG_FILE_LID_H



#define VER_LID(lid) lid##_VERNO lid
#define VER(lid)  lid##_VERNO


/* the definition of file LID */
typedef enum
{
	AP_CFG_FILE_VER_INFO_LID,
	AP_CFG_RDEB_FILE_BT_ADDR_LID,
//	AP_CFG_RDEB_FILE_WIFI_LID,
	AP_CFG_RDCL_FILE_AUXADC_LID,
	AP_CFG_RDCL_CAMERA_PARA_LID,
	AP_CFG_RDCL_CAMERA_3A_LID,
	AP_CFG_RDCL_CAMERA_SHADING_LID,
	AP_CFG_RDCL_CAMERA_DEFECT_LID,
	AP_CFG_RDCL_CAMERA_SENSOR_LID,  //10
	AP_CFG_RDCL_CAMERA_LENS_LID,
	AP_CFG_RDCL_UART_LID,
	//AP_CFG_RDCL_UUID_LID,
	//AP_CFG_RDCL_SIM_LID,
	AP_CFG_RDCL_FACTORY_LID,
    AP_CFG_RDCL_BWCS_LID,
    AP_CFG_RDCL_HWMON_ACC_LID,
    AP_CFG_RDCL_HWMON_GYRO_LID,
//    AP_CFG_RDEB_WIFI_CUSTOM_LID,
    AP_CFG_RDEB_OMADM_USB_LID,
	AP_CFG_RDCL_FILE_VOICE_RECOGNIZE_PARAM_LID,
	AP_CFG_RDCL_FILE_AUDIO_AUDENH_CONTROL_OPTION_PAR_LID,
	AP_CFG_RDCL_FILE_AUDIO_VOIP_PAR_LID,
	AP_CFG_RDCL_HWMON_PS_LID,
	AP_CFG_FILE_MDTYPE_LID,
	AP_CFG_FILE_EXT_MDTYPE_LID,
	AP_CFG_RDCL_FILE_SDIO_LID,
	AP_CFG_RDCL_CAMERA_VERSION_LID,
	AP_CFG_RDCL_CAMERA_FEATURE_LID,
	AP_CFG_RDCL_CAMERA_GEOMETRY_LID,
	AP_CFG_RDCL_FILE_MD_SBP_LID,
    AP_CFG_RDCL_CAMERA_SHADING2_LID,
    AP_CFG_RDCL_CAMERA_SHADING3_LID,
    AP_CFG_RDCL_CAMERA_SHADING4_LID,
    AP_CFG_RDCL_CAMERA_SHADING5_LID,
    AP_CFG_RDCL_CAMERA_SHADING6_LID,
    AP_CFG_RDCL_CAMERA_SHADING7_LID,
    AP_CFG_RDCL_CAMERA_SHADING8_LID,
    AP_CFG_RDCL_CAMERA_SHADING9_LID,
    AP_CFG_RDCL_CAMERA_SHADING10_LID,
    AP_CFG_RDCL_CAMERA_SHADING11_LID,
    AP_CFG_RDCL_CAMERA_SHADING12_LID,
    AP_CFG_RDCL_CAMERA_PLINE_LID,
    AP_CFG_RDCL_CAMERA_PLINE2_LID,
    AP_CFG_RDCL_CAMERA_PLINE3_LID,
    AP_CFG_RDCL_CAMERA_PLINE4_LID,
    AP_CFG_RDCL_CAMERA_PLINE5_LID,
    AP_CFG_RDCL_CAMERA_PLINE6_LID,
    AP_CFG_RDCL_CAMERA_PLINE7_LID,
    AP_CFG_RDCL_CAMERA_PLINE8_LID,
    AP_CFG_RDCL_CAMERA_PLINE9_LID,
    AP_CFG_RDCL_CAMERA_PLINE10_LID,
    AP_CFG_RDCL_CAMERA_PLINE11_LID,
    AP_CFG_RDCL_CAMERA_PLINE12_LID,
    AP_CFG_RDCL_CAMERA_AF_LID,
    AP_CFG_RDCL_CAMERA_FLASH_CALIBRATION_LID,


	AP_CFG_CUSTOM_BEGIN_LID,  ///Max LID.
}CFG_FILE_LID;
/*
int iCustomBeginLID=AP_CFG_CUSTOM_BEGIN_LID;
extern int iCustomBeginLID;
int iFileVerInfoLID=AP_CFG_FILE_VER_INFO_LID;
extern int iFileVerInfoLID;
int iFileBTAddrLID=AP_CFG_RDEB_FILE_BT_ADDR_LID;
extern int iFileBTAddrLID;

#if 0
int iFileWIFILID=AP_CFG_RDEB_FILE_WIFI_LID;
extern int iFileWIFILID;
int iFileCustomWIFILID=AP_CFG_RDEB_WIFI_CUSTOM_LID;
extern int iFileCustomWIFILID;
#endif

int iFileAuxADCLID=AP_CFG_RDCL_FILE_AUXADC_LID;
extern int iFileAuxADCLID;
*/
////if file num changed, the define of CFG_FILE_VER_FILE_REC_TOTAL should be changed too  ///
#define CFG_FILE_VER_FILE_REC_TOTAL   936

/* verno of data items */
/* ver file version */
/* md type file version*/
#define AP_CFG_FILE_MDTYPE_LID_VERNO			"000"
#define AP_CFG_FILE_EXT_MDTYPE_LID_VERNO			"000"
#define AP_CFG_FILE_VER_INFO_LID_VERNO				"000"
/* BT file version */
#define AP_CFG_RDEB_FILE_BT_ADDR_LID_VERNO			"001"
/* WIFI file version */
//#define AP_CFG_RDEB_FILE_WIFI_LID_VERNO				"000"
/* WIFI MAC addr file version */
//#define AP_CFG_RDCL_FILE_WIFI_ADDR_LID_VERNO		"000"
/* ADC file version */
#define AP_CFG_RDCL_FILE_AUXADC_LID_VERNO			"000"

/* camera nvram files */
#define AP_CFG_RDCL_CAMERA_PARA_LID_VERNO			"000"
#define AP_CFG_RDCL_CAMERA_3A_LID_VERNO				"000"
#define AP_CFG_RDCL_CAMERA_SHADING_LID_VERNO		"000"
#define AP_CFG_RDCL_CAMERA_DEFECT_LID_VERNO			"000"
#define AP_CFG_RDCL_CAMERA_SENSOR_LID_VERNO			"000"
#define AP_CFG_RDCL_CAMERA_LENS_LID_VERNO			"000"
#define AP_CFG_RDCL_CAMERA_VERSION_LID_VERNO        "000"
#define AP_CFG_RDCL_CAMERA_FEATURE_LID_VERNO        "000"
#define AP_CFG_RDCL_CAMERA_GEOMETRY_LID_VERNO        "000"
#define AP_CFG_RDCL_CAMERA_PLINE_LID_VERNO        "000"
#define AP_CFG_RDCL_CAMERA_AF_LID_VERNO        "000"
#define AP_CFG_RDCL_CAMERA_FLASH_CALIBRATION_LID_VERNO        "000"

#define AP_CFG_RDCL_UART_LID_VERNO                  "000"

#define AP_CFG_RDCL_UUID_LID_VERNO                  "000"

#define AP_CFG_RDCL_SIM_LID_VERNO                   "000"

/* For the factory mode info. used */
#define AP_CFG_RDCL_FACTORY_LID_VERNO               "000"

#define AP_CFG_RDCL_BWCS_LID_VERNO				    "000"

#define AP_CFG_RDCL_HWMON_ACC_LID_VERNO             "000"
#define AP_CFG_RDCL_HWMON_GYRO_LID_VERNO					"000"
#define AP_CFG_RDCL_HWMON_PS_LID_VERNO					"000"
//#define AP_CFG_RDEB_WIFI_CUSTOM_LID_VERNO				"000"
#define AP_CFG_RDEB_OMADM_USB_LID_VERNO				"000"
#define AP_CFG_RDCL_FILE_VOICE_RECOGNIZE_PARAM_LID_VERNO "000"

/* audio audenh control custom file version*/
#define AP_CFG_RDCL_FILE_AUDIO_AUDENH_CONTROL_OPTION_PAR_LID_VERNO "000"

//audio VoIP params custom file version
#define AP_CFG_RDCL_FILE_AUDIO_VOIP_PAR_LID_VERNO "000"
/* SDIO file version */
#define AP_CFG_RDCL_FILE_SDIO_LID_VERNO            "001"

/* MD SBP fle version */
#define AP_CFG_RDCL_FILE_MD_SBP_LID_VERNO         "001"

#endif /* CFG_FILE_LID_H */

