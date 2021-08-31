# 专为 Redmi 10X 5G 打造的 PixelExperience Plus 11

***

## 安装步骤
1. 首先通过小米官方[解锁设备](https://www.miui.com/unlock/)
2. 在[MIUI官网](https://www.miui.com/download-366.html#543)下载最新**MIUI稳定版**卡刷包并刷入
3. 下载以下文件
    >  [PixelExperience Plus 11 刷机包](https://github.com/xjl12/android_device_xiaomi_atom/releases)  
     >  [刷机包备用地址](https://pan.baidu.com/s/1m2kdz9o0fy03_m9y2QXsjA) （密码：juc8）  
    > [OrangeFox](https://github.com/ymdzq/OFRP-device_xiaomi_bomb/releases)或可用的Android 11 TWRP Recovery 镜像  
    > vbmeta 镜像
4. 在Bootloader下刷入TWRP或者OrangeFox，同时电脑输入以下命令禁用vbmeta验证：
   > fastboot --disable-verity --disable-verification flash vbmeta vbmeta.img
5. 重启至Recovery模式，若Data分区已加密则应该格式化Data分区，若未加密只需清除/data而无需清除/data/media
6. 直接刷入PixelExperience Plus 11刷机包（底包为**MIUI开发版**还需再刷入miui-dev-permissive-boot.img至Boot分区） 
7. （可选）修改vendor分区文件或以其他方式禁用Data分区加密
8. 重启手机，纵享丝滑！

## 升级步骤

1. 重启至Recovery，直接刷入ROM包（底包为**MIUI开发版**还需再刷入miui-dev-permissive-boot.img至Boot分区） 
2. 重启手机，体验新版！

## 正常工作
指纹（支持息屏指纹）、VoLTE、自动亮度、WiFi、3.5mm耳机孔、5G SA、短信、通话、蓝牙耳机

##  已知问题
* 蓝牙地址不正确
* 相机后置摄像头最高像素12MP且后置摄像头录像模式噪点严重
* TEE无法正常工作，不支持支付宝、微信指纹和人脸识别
* 指纹识别会有轻度闪烁，不影响正常使用
* HW叠加层有问题，部分界面会闪烁，停用HW叠加层可解决

## 注意事项
* 默认桌面为[Lawnchair](https://github.com/LawnchairLauncher/lawnchair) 
* 若当前网络受限，建议开机设置时不要连接Wifi，选择在离线状态下完成设置
* 本系统为userdebug版，adb默认打开且无需验证，电脑通过adb root命令即可获取root权限
* 若反复重启卡Redmi，请在Bootloader下禁用vbmeta验证
* 若发现WiFi等无法使用，建议使用OrangeFox刷入，开发板底包还请将miui-dev-permissive-boot.img刷写至Boot分区
