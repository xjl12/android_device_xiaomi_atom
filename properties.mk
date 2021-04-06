ifeq ($(TARGET_BUILD_VARIANT),eng)
# ADB
PRODUCT_PRODUCT_PROPERTIES += \
    ro.adb.secure=0
endif

# Priv-app permission
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.control_privapp_permissions=log

# Audio
PRODUCT_PRODUCT_PROPERTIES += \
    audio.offload.min.duration.secs=30 \
    ro.bluetooth.a2dp_offload.supported=false \
    persist.bluetooth.a2dp_offload.disabled=true \
    vendor.audio.feature.a2dp_offload.enable=false

# Camera
PRODUCT_PRODUCT_PROPERTIES += \
    vendor.camera.aux.packagelist=org.lineageos.snap,org.codeaurora.snapcam

# Display
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    debug.sf.enable_hwc_vds=1 \
    ro.sf.lcd_density=440

# IMS
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    persist.vendor.volte_support=1 \
    persist.vendor.mtk.volte.enable=1 \
    persist.vendor.mtk_ct_volte_support=3 \
    persist.vendor.mtk_hvolte_volte_indicator=0 \
    persist.vendor.volte_enable=1 \
    persist.vendor.mtk_wfc_support=1


# Telephony
PRODUCT_PRODUCT_PROPERTIES += \
    ro.telephony.default_network=33,22 \
    persist.vendor.data.iwlan.enable=true

# Vendor
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.vendor.qti.va_aosp.support=1

# Xiaomi
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    sys.displayfeature_hidl=true \
    vendor.displayfeature.entry.enable=true \
    vendor.hbm.enable=true \
    persist.vendor.power.dfps.level=0 \
    persist.vendor.dc_backlight.enable=false \
    ro.vendor.radio.features_common=2 \
    ro.vendor.net.enable_sla=1 \
    ro.vendor.touchfeature.type=11 \
    ro.vendor.radio.5g=3

# Sound Step
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.config.vc_call_vol_steps=14 \
    ro.config.media_vol_steps=30
