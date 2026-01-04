# Build script for Windows

param(
    [string]$BuildType = "Release"
)

# Get the directory where this script is located
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir

Write-Host "Building Pngyu ($BuildType)..." -ForegroundColor Green
Write-Host ""

$BuildDir = Join-Path $ProjectRoot "build\$BuildType"

# Create build directory
Write-Host "Creating build directory: $BuildDir" -ForegroundColor Cyan
New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null

# Navigate to build directory
Push-Location $BuildDir

try {
    # Configure with CMake
    Write-Host ""
    Write-Host "Configuring CMake..." -ForegroundColor Cyan
    cmake $ProjectRoot.
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }

    # Build
    Write-Host ""
    Write-Host "Building..." -ForegroundColor Cyan
    cmake --build . --config $BuildType
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }

    Write-Host ""
    Write-Host "✓ Build complete!" -ForegroundColor Green
    Write-Host "Application: $BuildDir\$BuildType\Pngyu.exe" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "To create a distributable package, run:" -ForegroundColor Yellow
    Write-Host "  .\scripts\deploy_win.ps1" -ForegroundColor White
    
} catch {
    Write-Host ""
    Write-Host "Error: $_" -ForegroundColor Red
    Pop-Location
    exit 1
}

Pop-Location
