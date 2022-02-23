/*
 * Copyright (C) 2021 The Android Open Source Project
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

#define LOG_TAG "android.hardware.biometrics.fingerprint@2.3-service.xiaomi_atom"

#include "BiometricsFingerprint.h"

#include <android-base/logging.h>
#include <hardware_legacy/power.h>
#include <fstream>
#include <cmath>
#include <thread>

#define FINGERPRINT_ERROR_VENDOR 8

#define COMMAND_NIT 10
#define PARAM_NIT_UDFPS 1
#define PARAM_NIT_NONE 0

#define TOUCH_FOD_ENABLE 10

#define DISPPARAM_PATH "/sys/class/drm/card0-DSI-1/disp_param"
#define DISPPARAM_HBM_UDFPS_ON  "0x20000" 
#define DISPPARAM_HBM_UDFPS_OFF "0xE0000"

namespace android {
namespace hardware {
namespace biometrics {
namespace fingerprint {
namespace V2_3 {
namespace implementation {

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
    LOG(ERROR) << "Set " << path << value;
}

static void threadboost(sp<IXiaomiFingerprint> txiaomiFingerprintService){
    LOG(ERROR) << "Thread start";
    usleep(200000);
    set(DISPPARAM_PATH, DISPPARAM_HBM_UDFPS_ON);
    txiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_UDFPS);
}

BiometricsFingerprint::BiometricsFingerprint() {
    LOG(ERROR) << "Start!";
    biometrics_2_1_service = IBiometricsFingerprint_2_1::getService();
    touchFeatureService = ITouchFeature::getService();
    xiaomiFingerprintService = IXiaomiFingerprint::getService();
    xiaomiDisplayFeatureService = IDisplayFeature::getService();
    xiaomiDisplayFeatureService->setFeature(0, 20, 1, 255);
}

Return<uint64_t> BiometricsFingerprint::setNotify(const sp<IBiometricsFingerprintClientCallback>& clientCallback) {
    LOG(ERROR) << "setNotify()";
    return biometrics_2_1_service->setNotify(clientCallback);
}

Return<uint64_t> BiometricsFingerprint::preEnroll() {
    return biometrics_2_1_service->preEnroll();
}

Return<RequestStatus> BiometricsFingerprint::enroll(const hidl_array<uint8_t, 69>& hat, uint32_t gid, uint32_t timeoutSec) {
    LOG(ERROR) << "enroll()";
    return biometrics_2_1_service->enroll(hat, gid, timeoutSec);
}

Return<RequestStatus> BiometricsFingerprint::postEnroll() {
    return biometrics_2_1_service->postEnroll();
}

Return<uint64_t> BiometricsFingerprint::getAuthenticatorId() {
    LOG(ERROR) << "getAuthenticatorId()";
    return biometrics_2_1_service->getAuthenticatorId();
}

Return<RequestStatus> BiometricsFingerprint::cancel() {
    return biometrics_2_1_service->cancel();
}

Return<RequestStatus> BiometricsFingerprint::enumerate() {
    LOG(ERROR) << "enumerate()";
    return biometrics_2_1_service->enumerate();
}

Return<RequestStatus> BiometricsFingerprint::remove(uint32_t gid, uint32_t fid) {
    LOG(ERROR) << "remove(" << gid << ", " << fid << ")" ;
    return biometrics_2_1_service->remove(gid, fid);
}

Return<RequestStatus> BiometricsFingerprint::setActiveGroup(uint32_t gid, const hidl_string& storePath) {
    LOG(ERROR) << "setActiveGroup()";
    return biometrics_2_1_service->setActiveGroup(gid, storePath);
}

Return<RequestStatus> BiometricsFingerprint::authenticate(uint64_t operationId, uint32_t gid) {
    LOG(ERROR) << "authenticate()";
    return biometrics_2_1_service->authenticate(operationId, gid);
}

Return<bool> BiometricsFingerprint::isUdfps(uint32_t) {
    LOG(ERROR) << "isUdfps()";
    return true;
}

Return<void> BiometricsFingerprint::onFingerDown(uint32_t a, uint32_t b, float c, float d) {
    LOG(ERROR) << "onFingerDown " << a << ", "  << b << ", " << c << ", " << d;
    acquire_wake_lock(PARTIAL_WAKE_LOCK, LOG_TAG);
    std::thread(threadboost,xiaomiFingerprintService).detach();
    return Void();
}

Return<void> BiometricsFingerprint::onFingerUp() {
    LOG(ERROR) << "onFingerUp()";
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_NONE);
    set(DISPPARAM_PATH, DISPPARAM_HBM_UDFPS_OFF);
    release_wake_lock(LOG_TAG);
    return Void();
}

/* 
 * For Xiaomi MTK Phone, we must call xiaomiDisplayFeatureService and
 * touchFeatureService before fingerprint authorization start. So we 
 * must merge the follow commit into your Android Source before use the
 * following two methods.
 * Commit [1]: https://github.com/Octavi-OS/platform_frameworks_base/commit/4d0f6c6e07c98d2ba22f9271c3798f0d545620ef 
 * Commit [2]: https://github.com/Octavi-OS/platform_hardware_interfaces/commit/7d663dba1d216e19c57f64f5567078f56b8fe0bb
 * 
 * @author xjl12
*/
Return<void> BiometricsFingerprint::onShowUdfpsOverlay() {
    LOG(ERROR) << "onShowUdfpsOverlay()";
    xiaomiDisplayFeatureService->setFeature(0, 20, 0, 255);
    xiaomiDisplayFeatureService->setFeature(0, 17, 1, 1);
    touchFeatureService->setTouchMode(TOUCH_FOD_ENABLE, 2);
    return Void();
}

Return<void> BiometricsFingerprint::onHideUdfpsOverlay() {
    LOG(ERROR) << "onHideUdfpsOverlay()";
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_NONE);
    set(DISPPARAM_PATH, DISPPARAM_HBM_UDFPS_OFF);
    xiaomiDisplayFeatureService->setFeature(0, 20, 1, 255);
    touchFeatureService->resetTouchMode(TOUCH_FOD_ENABLE);
    xiaomiDisplayFeatureService->setFeature(0, 17, 0, 1);
    return Void();
}

}  // namespace implementation
}  // namespace V2_3
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace hardware
}  // namespace android
