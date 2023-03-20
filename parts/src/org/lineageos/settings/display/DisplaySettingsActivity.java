/*
 * Copyright (C) 2015-2016 The CyanogenMod Project
 *               2020 YAAP
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

package org.lineageos.settings.display;

import android.os.Bundle;
import android.os.IBinder;
import android.content.ServiceConnection;
import android.content.Intent;
import android.content.ComponentName;

import org.lineageos.settings.doze.DozeService;

import com.android.settingslib.collapsingtoolbar.CollapsingToolbarBaseActivity;
import com.android.settingslib.widget.R;

public class DisplaySettingsActivity extends CollapsingToolbarBaseActivity {

    private static final String TAG_DCDIMMING = "dcdimming";
    private ServiceConnection conn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getFragmentManager().beginTransaction().replace(R.id.content_frame,
                new DisplaySettingsFragment(), TAG_DCDIMMING).commit();
    }

    public void callOnDCSwitch(boolean value) {
        conn = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName name, IBinder binder) {
                ((DozeService.DCBinder) binder).onDCSwitch(value);
            }
    
            @Override
            public void onServiceDisconnected(ComponentName name) {}
        };
        Intent intent = new Intent(this, DozeService.class);
        bindService(intent, conn, BIND_AUTO_CREATE);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(conn != null)
            unbindService(conn);
    }
}
