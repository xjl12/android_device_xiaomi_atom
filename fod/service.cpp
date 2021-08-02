/*
 * Copyright (C) 2019 The LineageOS Project
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

#define LOG_TAG "lineage.biometrics.fingerprint.inscreen@1.0-service.kona"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include "FingerprintInscreen.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using vendor::lineage::biometrics::fingerprint::inscreen::V1_0::IFingerprintInscreen;
using vendor::lineage::biometrics::fingerprint::inscreen::V1_0::implementation::FingerprintInscreen;

using android::OK;
using android::status_t;

int main() {
    android::sp<IFingerprintInscreen> service = new FingerprintInscreen();

    configureRpcThreadpool(1, true);

    status_t status = service->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Cannot register FOD HAL service.";
        return 1;
    }

    LOG(INFO) << "FOD HAL service ready.";

    joinRpcThreadpool();

    LOG(ERROR) << "FOD HAL service failed to join thread pool.";
    return 1;
}
