@echo off
setlocal

echo ===========================================
echo Building RTBPlayer...
echo ===========================================

:: Find MSBuild
for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set VS_INSTALL_PATH=%%i
)

if "%VS_INSTALL_PATH%"=="" (
    echo [ERROR] Visual Studio installation not found.
    pause
    exit /b 1
)

set MSBUILD_PATH=%VS_INSTALL_PATH%\MSBuild\Current\Bin\MSBuild.exe
if not exist "%MSBUILD_PATH%" (
    echo [ERROR] MSBuild.exe not found.
    pause
    exit /b 1
)

echo Found MSBuild at: "%MSBUILD_PATH%"

:: Build Release
echo.
echo Building RTBPlayer [Release x64]...
"%MSBUILD_PATH%" RTBPlayer.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild /m
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo ===========================================
echo RTBPlayer built successfully.
echo Output: ..\RTBEngineEditor\RTBEngine_SDK\Bin\RTBPlayer.exe
echo ===========================================
pause
