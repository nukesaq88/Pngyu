#include "basicimageview.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>
#include <cmath>

BasicImageView::BasicImageView(QWidget* parent) : QGraphicsView(parent) {
  setScene(new QGraphicsScene(this));
  setMouseTracking(true);

  setInteractive(true);

  // Always show scrollbars
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void BasicImageView::setImage(const QImage& image) {
  setPixmap(QPixmap::fromImage(image));
}

void BasicImageView::setPixmap(const QPixmap& pixmap) {
  m_pixmap = pixmap;
  setSceneRect(QRectF(pixmap.rect()));
  viewport()->repaint();
}

void BasicImageView::mouseMoveEvent(QMouseEvent* event) {
  const QPointF pos = event->pos();

  if (event->buttons() == Qt::LeftButton) {
    const QPointF delta = pos - m_lastMovePoint;

    QScrollBar* h_bar = horizontalScrollBar();
    QScrollBar* v_bar = verticalScrollBar();

    h_bar->setValue(static_cast<int>(h_bar->value() - delta.x()));
    v_bar->setValue(static_cast<int>(v_bar->value() - delta.y()));
  }
  m_lastMovePoint = pos;
  QGraphicsView::mouseMoveEvent(event);
}

void BasicImageView::drawBackground(QPainter* painter, const QRectF& rect) {
  const QRectF rectf = rect.intersected(sceneRect());

  painter->fillRect(rectf, backgroundBrush());

  painter->fillRect(rectf, QBrush(m_pixmap));
}

void BasicImageView::wheelEvent(QWheelEvent* event) {
  const int delta = event->angleDelta().y();
  const double SCALING_STEP = 1.2;
  if (0 < delta) {
    scale(SCALING_STEP, SCALING_STEP);
  } else if (0 > delta) {
    scale(1.0 / SCALING_STEP, 1.0 / SCALING_STEP);
  }
}
