package org.ifaa.android.manager;

import android.annotation.UnsupportedAppUsage;

public abstract class IFAAManagerV4 extends IFAAManagerV3 {
    @UnsupportedAppUsage
    public abstract int getEnabled(int i);

    @UnsupportedAppUsage
    public abstract int[] getIDList(int i);
}
