#include "pngyupreferencesdialog.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>

#include "pngyu_execute_pngquant_command.h"
#include "pngyu_util.h"
#include "ui_pngyupreferencesdialog.h"

PngyuPreferencesDialog::PngyuPreferencesDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::PngyuPreferencesDialog) {
  ui->setupUi(this);

#ifndef Q_OS_MACOS
  ui->groupBox_imageoptim_integration->setVisible(false);
#endif

  ui->pushButton_ok->setVisible(false);
  ui->pushButton_cancel->setVisible(false);

  connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(ok_pushed()));
  connect(ui->pushButton_cancel, SIGNAL(clicked()), this,
          SLOT(cancel_pushed()));
  connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(cancel_pushed()));
  connect(ui->pushButton_apply, SIGNAL(clicked()), this, SLOT(apply_pushed()));

  connect(ui->toolButton_open_pngquant_location, SIGNAL(clicked()), this,
          SLOT(open_pngquant_location_pushed()));
  connect(ui->comboBox_pngquant_path, SIGNAL(editTextChanged(QString)), this,
          SLOT(pngquant_location_changed()));

  connect(ui->toolButton_open_imageoptim_location, SIGNAL(clicked()), this,
          SLOT(open_image_optim_location_pushed()));
  connect(ui->lineEdit_imageoptim_location, SIGNAL(textChanged(QString)), this,
          SLOT(image_optim_location_changed()));

  // preference changed
  connect(ui->spinBox_n_jobs, SIGNAL(valueChanged(int)), this,
          SLOT(preference_changed()));
  connect(ui->comboBox_imageoptim_integration, SIGNAL(currentIndexChanged(int)),
          this, SLOT(preference_changed()));
  connect(ui->lineEdit_imageoptim_location, SIGNAL(textChanged(QString)), this,
          SLOT(preference_changed()));
  connect(ui->comboBox_pngquant_path, SIGNAL(editTextChanged(QString)), this,
          SLOT(preference_changed()));
  connect(ui->checkBox_force_execute, SIGNAL(toggled(bool)), this,
          SLOT(preference_changed()));
  connect(ui->spinBox_timeout, SIGNAL(valueChanged(int)), this,
          SLOT(preference_changed()));
  connect(ui->checkBox_save_compress_options, SIGNAL(toggled(bool)), this,
          SLOT(preference_changed()));
  connect(ui->checkBox_save_output_options, SIGNAL(toggled(bool)), this,
          SLOT(preference_changed()));
  connect(ui->comboBox_language, SIGNAL(currentIndexChanged(int)), this,
          SLOT(preference_changed()));
}

PngyuPreferencesDialog::~PngyuPreferencesDialog() { delete ui; }

bool PngyuPreferencesDialog::is_apply_button_enabled() const {
  return ui->pushButton_apply->isEnabled();
}

void PngyuPreferencesDialog::set_apply_button_enabled(bool b) {
  ui->pushButton_apply->setEnabled(b);
}

void PngyuPreferencesDialog::set_n_jobs(const int n) {
  ui->spinBox_n_jobs->setValue(n);
}

int PngyuPreferencesDialog::n_jobs() const {
  return ui->spinBox_n_jobs->value();
}

void PngyuPreferencesDialog::set_image_optim_integrate_mode(
    const pngyu::ImageOptimIntegration mode) {
  QComboBox* combobox = ui->comboBox_imageoptim_integration;
  switch (mode) {
    case pngyu::IMAGEOPTIM_ALWAYS_ENABLED: {
      combobox->setCurrentIndex(1);
      break;
    }
    case pngyu::IMAGEOPTIM_ALWAYS_DISABLED: {
      combobox->setCurrentIndex(2);
      break;
    }
    default: {
      combobox->setCurrentIndex(0);
    }
  }
}

pngyu::ImageOptimIntegration
PngyuPreferencesDialog::image_optim_integrate_mode() {
  switch (ui->comboBox_imageoptim_integration->currentIndex()) {
    case 1: {
      return pngyu::IMAGEOPTIM_ALWAYS_ENABLED;
    }
    case 2: {
      return pngyu::IMAGEOPTIM_ALWAYS_DISABLED;
    }
    default: {
      return pngyu::IMAGEOPTIM_ASK_EVERY_TIME;
    }
  }
  return pngyu::IMAGEOPTIM_ASK_EVERY_TIME;
}

void PngyuPreferencesDialog::set_pngquant_paths(const QStringList& paths) {
  ui->comboBox_pngquant_path->clear();
  for (const QString& path : paths) {
    ui->comboBox_pngquant_path->addItem(pngyu::util::to_dot_path(path));
  }
}

