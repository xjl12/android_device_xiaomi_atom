package org.lineageos.mediatek.incallservice;

import android.media.AudioManager;

import android.content.Intent;
import android.content.IntentFilter;
import android.content.Context;
import android.app.Service;
import android.os.IBinder;

import android.util.Log;

public class VolumeChangeService extends Service {
    public static final String LOG_TAG = "MtkInCallService";

    private Context mContext;
    private VolumeChangeReceiver mVolumeChangeReceiver;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startid) {
        mContext = this;
        mVolumeChangeReceiver = new VolumeChangeReceiver(mContext);

        Log.i(LOG_TAG, "Service is starting...");
        this.registerReceiver(mVolumeChangeReceiver,
                               new IntentFilter(AudioManager.VOLUME_CHANGED_ACTION));
        return START_STICKY;
    }
}
