package org.ifaa.android.manager;

import android.compat.annotation.UnsupportedAppUsage;
import android.content.Context;

public class IFAAManagerFactory {
    @UnsupportedAppUsage
    public static IFAAManager getIFAAManager(Context context, int authType) {
        return IFAAManagerImpl.getInstance(context);
    }
}
