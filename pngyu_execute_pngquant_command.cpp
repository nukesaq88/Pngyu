#include "pngyu_execute_pngquant_command.h"

#include <QProcess>
#include <QDir>
#include <QApplication>
#include <QDebug>

namespace pngyu
{

QStringList find_executable_pngquant_from_dir( const QDir &dir )
{
  QStringList found_paths;
  if( ! dir.exists() )
  {
    return found_paths;
  }
  foreach( const QFileInfo &child_file_info, dir.entryInfoList() )
  {
    if( ! child_file_info.baseName().contains( QRegExp( "pngquant", Qt::CaseInsensitive ) ) ||
        ! child_file_info.isExecutable() )
    {
      continue;
    }
    if( pngyu::is_executable_pnqguant( child_file_info ) )
    {
      found_paths.push_back( child_file_info.absoluteFilePath() );
    }
  }
  return found_paths;
}

QString pngquant_version( const QString &pnqquant_path )
{
  QProcess process;
  process.setProcessEnvironment( QProcessEnvironment::systemEnvironment() );
  process.start( pnqquant_path + " --version" );
  process.waitForFinished();
  const QString &version = process.readAllStandardOutput();
  return version.trimmed();
}

bool is_executable_pnqguant( const QFileInfo pngquant_path )
{
  if( ! pngquant_path.baseName().contains( QRegExp( "pngquant", Qt::CaseInsensitive ) ) ||
      ! pngquant_path.isExecutable() )
  {
    return false;
  }
  const QString &version = pngquant_version( pngquant_path.absoluteFilePath() );
  return ! version.isEmpty();
}

QStringList find_executable_pngquant()
{
  QStringList search_dirs;

#ifdef Q_OS_MACX
  search_dirs << ( QApplication::applicationDirPath() + "/../Resources/" );
#endif
#ifdef Q_OS_UNIX
  search_dirs << "/usr/bin" << "/usr/local/bin" << "/usr/sbin";
#endif
#ifdef Q_OS_WIN

#endif
  QStringList found_paths;
  foreach( const QString &dir, search_dirs )
  {
    found_paths.append( find_executable_pngquant_from_dir( QDir(dir) ) );
  }


  return found_paths;
}

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
    process.closeWriteChannel();
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
