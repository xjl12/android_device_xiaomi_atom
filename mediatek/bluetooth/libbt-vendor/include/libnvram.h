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
 *   Libnvram.h
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
 *  Nick Huang (mtk02183)
 *
 ****************************************************************************/


#ifndef __NVRAM_LIB_H
#define __NVRAM_LIB_H

#include "CFG_file_public.h"
#include <stdbool.h>
#include <time.h>
#include <mtd/mtd-abi.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef bool
#define bool int
#define false 0
#define true 1
#endif

#define NVRAM_JOURNAL_HANDLE_SIZE	512

#define NVRAM_JOURNAL_MAGIC 0x5a5a7b7b
#define NVRAM_MISC_JOURNAL_MAGIC 0x4D52564E
#define MAX_FUNCTION_NAME_LENGTH  48
//#define LOG_BLOCK_NUMBER   8
//#define RESERVE_BLOCK_NUMBER    8
#define MAX_LOG_LENGTH  (NVRAM_JOURNAL_HANDLE_SIZE - 5 * sizeof(unsigned int) - sizeof(time_t) - MAX_FUNCTION_NAME_LENGTH)
#define DEFAULT_LOG_LEVEL 1
#define NVRAM_JOURNAL_SUPER_PAGE  0
#define NVRAM_JOURNAL_LOG_PAGE  1
#define NVRAM_MNT_POINT "/nvram"
#define MISC_MNT_POINT "/misc"
#define PROINFO_MNT_POINT "/proinfo"
#define NVDATA_MNT_POINT "/nvdata"

typedef struct {
	char 	cFileVer[FILEVERLENGTH];
	char 	cFileName[FILENAMELENGTH];
	int 	i4RecSize;
	int 	i4RecNum;
	int 	i4MaxFileLid;
} F_INFO;
typedef struct{
	int iFileDesc;
	int ifile_lid;
	bool bIsRead;
} F_ID;

typedef enum{
	VerUpdate,
	VerDel,
	VerAdd,
}VerInfoUpdateFlag;

#pragma pack(4)
typedef struct nvram_journal_handle {
    unsigned int h_magic;
    unsigned int h_type;
    unsigned int h_sequence;
    time_t h_utc_time;
    char h_func_name[MAX_FUNCTION_NAME_LENGTH];
    unsigned int h_func_line;
    unsigned int h_log_lengeh;
    char h_log[MAX_LOG_LENGTH];
} nvram_journal_handle_t;
#pragma pack()
#define ISREAD      1
#define ISWRITE     0

#ifdef _WIN32
#define LOGD(x)
#else
#include <utils/Log.h>
#undef LOG_TAG
#define LOG_TAG "NVRAM"
#endif


#define NVRAM_LOG(...) \
    do { \
        ALOGD(__VA_ARGS__); \
    } while (0)

bool NVM_GetBackupFileNum(unsigned int * iAPBackupFileNum, unsigned short *iMDBackupFileNum);

bool NVM_AddBackupNum(unsigned int iModifiedFileNum,int iSrc);

bool NVM_AddBackupFileNum(unsigned int iLid);

