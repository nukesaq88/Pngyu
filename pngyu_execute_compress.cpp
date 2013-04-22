#include "pngyu_execute_compress.h"

#include <QProcess>

namespace pngyu
{

QPair<bool,QString> execute_compress(
    const QString &pngquant_command )
{
  typedef QPair<bool,QString> result_t;

  bool compress_succeed = true;
  QString error_string;

  try
  {
    QProcess process;
    process.setProcessEnvironment( QProcessEnvironment::systemEnvironment() );
    process.start( pngquant_command );

    if( ! process.waitForStarted() )
    {
      throw QString( "Error: Process cannot started" );
    }
    if( ! process.waitForFinished() )
    {
      throw QString( "Error: Process timeout" );
    }
    const int exit_code = process.exitCode();
    if( process.exitCode() != 0 )
    {
      throw QString( "Error code : %1\nCause : %2" )
          .arg( exit_code )
          .arg( QString(process.readAllStandardError()) );
    }
  }
  catch( const QString &e )
  {
    compress_succeed = false;
    error_string = e;
  }
  return result_t( compress_succeed, error_string );
}

QPair<QByteArray,QString> execute_compress_stdio_mode(
    const QString &pngquant_command,
    const QByteArray &src_png_data
    )
{
  typedef QPair<QByteArray,QString> result_t;

  QByteArray dst_png_data;
  QString error_string;
  try
  {
    if( src_png_data.isEmpty() )
    {
      throw QString( "Error: Original data is empty" );
    }

    QProcess process;
    process.setProcessEnvironment( QProcessEnvironment::systemEnvironment() );
    process.start( pngquant_command );

    if( ! process.waitForStarted() )
    {
      throw QString( "Error: Process cannot started" );
    }
    process.write( src_png_data );
    if( ! process.waitForFinished() )
    {
      throw QString( "Error: Process timeout" );
    }
    const int exit_code = process.exitCode();
    if( process.exitCode() != 0 )
    {
      throw QString( "Error code : %1\nCause : %2" )
          .arg( exit_code )
          .arg( QString(process.readAllStandardError()) );
    }
    dst_png_data = process.readAllStandardOutput();
  }
  catch( const QString &e )
  {
    dst_png_data.clear();
    error_string = e;
  }
  return result_t( dst_png_data, error_string );
}

}
