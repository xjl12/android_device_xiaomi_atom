# 源码补丁

鉴于本项目不编译适配vendor，而是直接使用官方vendor分区，所以难免会造成冲突，必须及时对源码进行修正。为规范化源码补丁，本文件夹所存补丁都是由`git format-patch -N`命令导出。

## 使用方法

在源码根目录执行：
```
bash /device/xiaomi/atom/patch/patch.sh
```
请自行留意补丁合并过程中的错误