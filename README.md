# key-board-hook
基于win32api的底层键盘钩子

# 编译
    make.bat 或 make

# 使用
程序通过main.cpp中的"outfile_path"宏记录键盘按键,每1min保存一次

通过托盘图标在一定时间内记录(默认一直记录)或退出