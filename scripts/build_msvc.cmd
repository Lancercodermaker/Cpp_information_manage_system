@echo off
setlocal
cd /d "%~dp0.."
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALL=%%I"
)
if not defined VS_INSTALL if exist "D:\C++\Common7\Tools\VsDevCmd.bat" set "VS_INSTALL=D:\C++"
if not exist "%VS_INSTALL%\Common7\Tools\VsDevCmd.bat" (
    echo Visual Studio C++ build tools not found.
    exit /b 1
)
call "%VS_INSTALL%\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
if not exist build mkdir build
cl /nologo /std:c++17 /EHsc /utf-8 /W4 /Iinclude src\main.cpp src\Product.cpp src\CsvUtil.cpp src\InventoryManager.cpp src\InputHelper.cpp src\Menu.cpp /Fo:build\ /Fe:build\supermarket_inventory.exe
exit /b %errorlevel%
