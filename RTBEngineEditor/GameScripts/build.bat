@echo off
setlocal

:: Change to the directory where the .bat is located to ensure it finds the .vcxproj
cd /d "%~dp0"

:: Defaults to Debug, but Release can be passed as a parameter (e.g. build.bat Release)
set CONFIGURATION=Debug
if not "%~1"=="" set CONFIGURATION=%~1

:: Find MSBuild using vswhere (included in Visual Studio Installer)
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found. Is Visual Studio installed?
    pause
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set "MSBUILD=%%i"
    goto :found_msbuild
)

echo [ERROR] MSBuild.exe not found.
pause
exit /b 1

:found_msbuild

echo [INFO] Compiling GameScripts (%CONFIGURATION%^|x64)...
"%MSBUILD%" GameScripts.vcxproj /p:Configuration=%CONFIGURATION% /p:Platform=x64 /t:Build

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Compilation FAILED!
    pause
    exit /b %ERRORLEVEL%
)

echo [INFO] GameScripts.dll has been successfully replaced in the target directory.

pause
exit /b 0