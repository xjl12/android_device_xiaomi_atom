<head>
    <script src="https://hm.baidu.com/hm.js?8a3608795648935457c4799145ab9d75" async="async"></script>
    <script src="https://cdn.jsdelivr.net/gh/xjl12/count@4/count" async="async"></script>
</head>

# 专为 Redmi 10X 5G 打造的 AOSP

***

## 感谢
致敬本项目的上游项目（android_device_xiaomi_cezanne）中各位前辈的工作

![赞助方式](/android_device_xiaomi_atom/skm.webp)


感谢以下网友的[**资助**](https://xjl12.gitee.io)😘

<table>
<thead>
<tr>
<th align="center">支付宝</th>
<th align="center">微信</th>
</tr>
</thead>
<tbody>
<tr>
<td align="center">**杰</td>
<td align="center">C*.</td>
</tr>
<tr>
<td align="center">**静</td>
<td align="center">A*l</td>
</tr>
<tr>
<td align="center">**宇</td>
<td align="center">**文</td>
</tr>
<tr>
<td align="center">**鹏</td>
<td align="center">*届</td>
</tr>
<tr>
<td align="center">**芳</td>
<td align="center">A*B</td>
</tr>
<tr>
<td align="center">**博</td>
<td align="center">さ*い</td>
</tr>
</tbody>
</table>

## 正常工作
指纹（指纹支付、解锁和息屏指纹）、VoLTE、自动亮度、WiFi、3.5mm耳机孔、5G SA、短信、通话、蓝牙

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

## 郑重声明
**刷机有风险，由此造成的损失与作者无关。本项目为免费项目，未组建任何QQ微信群聊，系统发布及讨论渠道为[GitHub](https://github.com/xjl12/android_device_xiaomi_atom/releases)，如需在本系统基础上二次改造，请与我联系！(xiao-xjle@outlook.com)**
