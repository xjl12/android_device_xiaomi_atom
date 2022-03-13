# 专为 Redmi 10X 5G 打造的 Octavi OS 12

***

## 感谢
致敬本项目的[上游项目](https://github.com/Project-Cezanne/android_device_xiaomi_cezanne)中各位前辈的工作

![赞助方式](https://xjl12.gitee.io/shou_kuang_ma_1.jpg)
感谢以下网友的[**资助**](https://xjl12.gitee.io)😘
| 支付宝 | 微信  |
| --- | --- |
|   *静   |  C*.
|  **宇  |  A*l |
|  **芳  |   |

## 安装步骤
1. 首先备份数据并通过小米官方[解锁设备](https://www.miui.com/unlock/)
2. 下载以下文件
    > [MIUI开发版22.1.19](https://bigota.d.miui.com/22.1.19/miui_ATOM_22.1.19_87c2a4d62e_11.0.zip)。推荐使用MIUI开发版官改做底包  
    >  [刷机包](https://github.com/xjl12/android_device_xiaomi_atom/releases)   
    > [OrangeFox](https://github.com/ymdzq/OFRP-device_xiaomi_bomb/releases)或其他可用的Android 11 TWRP Recovery 镜像  
    > vbmeta 镜像    
    > [百度网盘](https://pan.baidu.com/s/1z4lBBNVvYPMRvRpqe-eJVg)（密码：atom）            
    > [阿里云盘](https://www.aliyundrive.com/s/Kx8Cu1LLoCh)（密码：v85l，文件需要转换后才能用）
3. 重启至Recovery模式，若Data分区已加密或不清楚状况则格式化Data分区，若未加密只需清除/data而无需清除/data/media
4. 刷入MIUI开发版22.1.19（推荐使用同版本的官改）
5. 刷入OctaviOS 12刷机包 
6. **禁用Data分区加密**     
    > MIUI官改包已经禁用了强制加密，无需此步             
    > 禁用Data加密的方法很多，下面只介绍其中一种     
    > Recovery下电脑连接手机，键入以下命令([橙狐Rec](https://github.com/ymdzq/OFRP-device_xiaomi_bomb/releases)测试通过)   
    > `adb shell`    
    > `mount /dev/block/dm-1 /vendor`    
    > `busybox vi /vendor/etc/fstab.mt6873`
    > 修改第23行行尾附近`fileencryption`改为`encryptable`  
    > 以同样的方法修改/vendor/etc/fstab.emmc
    > 如果空间不够可以运行：`rm -r /vendor/data-app`
    >> 如果文件不存在，打开一片空白，执行：     
           `umount /vendor ; mount /dev/block/dm-2 /vendor ; busybox vi /vendor/etc/fstab.mt6873`           
    >> 提示：VI编辑器具体使用请百度，按i进入编辑，编辑完成后按ESC退出编辑模式，再输入:wq回车保存退出   
8. 重启至bootloader模式，输入命令：
   ```
   fastboot --disable-verity --disable-verification flash vbmeta vbmeta.img
   fastboot reboot
   ```
## Redmi 10X Pro (bomb)
理论上可以安装到Pro机型，但未经实际测试，
注意底包选择Pro的[MIUI-22.1.19](https://bigota.d.miui.com/22.1.19/miui_BOMB_22.1.19_2f7ea53a62_11.0.zip)或对应版本的官改
刷完所有包后，最后再补上`bomb-xjl12-patched.img`至boot重启即可

## 升级步骤

从Octavi 12升级无需清除数据，从上述第五步执行

## 安装MIUI双系统

下载或制作dsu.zip存放至内置存储的根目录(/sdcard/dsu.zip)，在开发者选项中点击DSU Loader即可安装官方MIUI开发版为第二系统（默认Data大小8G）.

## 正常工作
指纹（指纹支付、解锁和息屏指纹）、VoLTE、自动亮度、WiFi、3.5mm耳机孔、5G SA、短信、通话、蓝牙

##  已知问题
* 相机后置摄像头最高像素12MP且后置摄像头录像模式噪点严重
* 系统自带录屏功能异常

## 注意事项
* 相机可以使用Google相机，请自行搜索下载
* 不支持运营商视频通话，请勿打开设置中的这一开关（默认关闭），否则可能会无法拨出电话
* 默认桌面为[Lawnchair](https://github.com/LawnchairLauncher/lawnchair)，使用其他桌面请安装相应桌面的Magisk模块
* 若反复重启卡Redmi，请在Bootloader下禁用vbmeta验证
* 若卡开机动画，请禁用Data强制加密并格式化Data分区
* 若反复重启进Recovery，请禁用Data分区强制加密
* SELinux为permissive模式

## 安装部分MIUI优质软件
* 首先安装Magisk模块[MiuiCore](https://github.com/reiryuki/Miui-Core-Magisk-Module)
* [MIUI桌面](https://github.com/reiryuki/Miui-Home-Magisk-Module)
* [MIUI相册](https://github.com/reiryuki/Miui-Gallery-Magisk-Module)
* [MIUI录屏](https://github.com/reiryuki/Miui-Screen-Recorder-Magisk-Module)

## DSU包制作方法

1. 下载MIUI开发版卡刷包，从中解压出system.new.dat.br、system.transfer.list、product.new.dat.br、product.transfer.list
2. 运行以下命令
```
brotli -d system.new.dat.br
brotli -d product.new.dat.br
python sdat2img.py system.transfer.list system.new.dat system.img
python sdat2img.py product.transfer.list product.new.dat product.img
zip dsu.zip system.img product.img
```
> 注意：Magisk 23及以下版本在制作DSU包时，要将system.img 改名为system_root.img

## 郑重声明
**刷机有风险，由此造成的损失与作者无关。本项目为免费项目，未组建任何QQ微信群聊，系统发布及讨论渠道为[GitHub](https://github.com/xjl12/android_device_xiaomi_atom/releases)以及[酷安](https://www.coolapk.com/u/597475)，如需在本系统基础上二次改造，请与我联系！(xiao-xjle@outlook.com)**
