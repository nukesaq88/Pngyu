#include "pngyuimageoptimintegrationquestiondialog.h"
#include "ui_pngyuimageoptimintegrationquestiondialog.h"

PngyuImageOptimIntegrationQuestionDialog::PngyuImageOptimIntegrationQuestionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PngyuImageOptimIntegrationQuestionDialog)
{
  ui->setupUi(this);
}

PngyuImageOptimIntegrationQuestionDialog::~PngyuImageOptimIntegrationQuestionDialog()
{
  delete ui;
}

bool PngyuImageOptimIntegrationQuestionDialog::is_dont_ask_next_checked() const
{
  return ui->checkBox_dont_ask_next_time->isChecked();
}
