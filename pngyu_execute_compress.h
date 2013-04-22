#ifndef PNGYU_EXECUTE_COMPRESS_H
#define PNGYU_EXECUTE_COMPRESS_H

#include <QString>
#include <QByteArray>
#include <QPair>

#include "pngyu_option.h"

namespace pngyu
{

// execute pnngquant command
// returns <succeed,error_string>
QPair<bool,QString> execute_compress(
    const QString &pngquant_command
    );

// execute pnngquant command with stdio mode
// returns <dst_png_data,error_string>
// if dst_png_data is null, this function has failed
QPair<QByteArray,QString> execute_compress_stdio_mode(
    const QString &pngquant_command,
    const QByteArray &src_png_data
    );

}

#endif // PNGYU_EXECUTE_COMPRESS_H
