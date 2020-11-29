# Audio
PRODUCT_PRODUCT_PROPERTIES += \
    audio.offload.min.duration.secs=30

# Camera
PRODUCT_PRODUCT_PROPERTIES += \
    vendor.camera.aux.packagelist=org.mokee.snap,org.codeaurora.snapcam

# Display
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    debug.sf.enable_hwc_vds=1 \
    ro.sf.lcd_density=440

# IMS
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    persist.dbg.volte_avail_ovr=1 \
    persist.dbg.vt_avail_ovr=1 \
    persist.dbg.wfc_avail_ovr=1

# Telephony
PRODUCT_PRODUCT_PROPERTIES += \
    ro.telephony.default_network=33,22

# Vendor
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.vendor.qti.va_aosp.support=1
