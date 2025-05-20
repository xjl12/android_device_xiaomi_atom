# Generic Android ROM for Redmi 10X 5G

### You can download the prebuilt images from [Github Releases](https://github.com/xjl12/android_device_xiaomi_atom/releases).

## Build From Source

The compilation process of this project is mainly done on Linux platform（Recommend Ubuntu latest LTS version）. We mainly refer to the official build guide of Lineage OS (https://wiki.lineageos.org/devices/salami/build) to build it, and the brief steps are as follows:
1. Install the dependency packages. Open the system terminal and execute the following commands:
  ```
  sudo apt update && sudo apt -y install bc bison build-essential ccache curl flex g++-multilib gcc-multilib git git-lfs gnupg gperf imagemagick lib32readline-dev lib32z1-dev libelf-dev liblz4-tool lz4 libsdl1.2-dev libssl-dev libxml2 libxml2-utils lzop pngcrush rsync schedtool squashfs- tools xsltproc zip zlib1g-dev libncurses5
  ```

2. Create a working directory. Execute:
  ```
  mkdir -p ~/bin
  mkdir -p ~/android/lineage
  ```
  The ~/bin directory will contain the repo tool, the ~/android/lineage directory will contain the complete Lineage OS source code, and the project code will be cloned to the ~/android/lineage/device/xiaomi/atom directory.

3. Install the repo tool. Execute the following command:
  ```
  curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
  chmod a+x ~/bin/repo
  ```
4. Initialize the Lineage OS source code. Execute the following command:
  ```
  cd ~/android/lineage
  repo init -u https://github.com/LineageOS/android.git -b lineage-21.0 --git-lfs --no-clone-bundle
  repo sync
  ```
5. Deploy the project code. Execute the following command:
  ```
  mkdir -p ~/android/lineage/device/xiaomi/atom
  git clone https://github.com/xjl12/android_device_xiaomi_atom -b lieage-21.0 ~/android/lineage/device/xiaomi/atom
  ```
6. Extract the device private library. Insert a phone of the target model and make sure ADB is enabled, execute the following command:
  ```
  cd ~/android/lineage/device/xiaomi/atom
  . /extract-files.sh
  ```
7. Start building the Android ROM. execute the following command:
  ```
  cd ~/android/lineage
  source build/envsetup.sh
  lunch lineage_atom-trunk_staging-user
  make bacon
  ```
8. After the build is complete, you can find lineage-21.0-\<build date\>-UNOFFICIAL-atom.zip in the $OUT directory, which is the Lineage OS installer.
9. Unlock the device Bootloader, reboot to the third party [Recovery](https://github.com/HuaZoffice/OFRP-device_xiaomi_atom/releases), install the ROM package generated in the previous step, clear all the data and reboot the device.

## Device specifications

The Redmi 10X 5G (codenamed _"atom"_) is a medium level smartphone from Xiaomi.

It was announced on May 26, 2020. Release date was June 2020.

Basic   | Spec Sheet
-------:|:-------------------------
SoC     | Mediatek MT6875 Dimensity 820 (7nm)
CPU     | Octa-core (4x2.6 GHz Cortex-A76 & 4x2.0 GHz Cortex-A55)
GPU     | Mali-G57 MC5
Memory  | 128GB 6GB RAM, 128GB 8GB RAM, 256GB 8GB RAM, 64GB 6GB RAM
Shipped Android Version | 10.0 with MIUI 11
Storage | 64/128/256 GB
Battery | Li-Po 4520 mAh, non-removable
Display | 1080 x 2400 pixels, 20:9 ratio, 6.57 inches, AMOLED, 60Hz, HDR10+, 400 nits
Camera  | 48MP(wide) 8MP(ultrawide) 2MP(telephoto macro) 2MP(depth)
## Device picture

![Redmi 10X 5G ](https://fdn2.gsmarena.com/vv/pics/xiaomi/xiaomi-redmi-10x-5g-1.jpg "Redmi 10X 5G")
