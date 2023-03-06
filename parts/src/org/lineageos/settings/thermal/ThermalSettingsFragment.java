/**
 * Copyright (C) 2020 The LineageOS Project
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
package org.lineageos.settings.thermal;

import android.annotation.Nullable;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.SectionIndexer;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.preference.PreferenceFragment;

import com.android.settingslib.applications.ApplicationsState;

import org.lineageos.settings.R;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ThermalSettingsFragment extends PreferenceFragment
        implements AdapterView.OnItemClickListener, ApplicationsState.Callbacks {

    private AllPackagesAdapter mAllPackagesAdapter;
    private ApplicationsState mApplicationsState;
    private ApplicationsState.Session mSession;
    private ActivityFilter mActivityFilter;
    private Map<String, ApplicationsState.AppEntry> mEntryMap =
            new HashMap<String, ApplicationsState.AppEntry>();

    private ListView mUserListView;

    private ThermalUtils mThermalUtils;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mApplicationsState = ApplicationsState.getInstance(getActivity().getApplication());
        mSession = mApplicationsState.newSession(this);
        mSession.onResume();
        mActivityFilter = new ActivityFilter(getActivity().getPackageManager());

        mAllPackagesAdapter = new AllPackagesAdapter(getActivity());

        mThermalUtils = new ThermalUtils(getActivity());
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        return inflater.inflate(R.layout.thermal_layout, container, false);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
    }

    @Override
    public void onViewCreated(final View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        mUserListView = view.findViewById(R.id.thermal_list_view);
        mUserListView.setAdapter(mAllPackagesAdapter);
        mUserListView.setOnItemClickListener(this);
    }


    @Override
    public void onResume() {
        super.onResume();
        getActivity().setTitle(getResources().getString(R.string.thermal_title));
        rebuild();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        mSession.onPause();
        mSession.onDestroy();
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        ViewHolder holder = (ViewHolder) view.getTag();
        holder.mode.performClick();
    }

    @Override
    public void onPackageListChanged() {
        mActivityFilter.updateLauncherInfoList();
        rebuild();
    }

    @Override
    public void onRebuildComplete(ArrayList<ApplicationsState.AppEntry> entries) {
        if (entries != null) {
            handleAppEntries(entries);
            mAllPackagesAdapter.notifyDataSetChanged();
        }
    }

    @Override
    public void onLoadEntriesCompleted() {
        rebuild();
    }

    @Override
    public void onAllSizesComputed() {
    }

    @Override
    public void onLauncherInfoChanged() {
    }

    @Override
    public void onPackageIconChanged() {
    }

    @Override
    public void onPackageSizeChanged(String packageName) {
    }

    @Override
    public void onRunningStateChanged(boolean running) {
    }

    private void handleAppEntries(List<ApplicationsState.AppEntry> entries) {
        final ArrayList<String> sections = new ArrayList<String>();
        final ArrayList<Integer> positions = new ArrayList<Integer>();
        final PackageManager pm = getActivity().getPackageManager();
        String lastSectionIndex = null;
        int offset = 0;

        for (int i = 0; i < entries.size(); i++) {
            final ApplicationInfo info = entries.get(i).info;
            final String label = (String) info.loadLabel(pm);
            final String sectionIndex;

            if (!info.enabled) {
                sectionIndex = "--"; // XXX
            } else if (TextUtils.isEmpty(label)) {
                sectionIndex = "";
            } else {
                sectionIndex = label.substring(0, 1).toUpperCase();
            }

            if (lastSectionIndex == null ||
                    !TextUtils.equals(sectionIndex, lastSectionIndex)) {
                sections.add(sectionIndex);
                positions.add(offset);
                lastSectionIndex = sectionIndex;
            }

            offset++;
        }

        mAllPackagesAdapter.setEntries(entries, sections, positions);
        mEntryMap.clear();
        for (ApplicationsState.AppEntry e : entries) {
            mEntryMap.put(e.info.packageName, e);
        }
    }

    private void rebuild() {
        mSession.rebuild(mActivityFilter, ApplicationsState.ALPHA_COMPARATOR);
    }

    private int getStateDrawable(int state) {
        switch (state) {
            case ThermalUtils.STATE_BENCHMARK:
                return R.drawable.ic_thermal_benchmark;
            case ThermalUtils.STATE_BROWSER:
                return R.drawable.ic_thermal_browser;
            case ThermalUtils.STATE_CAMERA:
                return R.drawable.ic_thermal_camera;
            case ThermalUtils.STATE_DIALER:
                return R.drawable.ic_thermal_dialer;
            case ThermalUtils.STATE_GAMING:
                return R.drawable.ic_thermal_gaming;
            case ThermalUtils.STATE_DEFAULT:
            default:
                return R.drawable.ic_thermal_default;
        }
    }

    private static class ViewHolder {
        private TextView title;
        private Spinner mode;
        private ImageView icon;
        private View rootView;
        private ImageView stateIcon;

        private ViewHolder(View view) {
            this.title = view.findViewById(R.id.app_name);
            this.mode = view.findViewById(R.id.app_mode);
            this.icon = view.findViewById(R.id.app_icon);
            this.stateIcon = view.findViewById(R.id.state);
            this.rootView = view;

            view.setTag(this);
        }
    }

    private static class ModeAdapter extends BaseAdapter {

        private final LayoutInflater inflater;
        private final TypedValue textColorSecondary;
        private final int textColor;
        private final int[] items = {
                R.string.thermal_default,
                R.string.thermal_benchmark,
                R.string.thermal_browser,
                R.string.thermal_camera,
                R.string.thermal_dialer,
                R.string.thermal_gaming,
        };

        private ModeAdapter(Context context) {
            inflater = LayoutInflater.from(context);

            textColorSecondary = new TypedValue();
            context.getTheme().resolveAttribute(com.android.internal.R.attr.textColorSecondary,
                    textColorSecondary, true);
            textColor = context.getColor(textColorSecondary.resourceId);
        }

        @Override
        public int getCount() {
            return items.length;
        }

        @Override
        public Object getItem(int position) {
            return items[position];
        }

        @Override
        public long getItemId(int position) {
            return 0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            TextView view;
            if (convertView != null) {
                view = (TextView) convertView;
            } else {
                view = (TextView) inflater.inflate(android.R.layout.simple_spinner_dropdown_item,
                        parent, false);
            }

            view.setText(items[position]);
            view.setTextColor(textColor);
            view.setTextSize(14f);

            return view;
        }
    }

    private class AllPackagesAdapter extends BaseAdapter
            implements AdapterView.OnItemSelectedListener, SectionIndexer {

        private final LayoutInflater mInflater;
        private final ModeAdapter mModesAdapter;
        private List<ApplicationsState.AppEntry> mEntries = new ArrayList<>();
        private String[] mSections;
        private int[] mPositions;

        public AllPackagesAdapter(Context context) {
            mInflater = LayoutInflater.from(context);
            mModesAdapter = new ModeAdapter(context);
            mActivityFilter = new ActivityFilter(context.getPackageManager());
        }

        @Override
        public int getCount() {
            return mEntries.size();
        }

        @Override
        public Object getItem(int position) {
            return mEntries.get(position);
        }

        @Override
        public boolean hasStableIds() {
            return true;
        }

        @Override
        public long getItemId(int position) {
            return mEntries.get(position).id;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder holder;
            if (convertView == null) {
                holder = new ViewHolder(mInflater.inflate(
                        R.layout.thermal_list_item, parent, false));
                holder.mode.setAdapter(mModesAdapter);
                holder.mode.setOnItemSelectedListener(this);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            ApplicationsState.AppEntry entry = mEntries.get(position);

            if (entry == null) {
                return holder.rootView;
            }

            holder.title.setText(entry.label);
            mApplicationsState.ensureIcon(entry);
            holder.icon.setImageDrawable(entry.icon);
            holder.mode.setSelection(mThermalUtils.getStateForPackage(entry.info.packageName),
                    false);
            holder.mode.setTag(entry);
            holder.stateIcon.setImageResource(getStateDrawable(
                    mThermalUtils.getStateForPackage(entry.info.packageName)));
            return holder.rootView;
        }

        private void setEntries(List<ApplicationsState.AppEntry> entries,
                List<String> sections, List<Integer> positions) {
            mEntries = entries;
            mSections = sections.toArray(new String[sections.size()]);
            mPositions = new int[positions.size()];
            for (int i = 0; i < positions.size(); i++) {
                mPositions[i] = positions.get(i);
            }
            notifyDataSetChanged();
        }


        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            final ApplicationsState.AppEntry entry = (ApplicationsState.AppEntry) parent.getTag();
            switch (position) {
                case ThermalUtils.STATE_DEFAULT:
                    mThermalUtils.writePackage(entry.info.packageName, ThermalUtils.STATE_DEFAULT);
                    break;
                case ThermalUtils.STATE_BENCHMARK:
                    mThermalUtils.writePackage(entry.info.packageName,
                            ThermalUtils.STATE_BENCHMARK);
                    break;
                case ThermalUtils.STATE_BROWSER:
                    mThermalUtils.writePackage(entry.info.packageName, ThermalUtils.STATE_BROWSER);
                    break;
                case ThermalUtils.STATE_CAMERA:
                    mThermalUtils.writePackage(entry.info.packageName, ThermalUtils.STATE_CAMERA);
                    break;
                case ThermalUtils.STATE_DIALER:
                    mThermalUtils.writePackage(entry.info.packageName, ThermalUtils.STATE_DIALER);
                    break;
                case ThermalUtils.STATE_GAMING:
                    mThermalUtils.writePackage(entry.info.packageName, ThermalUtils.STATE_GAMING);
                    break;
            }
            notifyDataSetChanged();
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
        }

        @Override
        public int getPositionForSection(int section) {
            if (section < 0 || section >= mSections.length) {
                return -1;
            }

            return mPositions[section];
        }

        @Override
        public int getSectionForPosition(int position) {
            if (position < 0 || position >= getCount()) {
                return -1;
            }

            final int index = Arrays.binarySearch(mPositions, position);

            /*
             * Consider this example: section positions are 0, 3, 5; the supplied
             * position is 4. The section corresponding to position 4 starts at
             * position 3, so the expected return value is 1. Binary search will not
             * find 4 in the array and thus will return -insertPosition-1, i.e. -3.
             * To get from that number to the expected value of 1 we need to negate
             * and subtract 2.
             */
            return index >= 0 ? index : -index - 2;
        }

        @Override
        public Object[] getSections() {
            return mSections;
        }
    }

    private class ActivityFilter implements ApplicationsState.AppFilter {

        private final PackageManager mPackageManager;
        private final List<String> mLauncherResolveInfoList = new ArrayList<String>();

        private ActivityFilter(PackageManager packageManager) {
            this.mPackageManager = packageManager;

            updateLauncherInfoList();
        }

        public void updateLauncherInfoList() {
            Intent i = new Intent(Intent.ACTION_MAIN);
            i.addCategory(Intent.CATEGORY_LAUNCHER);
            List<ResolveInfo> resolveInfoList = mPackageManager.queryIntentActivities(i, 0);

            synchronized (mLauncherResolveInfoList) {
                mLauncherResolveInfoList.clear();
                for (ResolveInfo ri : resolveInfoList) {
                    mLauncherResolveInfoList.add(ri.activityInfo.packageName);
                }
            }
        }

        @Override
        public void init() {
        }

        @Override
        public boolean filterApp(ApplicationsState.AppEntry entry) {
            boolean show = !mAllPackagesAdapter.mEntries.contains(entry.info.packageName);
            if (show) {
                synchronized (mLauncherResolveInfoList) {
                    show = mLauncherResolveInfoList.contains(entry.info.packageName);
                }
            }
            return show;
        }
    }
}
