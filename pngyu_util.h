#ifndef PNGYU_UTIL_H
#define PNGYU_UTIL_H

#include <cmath>

#include <QWidget>
#include <QApplication>
#include <QCursor>
#include <QFileInfo>
#include <QImageReader>
#include <QRegularExpression>
#include <QDir>
#include <QFile>
#include <QByteArray>
#include <QStringList>
#include <QProcess>
#include <QIcon>

namespace pngyu
{
namespace util
{
namespace detail
{
  const QColor DEFAULT_ENABLE_COLOR = QColor("lightskyblue");
}

inline QString app_home_path()
{
  return QDir::homePath() + "/.pngyu";
}

inline QString app_temporay_path()
{
  return app_home_path() + "/temp";
}

inline bool ma_app_home_path()
{
  return QDir().mkpath( app_home_path() );
}

inline bool make_app_temporary_path()
{
  return QDir().mkpath( app_temporay_path() );
}

inline QByteArray png_file_to_bytearray( const QString &filename )
{
  QFile f( filename );
  f.open( QIODevice::ReadOnly );
  return f.readAll();
}

inline bool write_png_data( const QString &filename, const QByteArray png_data )
{
  QFile f( filename );
  if( ! f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
  {
    return false;
  }

  if( f.write( png_data ) != png_data.size() )
  {
    return false;
  }
  return true;
}

inline QString size_to_string_kb( const qint64 size )
{
  return QString( "%1KB" )
      .arg( static_cast<double>(size) / 1024, 0, 'f', 2 );
}

inline QString size_to_string_mb( const qint64 size )
{
  return QString( "%1MB" )
      .arg( static_cast<double>(size) / ( 1024 * 1024 ), 0, 'f', 2 );
}

inline bool has_png_extention( const QFileInfo &file )
{
  return file.suffix().compare("png", Qt::CaseInsensitive) == 0;
}

inline bool can_read_png_file( const QFileInfo &file )
{
  QImageReader image_reader( file.absoluteFilePath(), "png" );
  return image_reader.canRead();
}

inline QImage read_thumbnail_image( const QString &filename, const int size )
{
  QImageReader image_reader( filename );
  const QSize &origin_size = image_reader.size();
  const double scale =  static_cast<double>( size ) / std::max( origin_size.width(), origin_size.height() );
  const QSize dst_size( scale * origin_size.width(), scale * origin_size.height() );
  image_reader.setScaledSize( dst_size );
  return image_reader.read();
}

inline bool is_under_mouse( const QWidget * const widget )
{
  return QRect( QPoint(), widget->size() ).contains( widget->mapFromGlobal( QCursor::pos() ) );
}

inline void set_drop_enabled_palette(
    QWidget * const widget,
    const bool enabled,
    const QColor &enable_color = detail::DEFAULT_ENABLE_COLOR )
{
  QPalette palette = widget->palette();
  if( enabled )
  {
    palette.setBrush( QPalette::Base, QBrush( enable_color ) );
  }
  else
  {
    palette.setBrush( QPalette::Base, QPalette().brush( QPalette::Base ) );
  }
  widget->setPalette( palette );
}

inline void set_drop_here_stylesheet(
    QWidget * const widget,
    const bool drag_hoverring,
    const QColor &hoverring_color = detail::DEFAULT_ENABLE_COLOR )
{
//  widget->setStyleSheet();
  QString stylesheet =
      "QWidget{"
      "background-image : url(:/background/drop_here.png);"
      "background-position: center ;"
      "background-repeat : repeat-none;";
  if( drag_hoverring )
  {
    stylesheet += "background-color : " + hoverring_color.name() + ";\n";
  }
  else
  {
    stylesheet += "background-color : " + QColor(Qt::white).name() + ";\n";
  }
  stylesheet += "}";

  widget->setStyleSheet( stylesheet );
}

inline const QPixmap& success_icon_pixmap()
{
  static QPixmap p( ":/icons/check.png" );
  return p;
}

inline const QPixmap& failure_icon_pixmap()
{
  static QPixmap p( ":/icons/stop.png" );
  return p;
}

inline const QIcon& success_icon()
{
  static QIcon i( success_icon_pixmap() );
  return i;
}

inline const QIcon& failure_icon()
{
  static QIcon i( failure_icon_pixmap() );
  return i;
}

inline bool open_with_mac_app( const QStringList &files, const QString app_path )
{
//  QStringList args;
//  args.push_back( "-c" ); // python option
//  { // make python script
//    QString script;
//    script += "# -*- coding: utf-8-mac -*-\n"
//              "import AppKit\n"
//              "w = AppKit.NSWorkspace.sharedWorkspace()\n";
//    foreach( const QString &filepath, files )
//    {
//      script += QString("w.openFile_withApplication_(u'%1', '%2')\n" ).arg( filepath, app_path );
//    }
//    args.push_back( script );
//  }
//  return QProcess::execute( "/usr/bin/pythonw", args );
  return QProcess::startDetached( "/usr/bin/open", (QStringList() << "-a" << app_path) += files );
  //return QProcess::startDetached( app_path, files );
  //return QProcess::execute( app_path, files );
}

inline const QString& dot_path()
{
  static QString p = QFileInfo( QApplication::applicationDirPath() ).absoluteFilePath();
  return p;
}

inline const QString& dot_dot_path()
{
  static QString p = QFileInfo( dot_path() + "/.." ).absoluteFilePath();
  return p;
}

inline QString to_dot_path( const QString &path )
{
  const QString &dot = dot_path();
  const QString &dot_dot = dot_dot_path();

  QString dot_path = QFileInfo( path ).absoluteFilePath();

  dot_path.replace( dot, "." );
  dot_path.replace( dot_dot, ".." );

  return dot_path;
}

inline QString from_dot_path( const QString &path )
{
  const QString &dot = QFileInfo( QApplication::applicationDirPath() ).absoluteFilePath();
  const QString &dot_dot = QFileInfo( dot_path() + "/.." ).absoluteFilePath();

  QString abs_path = path;
  abs_path.replace( "../", dot_dot + "/" );
  abs_path.replace( "./", dot + "/" );
  return abs_path;
}


} // namespace util
} // namespace pngyu


#endif // PNGYU_UTIL_H
