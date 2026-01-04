# Pngyu

Pngyu is a Front-end GUI application for [pngquant](https://pngquant.org/).

## Download

Download the latest version from the [Homepage](https://nukesaq88.github.io/Pngyu/).

Pre-built binaries are available for Windows and macOS.

## Building from Source

Pngyu can be built on any platform that supports Qt and pngquant command-line tool.

### Requirements
- Qt 6.x
- CMake 3.16 or later
- C++17 compatible compiler
- pngquant binary (included in `pngquant_bin` directory)

### Build Instructions

#### Command Line

**macOS:**
```bash
./scripts/build_mac.sh
```

The built application will be available at `build/Release/Pngyu.app`.

To build Debug version:
```bash
BUILD_TYPE=Debug ./scripts/build_mac.sh
```

**Windows:**
```powershell
.\scripts\build_win.ps1
```

To build Debug version:
```powershell
.\scripts\build_win.ps1 -BuildType Debug
```

#### Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. In the "Configure Project" screen, set the build directory to `build/Release` (for Release builds) or `build/Debug` (for Debug builds)
3. Build the project (Ctrl+B / Cmd+B)

The built application will be in the configured build directory.

### Distribution

After building, use the deployment scripts to create distributable packages:

**macOS:**
```bash
./scripts/deploy_mac.sh
```

This will bundle Qt frameworks and create a DMG file.

**Windows:**
```powershell
.\scripts\deploy_win.ps1
```

This will bundle Qt DLLs and create an archive.

The deployment scripts work with both command-line and Qt Creator builds, as long as the build directory is `build/Release` (configurable via `BUILD_DIR` environment variable).

## License

Pngyu itself is distributed under the BSD license.

However, the pre-built binaries include pngquant, which is licensed under GPL v3 or later.
See https://github.com/kornelski/pngquant for pngquant's license details.