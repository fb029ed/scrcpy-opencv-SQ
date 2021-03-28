# scrcpy-opencv-SQ
## 项目介绍
https://github.com/Genymobile/scrcpy
实现了安卓手机的投屏和触控功能
本项目使用c++对scrcpy进行重构，提供了opencv中mat结构的图像便于后续处理
SQ(simple and quick)
* 因为只实现了一部分，所以项目比较简单清晰，容易二次开发
* 提供opencv Mat图像，可以对获取到的图像进行图像处理
* 对解码和图像处理进行了并行优化，实时性较好
* recipe中提供了一些已经实现好的自动化脚本
> 2021.3.27针对智慧树网课实现了自动刷课功能

## 使用方式
```shell
mkdir build
cd build 
cmake ..
make -j2
执行可执行文件
```

## TODO
* recipe写的还比较粗糙,改成工厂模式
* 游戏键盘映射还没做
* 变量的配置方式和相对路径
* 只实现了usb数据传输方式,wifi方式待实现
* 提供可直接运行的文件
* 私有仓库时期的一些命名和现在还不一致

