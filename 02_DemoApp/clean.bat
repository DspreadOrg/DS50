@echo off
rem 设置cmd窗口的大小
rem mode con cols=60 lines=40
mode con cols=60

call tools\core_launch.bat
rd /s /q out\appimage_debug
