#ifndef PNGYU_CUSTOM_TABLEWIDGET_ITEM_H
#define PNGYU_CUSTOM_TABLEWIDGET_ITEM_H

#include <QTableWidgetItem>

namespace pngyu {
class TableValueCompareItem : public QTableWidgetItem {
 public:
  TableValueCompareItem(int type = Type) : QTableWidgetItem(type) {}
  explicit TableValueCompareItem(const QString& text, int type = Type)
      : QTableWidgetItem(text, type) {}
  explicit TableValueCompareItem(const QIcon& icon, const QString& text,
                                 int type = Type)
      : QTableWidgetItem(icon, text, type) {}
  TableValueCompareItem(const QTableWidgetItem& other)
      : QTableWidgetItem(other) {}

  bool operator<(const QTableWidgetItem& other) const {
    return (this->data(1).toDouble() < other.data(1).toDouble());
  }
};

}  // namespace pngyu

#endif  // PNGYU_CUSTOM_TABLEWIDGET_ITEM_H
