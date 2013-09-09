#ifndef PNGYUIMAGEOPTIMINTEGRATIONQUESTIONDIALOG_H
#define PNGYUIMAGEOPTIMINTEGRATIONQUESTIONDIALOG_H

#include <QDialog>

namespace Ui {
class PngyuImageOptimIntegrationQuestionDialog;
}

class PngyuImageOptimIntegrationQuestionDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit PngyuImageOptimIntegrationQuestionDialog(QWidget *parent = 0);
  ~PngyuImageOptimIntegrationQuestionDialog();

  bool is_dont_ask_next_checked() const;
  
private:
  Ui::PngyuImageOptimIntegrationQuestionDialog *ui;
};

#endif // PNGYUIMAGEOPTIMINTEGRATIONQUESTIONDIALOG_H
