/*
 * Copyright (C) 2020 The Android Open Source Project
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

package com.android.server.biometrics.sensors.fingerprint;

import android.annotation.NonNull;
import android.annotation.Nullable;
import android.content.Context;
import android.provider.Settings;
import android.hardware.biometrics.fingerprint.V2_1.IBiometricsFingerprint;
import android.hardware.fingerprint.FingerprintManager;
import android.hardware.fingerprint.IUdfpsOverlayController;
import android.hardware.fingerprint.IUdfpsOverlayControllerCallback;
import android.os.RemoteException;
import android.util.Slog;

import com.android.server.biometrics.sensors.AcquisitionClient;

/**
 * Contains helper methods for under-display fingerprint HIDL.
 */
public class UdfpsHelper {

    private static final String TAG = "UdfpsHelper";
    private static final int defBrightness = 100;
    private static int brightness;

    public static void onFingerDown(IBiometricsFingerprint daemon, int x, int y, float minor,
            float major) {
        android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint extension =
                android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint.castFrom(
                        daemon);
        if (extension == null) {
            Slog.v(TAG, "onFingerDown | failed to cast the HIDL to V2_3");
            return;
        }

        try {
            extension.onFingerDown(x, y, minor, major);
        } catch (RemoteException e) {
            Slog.e(TAG, "onFingerDown | RemoteException: ", e);
        }
    }

    public static void onFingerUp(IBiometricsFingerprint daemon) {
        android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint extension =
                android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint.castFrom(
                        daemon);
        if (extension == null) {
            Slog.v(TAG, "onFingerUp | failed to cast the HIDL to V2_3");
            return;
        }

        try {
            extension.onFingerUp();
        } catch (RemoteException e) {
            Slog.e(TAG, "onFingerUp | RemoteException: ", e);
        }
    }

    public static int getReasonFromEnrollReason(@FingerprintManager.EnrollReason int reason) {
        switch (reason) {
            case FingerprintManager.ENROLL_FIND_SENSOR:
                return IUdfpsOverlayController.REASON_ENROLL_FIND_SENSOR;
            case FingerprintManager.ENROLL_ENROLL:
                return IUdfpsOverlayController.REASON_ENROLL_ENROLLING;
            default:
                return IUdfpsOverlayController.REASON_UNKNOWN;
        }
    }

    public static void showUdfpsOverlay(int sensorId, int reason,
            @Nullable IUdfpsOverlayController udfpsOverlayController,
            @NonNull AcquisitionClient<?> client) {
        showUdfpsOverlay(null, sensorId, reason, udfpsOverlayController, client);
    }

    public static void showUdfpsOverlay(IBiometricsFingerprint daemon,
            int sensorId, int reason,
            @Nullable IUdfpsOverlayController udfpsOverlayController,
            @NonNull AcquisitionClient<?> client) {
        if (udfpsOverlayController == null) {
            return;
        }
        brightness = Settings.System.getInt(client.getContext().getContentResolver(), Settings.System.SCREEN_BRIGHTNESS,defBrightness);
        if (Settings.System.getInt(client.getContext().getContentResolver(), Settings.System.SCREEN_BRIGHTNESS_MODE,1) == 0) 
            brightness += 100000;
        final IUdfpsOverlayControllerCallback callback =
                new IUdfpsOverlayControllerCallback.Stub() {
                    @Override
                    public void onUserCanceled() {
                        client.onUserCanceled();
                    }
                };

        if (daemon != null) {
            android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint extension =
                android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint.castFrom(
                daemon);
            if (extension != null) {
                try {
                    extension.onShowUdfpsOverlay(brightness);
                } catch (RemoteException e) {
                    Slog.v(TAG, "showUdfpsOverlay | RemoteException: ", e);
                }
            } else {
                Slog.v(TAG, "onShowUdfpsOverlay | failed to cast the HIDL to V2_3");
            }
        } else {
             Slog.v(TAG, "onShowUdfpsOverlay | daemon null");
        }

        try {
            udfpsOverlayController.showUdfpsOverlay(sensorId, reason, callback);
        } catch (RemoteException e) {
            Slog.e(TAG, "Remote exception when showing the UDFPS overlay", e);
        }
    }

    public static void hideUdfpsOverlay(int sensorId,
            @Nullable IUdfpsOverlayController udfpsOverlayController) {
        hideUdfpsOverlay(null, sensorId, udfpsOverlayController);
    }

    public static void hideUdfpsOverlay(IBiometricsFingerprint daemon, int sensorId,
            @Nullable IUdfpsOverlayController udfpsOverlayController) {
        if (udfpsOverlayController == null) {
            return;
        }
        if (daemon != null) {
            android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint extension =
                android.hardware.biometrics.fingerprint.V2_3.IBiometricsFingerprint.castFrom(
                daemon);
            if (extension != null) {
                try {
                    extension.onHideUdfpsOverlay();
                } catch (RemoteException e) {
                    Slog.v(TAG, "hideUdfpsOverlay | RemoteException: ", e);
                }
            } else {
                Slog.v(TAG, "onHideUdfpsOverlay | failed to cast the HIDL to V2_3");
            }
        } else {
            Slog.v(TAG, "onHideUdfpsOverlay | daemon null");
        }

        try {
            udfpsOverlayController.hideUdfpsOverlay(sensorId);
        } catch (RemoteException e) {
            Slog.e(TAG, "Remote exception when hiding the UDFPS overlay", e);
        }
    }

    public static void onAcquiredGood(int sensorId,
            @Nullable IUdfpsOverlayController udfpsOverlayController) {
        if (udfpsOverlayController == null) {
            return;
        }

        try {
            udfpsOverlayController.onAcquiredGood(sensorId);
        } catch (RemoteException e) {
            Slog.e(TAG, "Remote exception when sending onAcquiredGood", e);
        }
    }

    public static void onEnrollmentProgress(int sensorId, int remaining,
            @Nullable IUdfpsOverlayController udfpsOverlayController) {
        if (udfpsOverlayController == null) {
            return;
        }
        try {
            udfpsOverlayController.onEnrollmentProgress(sensorId, remaining);
        } catch (RemoteException e) {
            Slog.e(TAG, "Remote exception when sending onEnrollmentProgress", e);
        }
    }

    public static void onEnrollmentHelp(int sensorId,
            @Nullable IUdfpsOverlayController udfpsOverlayController) {
        if (udfpsOverlayController == null) {
            return;
        }
        try {
            udfpsOverlayController.onEnrollmentHelp(sensorId);
        } catch (RemoteException e) {
            Slog.e(TAG, "Remote exception when sending onEnrollmentHelp", e);
        }
    }

    public static boolean isValidAcquisitionMessage(@NonNull Context context,
            int acquireInfo, int vendorCode) {
        return FingerprintManager.getAcquiredString(context, acquireInfo, vendorCode) != null;
    }
}
