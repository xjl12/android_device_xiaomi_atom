package org.lineageos.xiaomi.imsinit;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;

import com.android.ims.ImsManager;

/*
 * WARNING: DIRTY HACK AHEAD
 * I have no idea why MTK's IMS does not work after a reboot on
 * SELinux enforcing. I have literally tried to put every service
 * with denial messages to permissive and the bug was still present.
 * But a global permissive environment fixes that magically.
 * Re-enabling Enhanced 4G manually also fixes the issue.
 * This is a dirty hack to toggle IMS off and back on again
 * on every boot other than the first boot. I figured this is
 * at least better than having to put the entire system on permissive.
 * But it is very dirty and can't be guaranteed to work.
 * We still need to figure out at some point why this happens.
 */
public class BootCompletedReceiver extends BroadcastReceiver {
    private static final String LOG_TAG = "ImsInit";
    
    @Override
    public void onReceive(final Context context, Intent intent) {
        Log.i(LOG_TAG, "onBoot");
        
        context.startService(new Intent(context, PhoneStateService.class));
    }
}
