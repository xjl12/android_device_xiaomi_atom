# UDFPS Supported in Xiaomi Mediatek Device

## This may be the best solution for Xiaomi Mediatek FOD devices.

#### There are several points in the UDFPS function:

1. Modify system HIDL interface to support onShowUdfpsOverlay() and onHideUdfpsOverlay() binder call.
2. Let system fingerprint overlay service(SensorOverlays) call HIDL interface when UDFPS overlay begin to show or disappear.
3. HAL daemon implements onShowUdfpsOverlay() to notify Xiaomi TouchFeatureService be ready for fingerprint authentication.
4. Modify SystemUI UdfpsController to darken other areas of the screen when UDFPS overlay is showing.
5. Implements daemon onFingerDown() and onFingerUp() to call Xiaomi Fingerprint Service.

#### The above five points are the result of previous work, and the following is my refinement of this issue:

1. Modify SystemUI UdfpsView onAttachedToWindow() and onDetachedFromWindow() callback to put authentication begin or end infomation into Settings.System.
2. On XiaomiParts Application, we register SCREEN_ON and SCREEN_OFF broadcast. Then, we could check Settings.System UDFPS flag set by UdfpsView when received SCREEN_ON event. If UDFPS authenticationis in progress, we must turn HBM on immediately.
3. In SystemUI UdfpsView, we add onWindowVisibilityChanged() callback to put VISIBILITY info into Settings.System use key "udfps_need_hbm".
4. In XiaomiParts DozeService, we create a ContentObserver to monitor Settings.System key "udfps_need_hbm" changes. When it from ON turn into OFF, we should disable HBM immediately. owever, when it from OFF to ON, we start a thread to enable HBM later. What's more, we must ensure only ONE Thread running in the background.
5. Change SystemUI UdfpsControllerOverlay to rise UdfpsOverlay window layer above the navigation bar.
6. Modify SystemUI resources to hide Fingerprint background and make Fingerprint icon in gray color. This measure is to prevent the fingerprint unlock icon from being too bright on the lock screen page because we turn HBM on.

#### All in all, the mainline of my UDFPS solution is turn on HBM when Udfps overlay begin to show, not onFingerDown(). This is the point to improve the speed of fingerprint unlocking and bring better experience to users.