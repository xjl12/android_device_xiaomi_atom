# setup main mic to headhone
AudioSetParam  SET_LOOPBACK_TYPE=2,2
#tinymix Headset_PGAL_GAIN 2
#tinymix Headset_PGAR_GAIN 2

#INPUT DEVICE:
#enum loopback_t {
#
#    NO_LOOPBACK                                 = 0,
#
#    // AFE Loopback
#    AP_MAIN_MIC_AFE_LOOPBACK                    = 1,
#    AP_HEADSET_MIC_AFE_LOOPBACK                 = 2,
#    AP_REF_MIC_AFE_LOOPBACK                     = 3,
#    AP_3RD_MIC_AFE_LOOPBACK                     = 4,
#
#    // Acoustic Loopback
#    MD_MAIN_MIC_ACOUSTIC_LOOPBACK               = 21,
#    MD_HEADSET_MIC_ACOUSTIC_LOOPBACK            = 22,
#    MD_DUAL_MIC_ACOUSTIC_LOOPBACK_WITHOUT_DMNR  = 23,
#    MD_DUAL_MIC_ACOUSTIC_LOOPBACK_WITH_DMNR     = 24,
#    MD_REF_MIC_ACOUSTIC_LOOPBACK                = 25,
#    MD_3RD_MIC_ACOUSTIC_LOOPBACK                = 26,
#};
#
#
#OUTPUT DEVICE:
#enum loopback_output_device_t {
#    LOOPBACK_OUTPUT_RECEIVER = 1,
#    LOOPBACK_OUTPUT_EARPHONE = 2,
#    LOOPBACK_OUTPUT_SPEAKER  = 3,
#};
