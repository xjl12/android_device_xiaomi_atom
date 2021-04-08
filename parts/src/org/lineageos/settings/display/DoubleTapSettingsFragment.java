/*
 * Copyright (C) 2018 The LineageOS Project
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
import android.view.MenuItem;
import android.content.Context;
import androidx.preference.Preference;
import android.content.SharedPreferences;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceFragment;
import androidx.preference.SwitchPreference;
import android.provider.Settings;

import org.lineageos.settings.R;

import vendor.xiaomi.hardware.touchfeature.V1_0.ITouchFeature;

public class DoubleTapSettingsFragment extends PreferenceFragment implements
        OnPreferenceChangeListener {

    private static final String D2TW_ENABLE_KEY = "dt2w_enable";
        public static final String SHAREDD2TW = "sharadeD2TW";

    private SwitchPreference mD2TWPreference;
    private ITouchFeature mTouchFeature;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
      addPreferencesFromResource(R.xml.dt2w_settings);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);
       try {
            mTouchFeature = ITouchFeature.getService();
        } catch (Exception e) {
            // Do nothing
        }
        mD2TWPreference = (SwitchPreference) findPreference(D2TW_ENABLE_KEY);
        mD2TWPreference.setEnabled(true);
        mD2TWPreference.setOnPreferenceChangeListener(this);
        enableD2TW(1);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if (D2TW_ENABLE_KEY.equals(preference.getKey())) {
            enableD2TW((Boolean) newValue ? 1 : 0);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            getActivity().onBackPressed();
            return true;
        }
        return false;
    }

    private void enableD2TW(int enable) {
        if (mTouchFeature == null) return;
        try {
            mTouchFeature.setTouchMode(14,enable);
            SharedPreferences preferences = getActivity().getSharedPreferences(SHAREDD2TW,Context.MODE_PRIVATE);
            SharedPreferences.Editor editor = preferences.edit();
             editor.putInt(SHAREDD2TW, enable);
              editor.commit();
            }catch (Exception e) {
            // Do nothing
        }
    }
}

