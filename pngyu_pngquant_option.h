#ifndef PNGYU_OPTION_H
#define PNGYU_OPTION_H

#include <cmath>
#include <QString>

namespace pngyu
{

class PngquantOption
{
public:

  PngquantOption() :
    m_out_suffix(),
    m_ncolors(-1),
    m_speed(-1),
    m_force_overwrite(false),
    m_ie6_alpha_support(false),
    m_disable_floyd_steinberg_dithering(false),
    m_timeout_ms(20000)
  {}

  ~PngquantOption(){}

  void set_out_suffix( const QString &suffix )
  {
    m_out_suffix = suffix;
  }

  QString get_out_suffix() const
  {
    return m_out_suffix;
  }

  void set_ncolors( const int n )
  {
    m_ncolors = n;
  }

  int get_ncolors() const
  {
    return m_ncolors;
  }

  void set_speed( const int speed )
  {
    m_speed = speed;
  }

  int get_speed() const
  {
    return m_speed;
  }

  void set_force_overwrite( const bool b )
  {
    m_force_overwrite = b;
  }

  bool get_force_overwrite() const
  {
    return m_force_overwrite;
  }

  void set_ie6_alpha_support( const bool b )
  {
    m_ie6_alpha_support = b;
  }

  bool get_ie6_frilendly() const
  {
    return m_ie6_alpha_support;
  }

  void set_floyd_steinberg_dithering_disabled( const bool b )
  {
    m_disable_floyd_steinberg_dithering = b;
  }

  bool get_floyd_steinberg_dithering_disabled() const
  {
    return m_disable_floyd_steinberg_dithering;
  }

  void set_timeout_ms( const int timeout_ms )
  {
    m_timeout_ms = timeout_ms;
  }

  int get_timeout_ms() const
  {
    return m_timeout_ms;
  }

  QString to_pngquant_command_option() const
  {
    QString command( " " );

    if( ! m_out_suffix.isEmpty() )
    {
      command += QString( "--ext %1 " ).arg( m_out_suffix );
    }
    if( m_force_overwrite )
    {
      command += QString( "--force " );
    }
    if( m_ie6_alpha_support )
    {
      command += QString( "--iebug " );
    }
    if( m_disable_floyd_steinberg_dithering )
    {
      command += QString( "--nofs " );
    }
    if( m_speed > 0 )
    {
      command += QString( "--speed %1 " ).arg( m_speed );
    }

    // ncolor
    if( m_ncolors > 0 )
    {
      command += QString( "%1 " ).arg( m_ncolors );
    }
    return command;
  }

  QString make_pngquant_command(const QString &src_file_path ) const
  {
    return to_pngquant_command_option() + src_file_path;
  }

  QString make_pngquant_command_option_stdio_mode() const
  {
    return to_pngquant_command_option() + " -";
  }

  friend inline bool operator==( const pngyu::PngquantOption &o1, const pngyu::PngquantOption &o2 );
  friend inline bool operator!=( const pngyu::PngquantOption &o1, const pngyu::PngquantOption &o2 );

private:
  QString m_out_suffix;
  int m_ncolors;
  int m_speed;
  bool m_force_overwrite;
  bool m_ie6_alpha_support;
  bool m_disable_floyd_steinberg_dithering;
  int m_timeout_ms;
};

inline bool operator!=( const pngyu::PngquantOption &o1, const pngyu::PngquantOption &o2 )
{
  return
      o1.m_out_suffix != o2.m_out_suffix ||
      o1.m_ncolors != o2.m_ncolors ||
      o1.m_speed != o2.m_speed ||
      o1.m_force_overwrite != o2.m_force_overwrite ||
      o1.m_ie6_alpha_support != o2.m_ie6_alpha_support ||
      o1.m_disable_floyd_steinberg_dithering != o2.m_disable_floyd_steinberg_dithering ||
      o1.m_timeout_ms != o2.m_timeout_ms;
}

inline bool operator==( const pngyu::PngquantOption &o1, const pngyu::PngquantOption &o2 )
{
  return !( o1 != o2 );
}



}

#endif // PNGYU_OPTION_H
