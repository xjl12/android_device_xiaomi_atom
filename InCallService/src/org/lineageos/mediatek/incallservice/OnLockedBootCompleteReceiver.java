package org.lineageos.mediatek.incallservice;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.Context;

import android.util.Log;

public class OnLockedBootCompleteReceiver extends BroadcastReceiver {
    private static final String LOG_TAG = "MtkInCallService";

    @Override
    public void onReceive(final Context context, Intent intent) {
        Log.i(LOG_TAG, "onBoot");

        Intent sIntent = new Intent(context, VolumeChangeService.class);
        context.startService(sIntent);
    }
}
