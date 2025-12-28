@echo off
REM Deploy script for Windows distribution
REM Run this after building in Qt Creator (Release mode)

echo Starting deployment...
echo.

setlocal

REM Use environment variable if set, otherwise use default
if not defined BUILD_DIR set BUILD_DIR=build\Desktop_Qt_6_10_1_MinGW_64_bit-Release\release
set APP_NAME=Pngyu.exe

echo Looking for: %BUILD_DIR%\%APP_NAME%
echo.

if not exist "%BUILD_DIR%\%APP_NAME%" (
    echo Error: %APP_NAME% not found in %BUILD_DIR%
    echo Please build the project in Qt Creator first (Release mode)
    exit /b 1
)

echo Deploying Qt DLLs and plugins...
windeployqt "%BUILD_DIR%\%APP_NAME%" --release --no-translations

echo.
echo Copying pngquant...
if not exist "%BUILD_DIR%\pngquant_bin" mkdir "%BUILD_DIR%\pngquant_bin"
copy /Y "pngquant_bin\win\pngquant.exe" "%BUILD_DIR%\"

echo.
echo ✓ Deployment complete!
echo Distributable app: %BUILD_DIR%\%APP_NAME%
echo.
echo You can now distribute the contents of the %BUILD_DIR% folder.
echo.
pause
