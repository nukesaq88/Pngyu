#ifndef BASICIMAGEVIEW_H
#define BASICIMAGEVIEW_H

#include <QGraphicsView>

class BasicImageView : public QGraphicsView
{
  Q_OBJECT
public:
  explicit BasicImageView(QWidget *parent = 0);

  void setImage( const QImage &image );
  void setPixmap( const QPixmap &pixmap );

protected:
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void drawBackground(QPainter *painter, const QRectF &rect);
  virtual void wheelEvent(QWheelEvent *event);

private:

  QPixmap m_pixmap;
  QPointF m_lastMovePoint;

signals:
//  void mouseWheelScrolled( int delta );

public slots:

};

#endif // BASICIMAGEVIEW_H
