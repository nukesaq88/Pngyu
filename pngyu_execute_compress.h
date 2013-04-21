#ifndef PNGYU_EXECUTE_COMPRESS_H
#define PNGYU_EXECUTE_COMPRESS_H

#include <QString>
#include <QPair>

#include "pngyu_option.h"



namespace pngyu
{


// execute pnngquant command
// returns <succeed,error_string>
QPair<bool,QString> execute_compress(
    const QString &pngquant_command
    );

}

#endif // PNGYU_EXECUTE_COMPRESS_H