void PngyuPreferencesDialog::set_pngquant_path(const QString& path) {
  ui->comboBox_pngquant_path->insertItem(0, pngyu::util::to_dot_path(path));
  ui->comboBox_pngquant_path->setCurrentIndex(0);
}

QString PngyuPreferencesDialog::pngquant_path() const {
  return pngyu::util::from_dot_path(ui->comboBox_pngquant_path->currentText());
}

void PngyuPreferencesDialog::set_image_optim_path(const QString& path) {
  ui->lineEdit_imageoptim_location->setText(path);
}

QString PngyuPreferencesDialog::image_optim_path() const {
  return ui->lineEdit_imageoptim_location->text();
}

void PngyuPreferencesDialog::set_force_execute_if_negative_enabled(
    const bool enabled) {
  ui->checkBox_force_execute->setChecked(enabled);
}

bool PngyuPreferencesDialog::is_force_execute_if_negative_enabled() const {
  return ui->checkBox_force_execute->isChecked();
}

void PngyuPreferencesDialog::cancel_pushed() { reject(); }

void PngyuPreferencesDialog::ok_pushed() { accept(); }

void PngyuPreferencesDialog::close_pushed() { reject(); }

void PngyuPreferencesDialog::apply_pushed() {
  emit apply_pushed_signal();
  set_apply_button_enabled(false);
}

void PngyuPreferencesDialog::preference_changed() {
  set_apply_button_enabled(true);
}

void PngyuPreferencesDialog::pngquant_location_changed() {
  QComboBox* combo_box = ui->comboBox_pngquant_path;
  const QString& path = pngyu::util::from_dot_path(combo_box->currentText());

  QPalette palette = combo_box->palette();
  if (pngyu::is_executable_pnqguant(QFileInfo(path))) {
    palette.setBrush(QPalette::Text, QBrush());
    combo_box->setToolTip(pngyu::pngquant_version(path));
  } else {
    combo_box->setToolTip("");
    palette.setBrush(QPalette::Text, QBrush(Qt::red));
  }
  combo_box->setPalette(palette);
}

void PngyuPreferencesDialog::open_pngquant_location_pushed() {
  const QString default_dir = "/Applications";

  const QString path =
      QFileDialog::getOpenFileName(this, QString(), default_dir);

  if (!path.isEmpty()) {
    ui->comboBox_pngquant_path->insertItem(0, path);
    ui->comboBox_pngquant_path->setCurrentIndex(0);
  }
}

void PngyuPreferencesDialog::image_optim_location_changed() {
  QLineEdit* const line_edit = ui->lineEdit_imageoptim_location;
  QPalette palette = line_edit->palette();
  if (QFile::exists(image_optim_path())) {
    palette.setBrush(QPalette::Text, QBrush());
  } else {  // if imageoptim path is invalid, change text color
    palette.setBrush(QPalette::Text, QBrush(Qt::red));
  }
  line_edit->setPalette(palette);
}

void PngyuPreferencesDialog::open_image_optim_location_pushed() {
  const QString default_dir = "/Applications";

  const QString path =
      QFileDialog::getOpenFileName(this, QString(), default_dir);

  if (!path.isEmpty()) {
    set_image_optim_path(path);
  }
}

void PngyuPreferencesDialog::set_timeout_ms(const int timeout_ms) {
  ui->spinBox_timeout->setValue(timeout_ms);
}

int PngyuPreferencesDialog::timeout_ms() const {
  return ui->spinBox_timeout->value();
}

void PngyuPreferencesDialog::set_save_compress_options_enabled(
    const bool enable) {
  ui->checkBox_save_compress_options->setChecked(enable);
}

bool PngyuPreferencesDialog::is_save_compress_options_enabled() const {
  return ui->checkBox_save_compress_options->isChecked();
}

void PngyuPreferencesDialog::set_save_output_options_enabled(
    const bool enable) {
  ui->checkBox_save_output_options->setChecked(enable);
}

bool PngyuPreferencesDialog::is_save_output_options_enabled() const {
  return ui->checkBox_save_output_options->isChecked();
}

void PngyuPreferencesDialog::set_language(const QString& language) {
  m_initial_language = language;  // Save initial value for change detection
  if (language == "ja") {
    ui->comboBox_language->setCurrentIndex(2);
  } else if (language == "zh") {
    ui->comboBox_language->setCurrentIndex(3);
  } else if (language == "en") {
    ui->comboBox_language->setCurrentIndex(1);
  } else {
    ui->comboBox_language->setCurrentIndex(0);  // System default
  }
}

QString PngyuPreferencesDialog::language() const {
  switch (ui->comboBox_language->currentIndex()) {
    case 1:
      return "en";
    case 2:
      return "ja";
    case 3:
      return "zh";
    default:
      return "auto";  // System default
  }
}

bool PngyuPreferencesDialog::is_language_changed() const {
  return language() != m_initial_language;
}
