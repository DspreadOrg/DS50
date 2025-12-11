:: Copyright (C) 2018 RDA Technologies Limited and\or its affiliates("RDA").
:: All rights reserved.
::
:: This software is supplied "AS IS" without any warranties.
:: RDA assumes no responsibility or liability for the use of the software,
:: conveys no license or title under any patent, copyright, or mask work
:: right to the product. RDA reserves the right to make changes in the
:: software without notification.  RDA also make no representation or
:: warranty that such application will be suitable for the specified use
:: without further testing or modification.

:start
@echo off

set Start_at=%time%
set curtitle=cmd: %~n0 %*
title %curtitle% @ %time%

set PROJECT_ROOT=%CD%
set BUILD_TARGET=appimage
set BUILD_RELEASE_TYPE=debug
set PROJECT_OUT=%PROJECT_ROOT%\out\%BUILD_TARGET%_%BUILD_RELEASE_TYPE%

call :add_path %PROJECT_ROOT%\prebuilts\win32\bin
call :add_path %PROJECT_ROOT%\prebuilts\win32\cmake\bin
call :add_path %PROJECT_ROOT%\prebuilts\win32\python3
call :add_path %PROJECT_ROOT%\prebuilts\win32\gcc-arm-none-eabi\bin
call :add_path %PROJECT_ROOT%\tools
call :add_path %PROJECT_ROOT%\tools\win32

if not exist %PROJECT_OUT% mkdir %PROJECT_OUT%

cd /d %PROJECT_OUT%
::rd /s/q .\
cmake ..\.. -G  Ninja
ninja
:: cd /d %PROJECT_ROOT%
:: cd /d sign_vanstone\
:: .\VanstoneSign.exe ..\out\appimage_debug\hex\Lcldsd_app.img
cd /d %PROJECT_ROOT%

echo ************************************************Success************************************************


pause
goto start
:doneseg

	echo=
	echo %date%
	echo START TIME:  %Start_at%
	echo END TIME:    %time%
	echo=
	goto exit


:exit
exit /B 0
:add_path
(echo ";%PATH%;" | find /C /I ";%1;" > nul) || set "PATH=%1;%PATH%"
goto :eof
