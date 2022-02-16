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

#define LOG_TAG "android.hardware.biometrics.fingerprint@2.3-service"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include "BiometricsFingerprint.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using android::hardware::biometrics::fingerprint::V2_3::IBiometricsFingerprint;
using android::hardware::biometrics::fingerprint::V2_3::implementation::BiometricsFingerprint;

using android::OK;
using android::status_t;

int main() {
    android::sp<IBiometricsFingerprint> service = new BiometricsFingerprint();

    configureRpcThreadpool(1, true);

    status_t status = service->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Cannot register Biometrics 2.3 HAL service.";
        return 1;
    }

    LOG(INFO) << "Biometrics 2.3 HAL service ready.";

    joinRpcThreadpool();

    LOG(ERROR) << "Biometrics 2.3 HAL service failed to join thread pool.";
    return 1;
}