package org.ifaa.android.manager;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Build;
import android.os.Build.VERSION;
import android.os.HwBinder;
import android.os.HwBlob;
import android.os.HwParcel;
import android.os.IBinder;
import android.os.IBinder.DeathRecipient;
import android.os.IHwBinder;
import android.os.Parcel;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Slog;

import java.util.ArrayList;
import java.util.Arrays;

import org.json.JSONObject;

public class IFAAManagerImpl extends IFAAManagerV4 {
    private static final int CODE_PROCESS_CMD = 1;
    private static final int CODE_GETIDLIST_CMD = 2;

    private static final int IFAA_TYPE_FINGER = 1;
    private static final int IFAA_TYPE_IRIS = 2;
    private static final int IFAA_TYPE_SENSOR_FOD = 16;

    private static final int ACTIVITY_START_SUCCESS = 0;
    private static final int ACTIVITY_START_FAILED = -1;

    private static volatile IFAAManagerImpl INSTANCE = null;

    private static final String CA_CERT_ALIAS_DELIMITER = " ";

    private static final String INTERFACE_DESCRIPTOR =
            "vendor.xiaomi.hardware.mlipay@1.0::IMlipayService";
    private static final String SERVICE_NAME =
            "vendor.xiaomi.hardware.mlipay@1.0::IMlipayService";
    private static final String TAG = "IfaaManagerImpl";

    private static final String mIfaaActName = "org.ifaa.android.manager.IFAAService";
    private static final String mIfaaInterfaceDesc = "org.ifaa.android.manager.IIFAAService";
    private static final String mIfaaPackName = "com.tencent.soter.soterserver";

    private static IBinder mService = null;
    private String mDevModel = null;
    private static Context mContext = null;

