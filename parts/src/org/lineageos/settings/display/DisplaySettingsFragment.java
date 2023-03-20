/*
 * Copyright (C) 2020 YAAP
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

import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.view.MenuItem;
import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceFragment;
import androidx.preference.SwitchPreference;

import vendor.xiaomi.hardware.displayfeature.V1_0.IDisplayFeature;

import org.lineageos.settings.R;
import org.lineageos.settings.utils.FileUtils;

public class DisplaySettingsFragment extends PreferenceFragment implements
        OnPreferenceChangeListener {

    private SwitchPreference mHBMPreference,mDCPreference;
    private IDisplayFeature mDisplayFeature;
    private static final String HBM_ENABLE_KEY = "hbm_mode";
    private static final String DC_ENABLE_KEY = "dc_dimming_mode";
    private static final String HBM_NODE = "/sys/class/drm/card0-DSI-1/disp_param";

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.display_settings);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);
        mHBMPreference = (SwitchPreference) findPreference(HBM_ENABLE_KEY);
        mDCPreference = (SwitchPreference) findPreference(DC_ENABLE_KEY);
        if (FileUtils.fileExists(HBM_NODE)) {
            mHBMPreference.setEnabled(true);
            mHBMPreference.setOnPreferenceChangeListener(this);
        } else {
            mHBMPreference.setSummary(R.string.hbm_enable_summary_not_supported);
            mHBMPreference.setEnabled(false);
        }
        try {
            mDisplayFeature = IDisplayFeature.getService();
            mDCPreference.setEnabled(true);
            mDCPreference.setOnPreferenceChangeListener(this);
        } catch (RemoteException e) {
            mDCPreference.setSummary(R.string.dc_dimming_mode_not_supported);
            mDCPreference.setEnabled(false);
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        final boolean cur_value = (Boolean) newValue;
        if (HBM_ENABLE_KEY.equals(preference.getKey())) {
            FileUtils.writeLine(HBM_NODE, cur_value ? "0x20000" : "0xE0000");
        } else if (DC_ENABLE_KEY.equals(preference.getKey())) {
            try {
                mDisplayFeature.setFeature(0, 20, cur_value ? 1 : 0, 255);
                ((DisplaySettingsActivity) getActivity()).callOnDCSwitch(cur_value);
            } catch (RemoteException e) {
                mDCPreference.setSummary(R.string.dc_dimming_mode_not_supported);
                mDCPreference.setEnabled(false);
            }
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

}
