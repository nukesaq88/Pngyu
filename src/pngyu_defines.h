#ifndef PNGYU_DIFINES_H
#define PNGYU_DIFINES_H

#include <QString>
#include <QTableWidget>

#include "pngyu_pngquant_option.h"

namespace pngyu {

enum Version { VERSION_MAJOR = 1, VERSION_MINOR = 0, VERSION_REVISION = 3 };

enum CompressOptionMode {
  COMPRESS_OPTION_UNKNOWN = -1,
  COMPRESS_OPTION_DEFAULT = 0,
  COMPRESS_OPTION_CUSTOM
};

enum OutputOptionMode {
  OUTPUT_OPTION_UNKNOWN = -1,
  OUTPUT_OPTION_OVERWITE_ORIGINAL = 0,
  OUTPUT_OPTION_CUSTOM
};

enum OuputDirectoryMode {
  OUTPUT_DIR_UNKNOWN = -1,
  OUTPUT_DIR_SAME = 0,
  OUTPUT_DIR_OTHER
};

enum OutputFinenameMode {
  OUTPUT_FILE_UNKNOWN = -1,
  OUTPUT_FILE_SAME_AS_ORIGINAL = 0,
  OUTPUT_FILE_CUSTOM
};

enum ImageOptimIntegration {
  IMAGEOPTIM_ASK_EVERY_TIME = 0,
  IMAGEOPTIM_ALWAYS_ENABLED,
  IMAGEOPTIM_ALWAYS_DISABLED
};

enum TableColumn {
  COLUMN_BASENAME = 0,
  COLUMN_ABSOLUTE_PATH,
  COLUMN_RESULT,
  COLUMN_ORIGINAL_SIZE,
  COLUMN_OUTPUT_SIZE,
  COLUMN_SAVED_SIZE,
  COLUMN_SAVED_SIZE_RATE,
  TABLE_COLUMN_COUNT
};

class CompressResult {
public:
  CompressResult() : result(false), error_message() {}

  bool result;
  QString error_message;
  QString src_path;
  QString dst_path;
  qint64 src_size;
  qint64 dst_size;
};

class CompressQueueData {
public:
  CompressQueueData() : table_widget(0) {}

  QString src_path;
  QString dst_path;
  QString pngquant_path;
  pngyu::PngquantOption pngquant_option;
  bool overwrite_enabled;
  bool force_execute_if_negative;

  QTableWidget *table_widget;
  int table_row;
};

} // namespace pngyu

#endif // PNGYU_DIFINES_H
