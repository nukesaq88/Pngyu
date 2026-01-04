#ifndef PNGYU_EXECUTE_PNGQUANT_COMMAND_H
#define PNGYU_EXECUTE_PNGQUANT_COMMAND_H

#include <QByteArray>
#include <QFileInfo>
#include <QPair>
#include <QString>

#include "pngyu_pngquant_option.h"

namespace pngyu {

QString pngquant_version(const QString& pnqquant_path);

bool is_executable_pnqguant(const QFileInfo pngquant_path);

QStringList find_executable_pngquant();

// execute pnngquant command
// returns <succeed,error_string>
QPair<bool, QString> execute_compress(const QString& pngquant_command);

// execute pnngquant command with stdio mode
// returns <dst_png_data,error_string>
// if dst_png_data is null, this function has failed
QPair<QByteArray, QString> execute_compress_stdio_mode(
    const QString& pngquant_command, const QByteArray& src_png_data);

}  // namespace pngyu

#endif  // PNGYU_EXECUTE_PNGQUANT_COMMAND_H
