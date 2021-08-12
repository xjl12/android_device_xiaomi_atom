package org.lineageos.dt2w.merlin;

import android.app.Service;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.provider.Settings.Secure;

public class DT2WServiceMerlin extends Service {
    private static final String TAG = "DT2WServiceMerlin";
    private Context mContext;
    private Handler mHandler;
    private CustomSettingsObserver mCustomSettingsObserver;

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
        mHandler = new Handler(Looper.getMainLooper());
        mCustomSettingsObserver = new CustomSettingsObserver(mHandler);
        mCustomSettingsObserver.observe();
        mCustomSettingsObserver.update();
        return START_STICKY;
    }

    private class CustomSettingsObserver extends ContentObserver {
        CustomSettingsObserver(Handler handler) {
            super(handler);
        }

        void observe() {
            ContentResolver resolver = mContext.getContentResolver();
            resolver.registerContentObserver(Secure.getUriFor(Secure.DOUBLE_TAP_TO_WAKE),
                    false, this, UserHandle.USER_CURRENT);
        }

        void update() {
            int dt2wValue = Secure.getInt(mContext.getContentResolver(), Secure.DOUBLE_TAP_TO_WAKE, 0);
            boolean dt2wEnabled = dt2wValue == 1;
            SystemProperties.set("persist.sys.merlin.dt2w", dt2wEnabled ? "1" : "0");
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            if (uri.equals(Secure.getUriFor(Secure.DOUBLE_TAP_TO_WAKE))) {
                update();
            }
        }
    }
}
