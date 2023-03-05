package org.lineageos.mediatek.incallservice;

import android.content.Intent;
import android.content.Context;
import android.content.BroadcastReceiver;

import android.media.AudioManager;
import android.media.AudioSystem;
import android.media.AudioDeviceInfo;

import android.util.Log;

public class VolumeChangeReceiver extends BroadcastReceiver {
    public static final String LOG_TAG = "MtkInCallService";

    private AudioManager mAudioManager;

    public VolumeChangeReceiver(Context context) {
        mAudioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        int streamType = intent.getIntExtra(AudioManager.EXTRA_VOLUME_STREAM_TYPE, -1);
        if (streamType == AudioSystem.STREAM_VOICE_CALL) {
            AudioDeviceInfo callDevice = mAudioManager.getCommunicationDevice();
            if (callDevice.getInternalType() != AudioDeviceInfo.TYPE_BUILTIN_EARPIECE) {
                // Device is not the built in earpiece, we don't need to do anything.
                return;
            }

            // Start building parameters
            String parameters = "volumeDevice=" + (callDevice.getId() - 1) + ";";
            int volumeIndex = intent.getIntExtra(AudioManager.EXTRA_VOLUME_STREAM_VALUE, -1);
            if (volumeIndex < 0) {
                Log.w(LOG_TAG, "Could not get volumeIndex!");
                return;
            }

            // Limit volumeIndex to a max of 7 since that's the size of
            // MediaTek's gain table.
            parameters += "volumeIndex=" + Math.min(7, volumeIndex) + ";";
            parameters += "volumeStreamType=" + streamType;

            // Set gain parameters
            Log.d(LOG_TAG, "Setting audio parameters: " + parameters);
            AudioSystem.setParameters(parameters);
        }
    }
}
