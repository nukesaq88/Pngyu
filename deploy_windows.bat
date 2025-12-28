@echo off
REM Deploy script for Windows distribution
REM Run this after building in Qt Creator (Release mode)

setlocal

set BUILD_DIR=build\Desktop-Release\release
set APP_NAME=Pngyu.exe

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