    private static ServiceConnection ifaaconn = new ServiceConnection() {
        public void onServiceConnected(ComponentName name, IBinder service) {
            mService = service;
            try {
                mService.linkToDeath(mDeathRecipient, 0);
            } catch (RemoteException e) {
                Slog.e(TAG, "linkToDeath fail.", e);
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            if (mContext != null) {
                Slog.i(TAG, "re-bind the service.");
                initService();
            }
        }
    };

    private static DeathRecipient mDeathRecipient = new DeathRecipient() {
        public void binderDied() {
            if (mService != null) {
                Slog.d(TAG, "binderDied, unlink the service.");
                mService.unlinkToDeath(mDeathRecipient, 0);
            }
        }
    };

    public static IFAAManagerV4 getInstance(Context context) {
        if (INSTANCE == null) {
            synchronized (IFAAManagerImpl.class) {
                if (INSTANCE == null) {
                    INSTANCE = new IFAAManagerImpl();
                    if (VERSION.SDK_INT >= 28) {
                        mContext = context;
                        initService();
                    }
                }
            }
        }

        return INSTANCE;
    }

    private String initExtString() {
        String extStr = "";
        JSONObject obj = new JSONObject();
        JSONObject keyInfo = new JSONObject();
        String xy = "";
        String wh = "";

        if (VERSION.SDK_INT >= 28) {
            xy = SystemProperties.get("persist.vendor.sys.fp.fod.location.X_Y", "");
            wh = SystemProperties.get("persist.vendor.sys.fp.fod.size.width_height", "");
        } else {
            xy = SystemProperties.get("persist.sys.fp.fod.location.X_Y", "");
            wh = SystemProperties.get("persist.sys.fp.fod.size.width_height", "");
        }

        try {
            if (validateVal(xy) && validateVal(wh)) {
                String[] splitXy = xy.split(",");
                String[] splitWh = wh.split(",");
                keyInfo.put("startX", Integer.parseInt(splitXy[0]));
                keyInfo.put("startY", Integer.parseInt(splitXy[1]));
                keyInfo.put("width", Integer.parseInt(splitWh[0]));
                keyInfo.put("height", Integer.parseInt(splitWh[1]));
                keyInfo.put("navConflict", true);
                obj.put("type", 0);
                obj.put("fullView", keyInfo);
                extStr = obj.toString();
            } else {
                Slog.e(TAG, "initExtString invalidate, xy:" + xy + " wh:" + wh);
            }
        } catch (Exception e) {
            Slog.e(TAG, "Exception , xy:" + xy + " wh:" + wh, e);
        }

        return extStr;
    }

    private static void initService() {
        Intent ifaaIntent = new Intent();
        ifaaIntent.setClassName(mIfaaPackName, mIfaaActName);
        if (!mContext.bindService(ifaaIntent, ifaaconn, 1)) {
            Slog.e(TAG, "cannot bind service org.ifaa.android.manager.IFAAService");
        }
    }

    private boolean validateVal(String value) {
        return !"".equalsIgnoreCase(value) && value.contains(",");
    }

    public String getDeviceModel() {
        if (mDevModel == null) {
            mDevModel = Build.MANUFACTURER + "-" + Build.DEVICE;
        }

        Slog.i(TAG, "getDeviceModel deviceModel:" + mDevModel);
        return mDevModel;
    }

    public int getEnabled(int bioType) {
        return 1 == bioType ? 1000 : 1003;
    }

    public String getExtInfo(int authType, String keyExtInfo) {
        Slog.i(TAG, "getExtInfo:" + authType + CA_CERT_ALIAS_DELIMITER + keyExtInfo);
        return initExtString();
    }

    public int[] getIDList(int bioType) {
        int[] idList = new int[]{0};
        if (1 == bioType) {
            int retry_count = 10;
            while (true) {
                int retry_count2 = retry_count - 1;
                if (retry_count <= 0) {
                    break;
                }
                if (mService == null || !mService.pingBinder()) {
                    try {
                        Thread.sleep(30);
                    } catch (InterruptedException e) {
                        Slog.e(TAG, "getIDList InterruptedException while waiting: " + e, e);
                    }
                } else {
                    Parcel data = Parcel.obtain();
                    Parcel reply = Parcel.obtain();
                    try {
                        data.writeInterfaceToken(mIfaaInterfaceDesc);
                        data.writeInt(bioType);
                        mService.transact(CODE_GETIDLIST_CMD, data, reply, 0);
                        reply.readException();
                        idList = reply.createIntArray();
                    } catch (RemoteException e) {
                        Slog.e(TAG, "getIDList transact failed. ", e);
                    } catch (Throwable th) {
                        data.recycle();
                        reply.recycle();
                    }
                    data.recycle();
                    reply.recycle();
                }
                retry_count = retry_count2;
            }
        }
        return idList;
    }

    public int getSupportBIOTypes(Context context) {
        int ifaaProp;
        String fpVendor = "";

        if (VERSION.SDK_INT >= 28) {
            ifaaProp = SystemProperties.getInt("persist.vendor.sys.pay.ifaa", 0);
            fpVendor = SystemProperties.get("persist.vendor.sys.fp.vendor", "");
        } else {
            ifaaProp = SystemProperties.getInt("persist.sys.ifaa", 0);
            fpVendor = SystemProperties.get("persist.sys.fp.vendor", "");
        }

        int res = "none".equalsIgnoreCase(fpVendor) ?
                ifaaProp & IFAA_TYPE_IRIS : ifaaProp & (IFAA_TYPE_FINGER | IFAA_TYPE_IRIS);

        if ((res & IFAA_TYPE_FINGER) == IFAA_TYPE_FINGER && sIsFod) {
            res |= IFAA_TYPE_SENSOR_FOD;
        }

        Slog.i(TAG, "getSupportBIOTypes:" + ifaaProp + ", " + sIsFod + ", " + fpVendor +
                ", res:" + res);
        return res;
    }

    public int getVersion() {
        Slog.i(TAG, "getVersion sdk:" + VERSION.SDK_INT + " ifaaVer:" + sIfaaVer);
        return sIfaaVer;
    }

    public byte[] processCmdV2(Context context, byte[] param) {
        Slog.i(TAG, "processCmdV2 sdk:" + VERSION.SDK_INT);

        if (VERSION.SDK_INT >= 28) {
            int retry_count = 10;

            while (true) {
                int retry_count2 = retry_count - 1;
                if (retry_count <= 0) {
                    break;
                }
                if (mService == null || !mService.pingBinder()) {
                    Slog.i(TAG, "processCmdV2 waiting ifaaService, remain: " + retry_count2 +
                            " time(s)");
                    try {
                        Thread.sleep(30);
                    } catch (InterruptedException e) {
                        Slog.e(TAG, "processCmdV2 InterruptedException while waiting: " + e, e);
                    }
                } else {
                    Parcel data = Parcel.obtain();
                    Parcel reply = Parcel.obtain();
                    try {
                        data.writeInterfaceToken(mIfaaInterfaceDesc);
                        data.writeByteArray(param);
                        mService.transact(CODE_PROCESS_CMD, data, reply, 0);
                        reply.readException();
                        return reply.createByteArray();
                    } catch (RemoteException e) {
                        Slog.e(TAG, "processCmdV2 transact failed. ", e);
                        retry_count = retry_count2;
                    } finally {
                        data.recycle();
                        reply.recycle();
                    }
                }
                retry_count = retry_count2;
            }

            Slog.e(TAG, "processCmdV2, return null");
            return null;
        }

        HwParcel hidl_reply = new HwParcel();
        try {
            IHwBinder hwService = HwBinder.getService(SERVICE_NAME, "default");
            if (hwService != null) {
                HwParcel hidl_request = new HwParcel();
                hidl_request.writeInterfaceToken(INTERFACE_DESCRIPTOR);
                ArrayList sbuf = new ArrayList(Arrays.asList(HwBlob.wrapArray(param)));
                hidl_request.writeInt8Vector(sbuf);
                hidl_request.writeInt32(sbuf.size());
                hwService.transact(CODE_PROCESS_CMD, hidl_request, hidl_reply, 0);
                hidl_reply.verifySuccess();
                hidl_request.releaseTemporaryStorage();
                ArrayList<Byte> val = hidl_reply.readInt8Vector();
                byte[] array = new byte[val.size()];
                for (int i = 0; i < val.size(); i++) {
                    array[i] = ((Byte) val.get(i)).byteValue();
                }
                hidl_reply.release();
                return array;
            }
        } catch (RemoteException e) {
            Slog.e(TAG, "transact failed. ", e);
        } catch (Throwable th) {
            hidl_reply.release();
        }

        hidl_reply.release();
        Slog.e(TAG, "processCmdV2, return null");
        return null;
    }

    public void setExtInfo(int authType, String keyExtInfo, String valExtInfo) {
    }

    public int startBIOManager(Context context, int authType) {
        int res = ACTIVITY_START_FAILED;

        if (authType == IFAA_TYPE_FINGER) {
            Intent intent = new Intent("android.settings.SECURITY_SETTINGS");
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(intent);
            res = ACTIVITY_START_SUCCESS;
        }

        Slog.i(TAG, "startBIOManager authType:" + authType + " res:" + res);
        return res;
    }
}
