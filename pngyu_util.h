#ifndef PNGYU_UTIL_H
#define PNGYU_UTIL_H

#include <QWidget>
#include <QApplication>
#include <QCursor>
#include <QFileInfo>
#include <QImageReader>
#include <QRegExp>
#include <QDir>

namespace pngyu
{
namespace util
{
namespace detail
{
  const QColor ENABLE_COLOR = QColor("lightskyblue");
}

QString app_home_path()
{
  return QDir::homePath() + "/.pngyu";
}

QString app_temporay_path()
{
  return app_home_path() + "/temp";
}

bool ma_app_home_path()
{
  return QDir().mkpath( app_home_path() );
}

bool make_app_temporary_path()
{
  return QDir().mkpath( app_temporay_path() );
}

QString size_to_string( const qint64 size )
{
  return QString( "%1kb" ).arg( static_cast<double>(size) / 1000, 0, 'f', 2 );
//  return QString( "%1kb" ).arg( static_cast<double>(size) / 1000, );
}

bool has_png_extention( const QFileInfo &file )
{
  return QRegExp( "png", Qt::CaseInsensitive ).exactMatch( file.suffix() );
}

bool can_read_png_file( const QFileInfo &file )
{
  QImageReader image_reader( file.absoluteFilePath(), "png" );
  return image_reader.canRead();
}

bool is_under_mouse( const QWidget * const widget )
{
  return QRect( QPoint(), widget->size() ).contains( widget->mapFromGlobal( QCursor::pos() ) );
}

void set_drop_enabled_palette(
    QWidget * const widget,
    bool enabled,
    const QColor &enable_color = detail::ENABLE_COLOR )
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

}
}

#endif // PNGYU_UTIL_H
