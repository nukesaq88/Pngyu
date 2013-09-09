#ifndef PNGYUPREFERENCESDIALOG_H
#define PNGYUPREFERENCESDIALOG_H

#include <QDialog>
#include <QAbstractButton>

#include "pngyu_defines.h"

namespace Ui {
class PngyuPreferencesDialog;
}

class PngyuPreferencesDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit PngyuPreferencesDialog(QWidget *parent = 0);
  ~PngyuPreferencesDialog();

  bool is_apply_button_enabled() const;
  void set_apply_button_enabled( bool b );

  void set_n_jobs( const int n );
  int n_jobs() const;

  void set_image_optim_integrate_mode( const pngyu::ImageOptimIntegration mode );
  pngyu::ImageOptimIntegration image_optim_integrate_mode();

  void set_image_optim_path( const QString &path );
  QString image_optim_path() const;
  
private:
  Ui::PngyuPreferencesDialog *ui;

private slots:

  void cancel_pushed();
  void ok_pushed();
  void close_pushed();
  void apply_pushed();

  void preference_changed();

  void image_optim_location_changed();
  void open_image_optim_location_pushed();

signals:
  void apply_pushed_signal();
};

#endif // PNGYUPREFERENCESDIALOG_H