/********************************************************************************
//FUNCTION:
//		NVM_Init
//DESCRIPTION:
//		this function is called to call nvram pdd and get max lid.
//
//PARAMETERS:
//		None
//
//RETURN VALUE:
//		the max Lid number.
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
int NVM_Init(void);

/********************************************************************************
//FUNCTION:
//		NVM_CheckVerFile
//DESCRIPTION:
//		this function is called to check the exist of versiono file information
//      in FAT 2 partition or default version.
//
//PARAMETERS:
//		Is_FAT2: [IN] TRUE is to check fat2 partition, otherwise check default version
//
//RETURN VALUE:
//		TRUE is exist, otherwise version is not exist.
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_CheckVerFile(bool In_NVM);

/********************************************************************************
//FUNCTION:
//		NVM_GenerateFileVer
//DESCRIPTION:
//		this function is called to generate the version file in fat2 or user partition.
//
//PARAMETERS:
//		CPY_File_To_Fat2: [IN] TRUE is generate version file in FAT2 partition
//
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_GenerateFileVer(bool CPY_File_To_NVM);

/********************************************************************************
//FUNCTION:
//		NVM_CmpFileVerNo
//DESCRIPTION:
//		this function is called to compare file version between FAT2 and default version.
//
//PARAMETERS:
//		file_lid: [IN] the lid of nvram file
//
//RETURN VALUE:
//		TRUE is same, otherwise is not same
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_CmpFileVerNo(int file_lid);

/********************************************************************************
//FUNCTION:
//		NVM_CheckFileID
//DESCRIPTION:
//		this function is called to check the file lid and record id of nvram file.
//
//PARAMETERS:
//		file_lid: 	[IN]		the lid of nvram file
//		rec_id:		[IN]		the record id
//
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_CheckFileID(int file_lid, int rec_id);

/********************************************************************************
//FUNCTION:
//		NVM_CheckFileInfo
//DESCRIPTION:
//		this function is called to check file information, including the file lid
// 		and record id.if the file version is not right, it wll be reset to default
//		value.
//
//PARAMETERS:
//		file_lid: 	[IN]		the lid of nvram file
//		rec_id:	[IN]		the record id
//
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_CheckFileInfo(int file_lid, int rec_id);

/********************************************************************************
//FUNCTION:
//		NVM_ReadFileVerInfo
//DESCRIPTION:
//		this function is called to read the version information of nvram file.
//		if the file version is not right, it wll be reset to default value
//
//PARAMETERS:
//		file_lid:	[IN] the lid of nvram file
//
//RETURN VALUE:
//		refers to the definition of "F_INFO"
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
F_INFO NVM_ReadFileVerInfo(int file_lid);

/********************************************************************************
//FUNCTION:
//		NVM_UpdateFileVerNo
//DESCRIPTION:
//		this function is called to reset a NvRam to default value.
//
//PARAMETERS:
//		file_lid: [IN] the lid of nvram file
//
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_UpdateFileVerNo(int file_lid, VerInfoUpdateFlag UpdateFlag);

/********************************************************************************
//FUNCTION:
//		NVM_ResetFileToDefault
//DESCRIPTION:
//		this function is called to reset a NvRam to default value.
//
//PARAMETERS:
//		file_lid: [IN] the lid of nvram file
//
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_ResetFileToDefault(int file_lid);

/********************************************************************************
//FUNCTION:
//		NVM_GetFileHandle
//DESCRIPTION:
//		this function is called to the handle of nvram file and the information
//      of record size and number.
//
//PARAMETERS:
//		file_lid: 	[IN]	the lid of nvram file
//		pRecSize: 	[OUT] 	the record size
//		pRecNum: 	[OUT] 	the rocord number
//		IsRead: 	[IN]	TRUE is read, otherwise is write
//
//RETURN VALUE:
//		the file handle
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
F_ID NVM_GetFileDesc(int file_lid, int *pRecSize, int *pRecNum, bool IsRead);

/********************************************************************************
//FUNCTION:
//		NVM_CloseFileHandle
//DESCRIPTION:
//		this function is called to close the handle which is open by GetFileHandle.
//
//PARAMETERS:
//		hFile: 	[IN] the file handle
//
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		GetFileHandle must have been called
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
bool NVM_CloseFileDesc(F_ID iFileID);



/********************************************************************************
//FUNCTION:
//		NVM_GetLIDByName
//DESCRIPTION:
//		this function is called to get right lid accoring to file name.
//
//PARAMETERS:
//		filename: [IN] the name of nvram file
//RETURN VALUE:
//		Get lid successfully? -1 mean fail, others mean ok
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
int NVM_GetLIDByName(char* filename);

/********************************************************************************
//FUNCTION:
//		NVM_DataVerConvertAll
//DESCRIPTION:
//		this function is called to convert all data acccording to the version info .
//
//PARAMETERS:
//		file_lid: [IN] the lid of nvram file
//RETURN VALUE:
//		convert successfully?
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/


bool NVM_DataVerConvertAll(unsigned int iOldCommonFileNum,unsigned int iOldCustomFileNum);
int NVM_DataVerConvert(int file_lid);
int NVM_ProtectDataFile(int file_lid,bool Setflag);
bool NVM_CheckBackFlag(int iFileDesc);
bool NVM_CheckMDBackFlag(int iFileDesc);
bool NVM_ComputeBackflagCheckSum(int iFileDesc);
bool NVM_RestoreFromBinRegion_OneFile(int file_lid,const char * filename);
bool NVM_HistoryLog(unsigned int level, const char *func, unsigned int line, const char *log);
unsigned int NVM_GetSequenceNum(void);
bool NVM_IncSequenceNum(unsigned int sequence);
bool NVM_InSpecialLidList(int file_lid, int *index);
bool NVM_CheckFile(const char * filepath);

#define NVRAM_HISTORY_LOG(n, log)   NVM_HistoryLog(n, __func__, __LINE__, log)


bool NVM_MiscIncSeqNum(unsigned int sequence);
unsigned int NVM_MiscGetSeqNum(void);
bool NVM_MiscLog(unsigned int level, const char *func, unsigned int line, const char *log);
#define NVRAM_MISC_LOG(n, log)   NVM_MiscLog(n, __func__, __LINE__, log)

bool NVM_GetDeviceInfo(const char *path, struct mtd_info_user *device_info);
extern int init_nvram_platform_callback();
extern int nvram_platform_callback();

bool Check_FileVerinFirstBoot(void);

bool Check_UpdateStatus(void);
bool Change_DataNvramPermission(const char *cmd);

bool NVM_EraseDeviceBlock(const char *path, struct erase_info_user erase_info);


typedef struct NVRAM_PLATFORM {
	int log_block;
	int resv_block;
	int DM_block;
	int layout_version;
	int header_offset;
} NVRAM_PLATFORM_T;

extern int nvram_multi_storage_support(NVRAM_PLATFORM_T* pPlatform );
extern bool nvram_md5_support();
extern bool nvram_evdo_support();
int NVM_CheckFileSize(int iRealFileLid,const TCFG_FILE *pCfgFielTable);
extern bool nvram_new_partition_support();
extern bool nvram_emmc_support();
extern bool nvram_ecci_c2k_support();

#ifdef __cplusplus
}
#endif


#endif //__NVRAM_LIB_H
