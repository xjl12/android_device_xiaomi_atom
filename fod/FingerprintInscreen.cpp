/*
 * Copyright (C) 2019-2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"
#include <unistd.h>

#include <android-base/logging.h>
#include <hardware_legacy/power.h>
#include <cmath>
#include <fstream>
#include <thread>


#define FINGERPRINT_ACQUIRED_VENDOR 6

#define COMMAND_NIT 10
#define PARAM_NIT_FOD 1
#define PARAM_NIT_NONE 0

#define DISPPARAM_PATH "/sys/class/drm/card0-DSI-1/disp_param"
#define DISPPARAM_HBM_FOD_ON "0x20000"
#define DISPPARAM_HBM_FOD_OFF "0xE0000"

#define TOUCH_FOD_ENABLE 10

#define FOD_SENSOR_X 445
#define FOD_SENSOR_Y 2025
#define FOD_SENSOR_SIZE 189

#define BRIGHTNESS_PATH "/sys/class/leds/lcd-backlight/brightness"

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_0 {
namespace implementation {
    
template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;

    file >> result;
    return file.fail() ? def : result;
}

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

static void threadboost(sp<IXiaomiFingerprint> txiaomiFingerprintService,char *pflag){
    LOG(ERROR) << "Thread start";
    usleep(250000);
    set(DISPPARAM_PATH, *pflag ? DISPPARAM_HBM_FOD_ON:DISPPARAM_HBM_FOD_OFF);
    txiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_FOD);
}

FingerprintInscreen::FingerprintInscreen() {
    xiaomiDisplayFeatureService = IDisplayFeature::getService();
    touchFeatureService = ITouchFeature::getService();
    xiaomiFingerprintService = IXiaomiFingerprint::getService();
    //sd = 400000;
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return FOD_SENSOR_X;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return FOD_SENSOR_Y;
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t brightness ) {
    float alpha;
//    LOG(ERROR) << "getDimAmount()";
    if (brightness >= 62) {
        alpha = 1.0 - pow(brightness / 255.0 * 430.0 / 600.0, 0.485);
    } else if (brightness < 62 && brightness >=31) {
        alpha = 1.0 - pow(brightness / 255.0 * 430.0 / 600.0, 0.530);
    } else if (brightness > 7) {
         alpha = 1.0 - pow(brightness / 209.0, 0.525);
    }else{
        alpha = 1.0 - pow(brightness / 209.0, 0.475);
    }
    return 255 * alpha;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return FOD_SENSOR_SIZE;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
//    LOG(ERROR) << "OnPress()";
    acquire_wake_lock(PARTIAL_WAKE_LOCK, LOG_TAG);
    std::thread(threadboost,xiaomiFingerprintService,&flag).detach();
    flag=1;
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_NONE);
    set(DISPPARAM_PATH, DISPPARAM_HBM_FOD_OFF);
    flag=0;
    release_wake_lock(LOG_TAG);
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    xiaomiDisplayFeatureService->setFeature(0, 17, 1, 1);
    //xiaomiDisplayFeatureService->setFeature(0, 22, 1, 10);
    touchFeatureService->setTouchMode(TOUCH_FOD_ENABLE, 2);
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    set(DISPPARAM_PATH, DISPPARAM_HBM_FOD_OFF);
    touchFeatureService->resetTouchMode(TOUCH_FOD_ENABLE);
    //xiaomiDisplayFeatureService->setFeature(0, 22, 0, 10);
    xiaomiDisplayFeatureService->setFeature(0, 17, 0, 1);
    flag=0;
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    LOG(ERROR) << "acquiredInfo: " << acquiredInfo << ", vendorCode: " << vendorCode;
    std::lock_guard<std::mutex> _lock(mCallbackLock);
    if (mCallback == nullptr) {
        return false;
    }

    if (acquiredInfo == FINGERPRINT_ACQUIRED_VENDOR) {
        if (vendorCode == 22) {
            Return<void> ret = mCallback->onFingerDown();
            if (!ret.isOk()) {
                LOG(ERROR) << "FingerDown() error: " << ret.description();
            }
            return true;
        }

        if (vendorCode == 23) {
            Return<void> ret = mCallback->onFingerUp();
            if (!ret.isOk()) {
                LOG(ERROR) << "FingerUp() error: " << ret.description();
            }
            return true;
        }
    }
    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode) {
    LOG(ERROR) << "error: " << error << ", vendorCode: " << vendorCode;
    return false;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}


Return<void> FingerprintInscreen::setCallback(const sp<IFingerprintInscreenCallback>& callback) {
    {
        std::lock_guard<std::mutex> _lock(mCallbackLock);
        mCallback = callback;
    }
    return Void();
}


}  // namespace implementation
}  // namespace V1_0
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor
