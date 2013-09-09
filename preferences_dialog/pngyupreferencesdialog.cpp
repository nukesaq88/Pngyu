#include "pngyupreferencesdialog.h"
#include "ui_pngyupreferencesdialog.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>

PngyuPreferencesDialog::PngyuPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PngyuPreferencesDialog)
{
  ui->setupUi(this);

#ifndef Q_OS_MACX
  ui->groupBox_imageoptim_integration->setVisible( false );
#endif

  ui->pushButton_ok->setVisible( false );
  ui->pushButton_cancel->setVisible( false );

  connect( ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(ok_pushed()) );
  connect( ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(cancel_pushed()) );
  connect( ui->pushButton_close, SIGNAL(clicked()), this, SLOT(cancel_pushed()) );
  connect( ui->pushButton_apply, SIGNAL(clicked()), this, SLOT(apply_pushed()) );

  connect( ui->toolButton_open_imageoptim_location, SIGNAL(clicked()), SLOT(open_image_optim_location_pushed()) );

  // preference changed
  connect( ui->spinBox_n_jobs, SIGNAL(valueChanged(int)), this, SLOT(preference_changed()) );
  connect( ui->comboBox_imageoptim_integration, SIGNAL(currentIndexChanged(int)), this, SLOT(preference_changed()) );
  connect( ui->lineEdit_imageoptim_location, SIGNAL(textChanged(QString)), this, SLOT(preference_changed()) );
  connect( ui->lineEdit_imageoptim_location, SIGNAL(textChanged(QString)), this, SLOT(image_optim_location_changed()) );

}

PngyuPreferencesDialog::~PngyuPreferencesDialog()
{
  delete ui;
}

bool PngyuPreferencesDialog::is_apply_button_enabled() const
{
  return ui->pushButton_apply->isEnabled();
}

void PngyuPreferencesDialog::set_apply_button_enabled( bool b )
{
  ui->pushButton_apply->setEnabled( b );
}

void PngyuPreferencesDialog::set_n_jobs( const int n )
{
  ui->spinBox_n_jobs->setValue( n );
}

int PngyuPreferencesDialog::n_jobs() const
{
  return ui->spinBox_n_jobs->value();
}

void PngyuPreferencesDialog::set_image_optim_integrate_mode( const pngyu::ImageOptimIntegration mode )
{
  QComboBox *combobox = ui->comboBox_imageoptim_integration;
  switch( mode )
  {
    case pngyu::IMAGEOPTIM_ALWAYS_ENABLED:
    {
    combobox->setCurrentIndex( 1 );
    break;
    }
    case pngyu::IMAGEOPTIM_ALWAYS_DISABLED:
    {
    combobox->setCurrentIndex( 2 );
    break;
    }
    default:
    {
    combobox->setCurrentIndex( 0 );
    }
  }
}

pngyu::ImageOptimIntegration PngyuPreferencesDialog::image_optim_integrate_mode()
{
  switch( ui->comboBox_imageoptim_integration->currentIndex() )
  {
    case 1:
    {
    return pngyu::IMAGEOPTIM_ALWAYS_ENABLED;
    }
    case 2:
    {
    return pngyu::IMAGEOPTIM_ALWAYS_DISABLED;
    }
    default:
    {
    return pngyu::IMAGEOPTIM_ASK_EVERY_TIME;
    }
  }
  return pngyu::IMAGEOPTIM_ASK_EVERY_TIME;
}


void PngyuPreferencesDialog::set_image_optim_path( const QString &path )
{
  ui->lineEdit_imageoptim_location->setText( path );
}

QString PngyuPreferencesDialog::image_optim_path() const
{
  return ui->lineEdit_imageoptim_location->text();
}

void PngyuPreferencesDialog::cancel_pushed()
{
  reject();
}

void PngyuPreferencesDialog::ok_pushed()
{
  accept();
}

void PngyuPreferencesDialog::close_pushed()
{
  reject();
}

void PngyuPreferencesDialog::apply_pushed()
{
  emit apply_pushed_signal();
  set_apply_button_enabled( false );
}

void PngyuPreferencesDialog::preference_changed()
{
  set_apply_button_enabled( true );
}

void PngyuPreferencesDialog::image_optim_location_changed()
{
  QLineEdit * const line_edit = ui->lineEdit_imageoptim_location;
  QPalette palette = line_edit->palette();
  if( QFile::exists( image_optim_path() ) )
  {
    palette.setBrush( QPalette::Text, QBrush() );
  }
  else
  { // if imageoptim path is invalid, change text color
    palette.setBrush( QPalette::Text, QBrush(Qt::red) );
  }
  line_edit->setPalette( palette );
}

void PngyuPreferencesDialog::open_image_optim_location_pushed()
{
  const QString default_dir = "/Applications";

  const QString path = QFileDialog::getOpenFileName(
        this,
        QString(),
        default_dir );

  if( ! path.isEmpty() )
  {
    set_image_optim_path( path );
  }
}
