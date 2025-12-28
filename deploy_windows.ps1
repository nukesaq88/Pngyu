# Deploy script for Windows distribution
# Run this after building in Qt Creator (Release mode)

Write-Host "Starting deployment..." -ForegroundColor Green
Write-Host ""

# Use environment variable if set, otherwise use default
$BuildDir = if ($env:BUILD_DIR) { $env:BUILD_DIR } else { "build\Desktop_Qt_6_10_1_MinGW_64_bit-Release\release" }
$AppName = "Pngyu.exe"
$AppPath = Join-Path $BuildDir $AppName

Write-Host "Looking for: $AppPath"
Write-Host ""

# Check if the executable exists
if (-not (Test-Path $AppPath)) {
    Write-Host "Error: $AppName not found in $BuildDir" -ForegroundColor Red
    Write-Host "Please build the project in Qt Creator first (Release mode)" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Current directory: $PWD"
    
    if (Test-Path $BuildDir) {
        Write-Host "Directory exists: $BuildDir" -ForegroundColor Yellow
        Write-Host "Contents:"
        Get-ChildItem $BuildDir | Format-Table Name, Length, LastWriteTime
    } else {
        Write-Host "Directory does not exist: $BuildDir" -ForegroundColor Red
    }
    exit 1
}

Write-Host "Found: $AppPath" -ForegroundColor Green
Write-Host ""

# Deploy Qt DLLs and plugins
Write-Host "Deploying Qt DLLs and plugins..." -ForegroundColor Cyan
Write-Host "Running: windeployqt `"$AppPath`" --release --no-translations"
Write-Host ""

try {
    & windeployqt "$AppPath" --release --no-translations
    if ($LASTEXITCODE -ne 0) {
        throw "windeployqt failed with exit code $LASTEXITCODE"
    }
    Write-Host ""
    Write-Host "windeployqt completed successfully" -ForegroundColor Green
} catch {
    Write-Host ""
    Write-Host "Error: windeployqt failed" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host ""
    Write-Host "Make sure Qt bin directory is in your PATH" -ForegroundColor Yellow
    Write-Host "Example: C:\Qt\6.10.1\mingw_64\bin" -ForegroundColor Yellow
    exit 1
}

Write-Host ""

# Copy pngquant executable
Write-Host "Copying pngquant executable..." -ForegroundColor Cyan
$PngquantSrc = "pngquant_bin\win\pngquant.exe"
$PngquantDest = Join-Path $BuildDir "pngquant.exe"

if (Test-Path $PngquantSrc) {
    Copy-Item $PngquantSrc $PngquantDest -Force
    Write-Host "Copied: $PngquantSrc -> $PngquantDest" -ForegroundColor Green
} else {
    Write-Host "Warning: pngquant not found at $PngquantSrc" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Deployment completed successfully!" -ForegroundColor Green
Write-Host "You can now distribute the contents of: $BuildDir" -ForegroundColor Cyan
