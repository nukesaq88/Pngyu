# Translation Files

This directory contains internationalization (i18n) translation files for Pngyu.

## Quick Start

### 1. Update Translation Files

Extract translatable strings from source code:

```bash
./scripts/update_translations.sh
```

This will scan all source files and update `.ts` files with new strings.

### 2. Edit Translations

Open Qt Linguist to translate strings:

```bash
# The update script will show you the exact command
linguist translations/pngyu_ja.ts
```

In Qt Linguist:
- Review each source string and its context
- Enter translations
- Mark translations as complete (green checkmark)
- Save the file (Ctrl+S / Cmd+S)

### 3. Build Application

```bash
./scripts/build_mac.sh
```

CMake will automatically compile `.ts` files into `.qm` binary files during the build process.

## Supported Languages

- Japanese (ja): `pngyu_ja.ts` - ✅ 104/105 translations complete
- English (en): `pngyu_en.ts` - English source strings (no translation needed)
- Simplified Chinese (zh): `pngyu_zh.ts` - ⚠️ Translation needed

## File Types

### .ts (Translation Source)
- **Format**: XML (human-readable)
- **Purpose**: Source file for translators
- **Editing**: Qt Linguist or text editor
- **Version Control**: ✅ Commit to Git

### .qm (Qt Message)
- **Format**: Binary (compiled)
- **Purpose**: Used by application at runtime
- **Generation**: Automatically generated from `.ts` by `lrelease`
- **Version Control**: ❌ Do not commit (build artifact)

## Workflow

```
1. Code changes (add new tr() strings)
   ↓
2. Run ./scripts/update_translations.sh
   ↓
3. Edit translations with Qt Linguist
   ↓
4. Build application (generates .qm files)
   ↓
5. Application loads appropriate .qm based on system locale
```

## Adding New Languages

1. Add new `.ts` file to `CMakeLists.txt`:
   ```cmake
   set(TS_FILES
       translations/pngyu_ja.ts
       translations/pngyu_en.ts
       translations/pngyu_zh.ts  # Example: Chinese
   )
   ```

2. Add corresponding `.qm` file to `resource/resource.qrc`:
   ```xml
   <file>../translations/pngyu_zh.qm</file>
   ```

3. Run update script to create the new `.ts` file
4. Translate strings in Qt Linguist
5. Build application

## Troubleshooting

### Translations not appearing

1. Verify `.qm` files are generated in `build/Release/`
2. Check that `.qm` files are included in `resource.qrc`
3. Clean and rebuild:
   ```bash
   rm -rf build/Release
   ./scripts/build_mac.sh
   ```

## Notes

- All translatable strings must be wrapped with `tr()` or `QObject::tr()`
- The application automatically loads translations based on system locale
- Default fallback is English if no matching translation is found
- `.qm` files are automatically generated during build and should not be manually edited
