# Deploy script for Windows distribution
# Run this after building with CMake (Release mode)

# Get the directory where this script is located
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir

Write-Host "Starting deployment..." -ForegroundColor Green
Write-Host ""

# Use environment variable if set, otherwise use default
$DefaultBuildDir = Join-Path $ProjectRoot "build\Release\Release"
$BuildDir = if ($env:BUILD_DIR) { $env:BUILD_DIR } else { $DefaultBuildDir }
$AppName = "Pngyu.exe"
$AppPath = Join-Path $BuildDir $AppName

# Create deployment directory
$DeployDir = "build\Pngyu-Windows-x64"
$DeployAppPath = Join-Path $DeployDir $AppName

Write-Host "Looking for: $AppPath"
Write-Host ""

# Check if the executable exists
if (-not (Test-Path $AppPath)) {
    Write-Host "Error: $AppName not found in $BuildDir" -ForegroundColor Red
    Write-Host "Please build the project first:" -ForegroundColor Yellow
    Write-Host "  mkdir build\Release && cd build\Release" -ForegroundColor Yellow
    Write-Host "  cmake ../.." -ForegroundColor Yellow
    Write-Host "  cmake --build . --config Release" -ForegroundColor Yellow
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

# Create clean deployment directory
Write-Host "Creating deployment directory: $DeployDir" -ForegroundColor Cyan
if (Test-Path $DeployDir) {
    Remove-Item $DeployDir -Recurse -Force
    Write-Host "Removed existing deployment directory" -ForegroundColor Yellow
}
New-Item -ItemType Directory -Path $DeployDir -Force | Out-Null
Write-Host "Created: $DeployDir" -ForegroundColor Green
Write-Host ""

# Copy executable to deployment directory
Write-Host "Copying executable..." -ForegroundColor Cyan
Copy-Item $AppPath $DeployAppPath -Force
Write-Host "Copied: $AppName -> $DeployDir" -ForegroundColor Green
Write-Host ""

# Deploy Qt DLLs and plugins
Write-Host "Deploying Qt DLLs and plugins..." -ForegroundColor Cyan
Write-Host "Running: windeployqt `"$DeployAppPath`" --release --no-translations"
Write-Host ""

try {
    windeployqt "$DeployAppPath" --release --no-translations
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
    Write-Host "Please ensure Qt bin directory is in your PATH" -ForegroundColor Yellow
    exit 1
}

Write-Host ""

# Copy pngquant executable (already built with CMake in pngquant/ subdirectory)
Write-Host "Copying pngquant executable..." -ForegroundColor Cyan
$PngquantSrcDir = Join-Path $BuildDir "pngquant"
$PngquantSrc = Join-Path $PngquantSrcDir "pngquant.exe"
$PngquantDir = Join-Path $DeployDir "pngquant"

if (Test-Path $PngquantSrc) {
    New-Item -ItemType Directory -Path $PngquantDir -Force | Out-Null
    Copy-Item $PngquantSrc (Join-Path $PngquantDir "pngquant.exe") -Force
    Write-Host "Copied: pngquant/pngquant.exe -> $PngquantDir" -ForegroundColor Green
} else {
    Write-Host "Warning: pngquant.exe not found at $PngquantSrc" -ForegroundColor Yellow
    Write-Host "The executable should have been copied during build." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Deployment completed successfully!" -ForegroundColor Green
Write-Host "Distribution package created at: $DeployDir" -ForegroundColor Cyan
Write-Host ""
Write-Host "Contents:" -ForegroundColor Yellow
Get-ChildItem $DeployDir -Recurse -File | Measure-Object -Property Length -Sum | ForEach-Object {
    $totalSize = [math]::Round($_.Sum / 1MB, 2)
    Write-Host "  Total files: $($_.Count)" -ForegroundColor White
    Write-Host "  Total size: $totalSize MB" -ForegroundColor White
}
Write-Host ""

# Create ZIP archive
Write-Host "Creating ZIP archive..." -ForegroundColor Cyan
$ZipPath = "build\Pngyu-Windows-x64.zip"
if (Test-Path $ZipPath) {
    Remove-Item $ZipPath -Force
    Write-Host "Removed existing ZIP file" -ForegroundColor Yellow
}
Compress-Archive -Path "$DeployDir\*" -DestinationPath $ZipPath -Force
Write-Host "Created: $ZipPath" -ForegroundColor Green
Write-Host ""

$zipSize = [math]::Round((Get-Item $ZipPath).Length / 1MB, 2)
Write-Host "ZIP archive size: $zipSize MB" -ForegroundColor Cyan
Write-Host ""
Write-Host "Deployment and packaging completed!" -ForegroundColor Green
