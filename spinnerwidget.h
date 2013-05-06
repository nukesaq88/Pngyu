#ifndef SPINNERWIDGET_H
#define SPINNERWIDGET_H

#include <QLabel>
#include <QPixmap>
#include <QList>

// This class enables to visualize "waiting" or "loading", "executeing" condition.
// If you set SpinnerWidget in your layout, spinner pixmap begin spinning immidiately
class SpinnerWidget : public QLabel
{
public:
  explicit SpinnerWidget( QWidget *parent=0 ) :
    QLabel( parent ),
    m_current_index( 0 )
  {
    const QPixmap spinner( ":/icons/spinner.png" );
    const int origin_w = spinner.width();
    const int origin_h = spinner.height();

    for( int d = 0; d < 360; d += 15 )
    {
      const QPixmap &rotated =
          spinner.transformed( QMatrix().rotate(d), Qt::SmoothTransformation );
      // Trimming rotated image size same as original size,
      // because rotated image is always larger than original size
      const int diff_w = rotated.width() - origin_w;
      const int diff_h = rotated.height() - origin_h;
      const QPixmap &trimmed =
          rotated.copy( diff_w / 2, diff_h / 2, origin_w, origin_h );
      m_spinner_list.push_back( trimmed );
    }

    setScaledContents( true );
    setPixmap( spinner );

    startTimer( 50 );
  }

protected:
  virtual void timerEvent(QTimerEvent *)
  {
    if( m_spinner_list.empty() || ! isVisible() )
    {
      return;
    }

    if( m_current_index >= m_spinner_list.size() ||
        m_current_index < 0 )
    {
      m_current_index = 0;
    }

    setPixmap( m_spinner_list.at( m_current_index ) );

    m_current_index++;
  }

private:

  QList<QPixmap> m_spinner_list;
  int m_current_index;
};

#endif // SPINNERWIDGET_H
