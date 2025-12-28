# Deploy script for Windows distribution
# Run this after building in Qt Creator (Release mode)

Write-Host "Starting deployment..." -ForegroundColor Green
Write-Host ""

# Use environment variable if set, otherwise use default
$BuildDir = if ($env:BUILD_DIR) { $env:BUILD_DIR } else { "build\Desktop_Qt_6_10_1_MinGW_64_bit-Release\release" }
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

# Find windeployqt
$windeployqt = $null

# First check if it's in PATH
$windeployqt = Get-Command windeployqt -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Source

# If not in PATH, search common Qt installation locations
if (-not $windeployqt) {
    Write-Host "windeployqt not found in PATH, searching for Qt installation..." -ForegroundColor Yellow
    
    $qtSearchPaths = @(
        "C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe",
        "C:\Qt\6.10.0\mingw_64\bin\windeployqt.exe",
        "C:\Qt\6.9.3\mingw_64\bin\windeployqt.exe",
        "C:\Qt6\6.10.1\mingw_64\bin\windeployqt.exe",
        "C:\Qt6\6.10.0\mingw_64\bin\windeployqt.exe"
    )
    
    # Also search in Qt online installer locations
    $qtRoot = "C:\Qt"
    if (Test-Path $qtRoot) {
        $qtVersions = Get-ChildItem $qtRoot -Directory | Where-Object { $_.Name -match '^\d+\.\d+' }
        foreach ($version in $qtVersions) {
            $mingwDirs = Get-ChildItem $version.FullName -Directory -ErrorAction SilentlyContinue | Where-Object { $_.Name -match 'mingw' }
            foreach ($mingw in $mingwDirs) {
                $qtSearchPaths += Join-Path $mingw.FullName "bin\windeployqt.exe"
            }
        }
    }
    
    foreach ($path in $qtSearchPaths) {
        if (Test-Path $path) {
            $windeployqt = $path
            Write-Host "Found Qt at: $windeployqt" -ForegroundColor Green
            break
        }
    }
}

if (-not $windeployqt) {
    Write-Host ""
    Write-Host "Error: windeployqt not found" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please add Qt bin directory to your PATH or install Qt." -ForegroundColor Yellow
    Write-Host "Example: C:\Qt\6.10.1\mingw_64\bin" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Or set the PATH temporarily:" -ForegroundColor Yellow
    Write-Host '  $env:PATH += ";C:\Qt\6.10.1\mingw_64\bin"' -ForegroundColor Cyan
    exit 1
}

# Deploy Qt DLLs and plugins
Write-Host ""
Write-Host "Deploying Qt DLLs and plugins..." -ForegroundColor Cyan
Write-Host "Running: $windeployqt `"$DeployAppPath`" --release --no-translations"
Write-Host ""

try {
    & $windeployqt "$DeployAppPath" --release --no-translations
    if ($LASTEXITCODE -ne 0) {
        throw "windeployqt failed with exit code $LASTEXITCODE"
    }
    Write-Host ""
    Write-Host "windeployqt completed successfully" -ForegroundColor Green
} catch {
    Write-Host ""
    Write-Host "Error: windeployqt failed" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}

Write-Host ""

# Copy pngquant executable
Write-Host "Copying pngquant executable..." -ForegroundColor Cyan
$PngquantSrc = "pngquant_bin\win\pngquant.exe"
$PngquantDest = Join-Path $DeployDir "pngquant.exe"
$PngquantDest = Join-Path $BuildDir "pngquant.exe"

if (Test-Path $PngquantSrc) {
    Copy-Item $PngquantSrc $PngquantDest -Force
    Write-Host "Copied: $PngquantSrc -> $PngquantDest" -ForegroundColor Green
} else {
    Write-Host "Warning: pngquant not found at $PngquantSrc" -ForegroundColor Yellow
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
Write-Host "To create a ZIP archive, run:" -ForegroundColor Yellow
Write-Host "  Compress-Archive -Path '$DeployDir\*' -DestinationPath 'Pngyu-Windows-x64.zip'" -ForegroundColor Cyan
