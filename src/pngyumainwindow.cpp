#include "pngyumainwindow.h"
#include "ui_pngyumainwindow.h"

#include <cmath>

#include <QDebug>
#include <QDialogButtonBox>
#include <QProcess>
#include <QTime>

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include <QMoveEvent>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QScreen>
#include <QSettings>
#include <QUrl>

#include <QImageReader>
#include <QMessageBox>

#include "pngyuimageoptimintegrationquestiondialog.h"
#include "pngyupreferencesdialog.h"
#include "pngyupreviewwindow.h"

#include "executecompressworkerthread.h"
#include "pngyu_custom_tablewidget_item.h"
#include "pngyu_execute_pngquant_command.h"
#include "pngyu_util.h"

namespace {
const QString DEFAULT_IMAGE_OPTIM_PATH = "/Applications/ImageOptim.app";

} // namespace

PngyuMainWindow::PngyuMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PngyuMainWindow),
      m_preview_window(new PngyuPreviewWindow(this)),
      m_preferences_dialog(new PngyuPreferencesDialog(this)),
      m_current_pngquant_path(),
      m_current_imageoptim_path(DEFAULT_IMAGE_OPTIM_PATH),
      m_stop_request(false), m_is_busy(false),
      m_temporary_custom_output_custom_on(false), m_num_thread(1),
      m_image_optim_integration(pngyu::IMAGEOPTIM_ASK_EVERY_TIME),
      m_force_execute_if_negative_enables(false), m_timeout_ms(20000),
      m_save_compress_options_enabled(false),
      m_save_output_options_enabled(false) {
  ui->setupUi(this);

  setWindowTitle(windowTitle() + QString(" %1.%2.%3")
                                     .arg(pngyu::VERSION_MAJOR)
                                     .arg(pngyu::VERSION_MINOR)
                                     .arg(pngyu::VERSION_REVISION));

  pngyu::util::success_icon();
  pngyu::util::failure_icon();

  ui->mainToolBar->setVisible(false);

  // Enable drag and drop
  setAcceptDrops(true);

  { // init file list table widget
    QTableWidget *table_widget = file_list_table_widget();
    table_widget->setColumnCount(pngyu::TABLE_COLUMN_COUNT);
    table_widget->setHorizontalHeaderItem(
        pngyu::COLUMN_BASENAME, new QTableWidgetItem(tr("File Name")));
    table_widget->setHorizontalHeaderItem(pngyu::COLUMN_ABSOLUTE_PATH,
                                          new QTableWidgetItem(tr("Path")));
    table_widget->setHorizontalHeaderItem(pngyu::COLUMN_RESULT,
                                          new QTableWidgetItem(tr("Result")));
    table_widget->setHorizontalHeaderItem(pngyu::COLUMN_ORIGINAL_SIZE,
                                          new QTableWidgetItem(tr("Size")));
    table_widget->setHorizontalHeaderItem(
        pngyu::COLUMN_OUTPUT_SIZE, new QTableWidgetItem(tr("Compressed Size")));
    table_widget->setHorizontalHeaderItem(
        pngyu::COLUMN_SAVED_SIZE, new QTableWidgetItem(tr("Saved Size")));
    table_widget->setHorizontalHeaderItem(
        pngyu::COLUMN_SAVED_SIZE_RATE,
        new QTableWidgetItem(tr("Saved Size(%)")));

    pngyu::util::set_drop_here_stylesheet(table_widget->viewport(), false);
  }

  { // init homepage label layout
    QWidget *const spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    ui->statusBar->addWidget(spacer, 5);
    ui->statusBar->addWidget(ui->label_homepage);
  }

  ui->widget_executing->setVisible(false);
  ui->widget_file_appending->setVisible(false);
  ui->pushButton_stop_exec->setVisible(false);

  connect(ui->spinBox_colors, SIGNAL(valueChanged(int)), this,
          SLOT(ncolor_spinbox_changed()));
  connect(ui->horizontalSlider_colors, SIGNAL(valueChanged(int)), this,
          SLOT(ncolor_slider_changed()));

  connect(ui->pushButton_exec, SIGNAL(clicked()), this, SLOT(exec_pushed()));

  connect(ui->lineEdit_output_directory, SIGNAL(textChanged(QString)), this,
          SLOT(other_output_directory_changed()));

  connect(ui->toolButton_open_output_directory, SIGNAL(clicked()), this,
          SLOT(open_output_directory_pushed()));

  connect(ui->radioButton_output_same_directory, SIGNAL(toggled(bool)), this,
          SLOT(output_directory_mode_changed()));
  //  connect( ui->radioButton_output_other_directory, SIGNAL(toggled(bool)),
  //           this, SLOT(output_directory_mode_changed()) );

  connect(ui->toolButton_compress_option_default, SIGNAL(toggled(bool)), this,
          SLOT(compress_option_mode_changed()));

  connect(ui->toolButton_output_option_overwrite_original,
          SIGNAL(toggled(bool)), this, SLOT(output_option_mode_changed()));

  connect(ui->comboBox_output_filename_mode, SIGNAL(currentIndexChanged(int)),
          this, SLOT(output_filename_mode_changed()));

  connect(ui->pushButton_filelist_clear, SIGNAL(clicked()), this,
          SLOT(file_list_clear_pushed()));

  connect(ui->tableWidget_filelist,
          SIGNAL(currentCellChanged(int, int, int, int)), this,
          SLOT(table_widget_current_changed()));

  connect(ui->pushButton_preview, SIGNAL(toggled(bool)), this,
          SLOT(preview_button_toggled(bool)));

  connect(m_preview_window, SIGNAL(closed()), this,
          SLOT(preview_window_closed()));

  // connect compress option changed slot
  connect(ui->spinBox_colors, SIGNAL(valueChanged(int)), this,
          SLOT(compress_option_changed()));
  connect(ui->horizontalSlider_compress_speed, SIGNAL(valueChanged(int)), this,
          SLOT(compress_option_changed()));
  connect(ui->checkBox_ie6_support, SIGNAL(stateChanged(int)), this,
          SLOT(compress_option_changed()));
  connect(ui->checkBox_dithered, SIGNAL(stateChanged(int)), this,
          SLOT(compress_option_changed()));
  connect(ui->toolButton_compress_option_default, SIGNAL(toggled(bool)), this,
          SLOT(compress_option_changed()));
  ///

  connect(ui->pushButton_stop_exec, SIGNAL(clicked()), this,
          SLOT(stop_request()));

  connect(ui->pushButton_stop_file_appending, SIGNAL(clicked()), this,
          SLOT(stop_request()));

  connect(ui->toolButton_add_file, SIGNAL(clicked()), this,
          SLOT(add_file_button_pushed()));

  connect(ui->action_preferences, SIGNAL(triggered()), this,
          SLOT(menu_preferences_pushed()));
  connect(ui->action_quit, SIGNAL(triggered()), this, SLOT(menu_quit_pushed()));

  // connect preference dialogs
  connect(m_preferences_dialog, SIGNAL(apply_pushed_signal()), this,
          SLOT(preferences_apply_pushed()));

  output_directory_mode_changed();
  output_filename_mode_changed();
  compress_option_mode_changed();
  output_option_mode_changed();
  compress_option_changed();
  table_widget_current_changed();

  update_file_table();

  { // set window size
    const QRect screen_geometry = QGuiApplication::primaryScreen()->geometry();
    const QPoint center_pos = screen_geometry.center();
    const QSize window_size(500, 400);
    setGeometry(QRect(
        center_pos - QPoint(window_size.width() / 2, window_size.height() / 2),
        window_size));
  }

  { // find executable pngquant
    const QStringList &executable_pngquant_list =
        pngyu::find_executable_pngquant();
    if (executable_pngquant_list.empty()) {
      set_busy_mode(true);
      ui->statusBar->showMessage(tr("Executable pngquant not found"));
    } else {
      set_executable_pngquant_path(executable_pngquant_list.first());
    }
  }

  read_settings();
}

PngyuMainWindow::~PngyuMainWindow() {
  write_settings();

  delete ui;
}

QString PngyuMainWindow::get_settings_file_path() const {
  // make application ini file path
  const QString &app_path = QApplication::applicationFilePath();
  QString fn = app_path;
  fn.chop(QFileInfo(app_path).suffix().length());
  fn = fn + (fn.endsWith(".") ? "" : ".") + QString("ini");
  return fn;
}

void PngyuMainWindow::read_settings() {
  QSettings settings(get_settings_file_path(), QSettings::IniFormat);
  settings.beginGroup("options");
  const int imageoptim =
      settings.value("imageoptim_integration", pngyu::IMAGEOPTIM_ASK_EVERY_TIME)
          .toInt();
  set_image_optim_integration_mode(
      static_cast<pngyu::ImageOptimIntegration>(imageoptim));

  const QString pngquant_path = pngyu::util::from_dot_path(
      settings.value("pngquant_path", QString()).toString());
  if (pngyu::is_executable_pnqguant(QFileInfo(pngquant_path))) {
    set_executable_pngquant_path(pngquant_path);
  }

  const QString imageoptim_path =
      settings.value("imageoptim_path", QString(DEFAULT_IMAGE_OPTIM_PATH))
          .toString();
  if (QFile::exists(imageoptim_path)) {
    set_executable_image_optim_path(imageoptim_path);
  }

  const int n_thread = settings.value("num_thread", 1).toInt();
  set_num_thread(n_thread);

  const bool force_execute_if_negative =
      settings.value("force_execute_if_negative", false).toBool();
  set_force_execute_if_negative_enabled(force_execute_if_negative);

  const int timeout_ms = settings.value("timeout_ms", 20000).toInt();
  set_timeout_ms(timeout_ms);

  const bool save_compress_options =
      settings.value("save_compress_options", false).toBool();
  set_save_compress_options_enabled(save_compress_options);

  const bool save_output_options =
      settings.value("save_output_options", false).toBool();
  set_save_output_options_enabled(save_output_options);

  settings.endGroup();

  // Load last used options if enabled
  load_last_used_options();
}

void PngyuMainWindow::load_last_used_options() {
  QSettings settings(get_settings_file_path(), QSettings::IniFormat);

  // Load compress options if enabled
  if (is_save_compress_options_enabled()) {
    settings.beginGroup("last_used_compress_options");

    const int compress_mode =
        settings.value("compress_mode", pngyu::COMPRESS_OPTION_DEFAULT).toInt();
    set_current_compress_option_mode(
        static_cast<pngyu::CompressOptionMode>(compress_mode));

    if (compress_mode == pngyu::COMPRESS_OPTION_CUSTOM) {
      const int ncolor = settings.value("ncolor", 256).toInt();
      set_ncolor(ncolor);

      const int speed = settings.value("compress_speed", 3).toInt();
      set_compress_speed(speed);

      const bool dither = settings.value("dither_enabled", true).toBool();
      set_dither_enabled(dither);

      const bool ie6_support =
          settings.value("ie6_support_enabled", false).toBool();
      set_ie6_alpha_support_enabled(ie6_support);
    }

    settings.endGroup();
  }

  // Load output options if enabled
  if (is_save_output_options_enabled()) {
    settings.beginGroup("last_used_output_options");

    const int output_mode =
        settings.value("output_mode", pngyu::OUTPUT_OPTION_CUSTOM).toInt();
    set_current_output_option_mode(
        static_cast<pngyu::OutputOptionMode>(output_mode));

    const int output_dir_mode =
        settings.value("output_directory_mode", pngyu::OUTPUT_DIR_SAME).toInt();
    set_current_output_directory_mode(
        static_cast<pngyu::OuputDirectoryMode>(output_dir_mode));

    const int output_filename_mode =
        settings
            .value("output_filename_mode", pngyu::OUTPUT_FILE_SAME_AS_ORIGINAL)
            .toInt();
    set_current_outoput_filename_mode(
        static_cast<pngyu::OutputFinenameMode>(output_filename_mode));

    const QString other_dir =
        settings.value("other_output_directory", QString()).toString();
    if (!other_dir.isEmpty()) {
      set_other_output_directory(other_dir);
    }

    const QString prefix =
        settings.value("output_filename_prefix", QString()).toString();
    set_output_filename_prefix(prefix);

    const QString suffix =
        settings.value("output_filename_suffix", QString()).toString();
    set_output_filename_suffix(suffix);

    const bool overwrite =
        settings.value("file_overwrite_enabled", false).toBool();
    set_file_overwrite_enabled(overwrite);

    settings.endGroup();
  }
}

void PngyuMainWindow::write_settings() {
  QSettings settings(get_settings_file_path(), QSettings::IniFormat);

  settings.beginGroup("options");

  settings.setValue("pngquant_path",
                    pngyu::util::to_dot_path(executable_pngquant_path()));

  settings.setValue("imageoptim_integration", image_optim_integration_mode());

  settings.setValue("imageoptim_path", executable_image_optim_path());

  settings.setValue("num_thread", num_thread());

  settings.setValue("force_execute_if_negative",
                    is_force_execute_if_negative_enabled());

  settings.setValue("timeout_ms", timeout_ms());

  settings.setValue("save_compress_options",
                    is_save_compress_options_enabled());
  settings.setValue("save_output_options", is_save_output_options_enabled());

  settings.endGroup();
}

void PngyuMainWindow::save_last_used_options() {
  QSettings settings(get_settings_file_path(), QSettings::IniFormat);

  // Save compress options if enabled
  if (is_save_compress_options_enabled()) {
    settings.beginGroup("last_used_compress_options");

    settings.setValue("compress_mode", current_compress_option_mode());
    settings.setValue("ncolor", ncolor());
    settings.setValue("compress_speed", compress_speed());
    settings.setValue("dither_enabled", dither_enabled());
    settings.setValue("ie6_support_enabled", ie6_alpha_support_enabled());

    settings.endGroup();
  }

  // Save output options if enabled
  if (is_save_output_options_enabled()) {
    settings.beginGroup("last_used_output_options");

    settings.setValue("output_mode", current_output_option_mode());
    settings.setValue("output_directory_mode", current_output_directory_mode());
    settings.setValue("output_filename_mode", current_output_filename_mode());
    settings.setValue("other_output_directory", other_output_directory());
    settings.setValue("output_filename_prefix", output_filename_prefix());
    settings.setValue("output_filename_suffix", output_filename_suffix());
    settings.setValue("file_overwrite_enabled", file_overwrite_enabled());

    settings.endGroup();
  }
}

QTableWidget *PngyuMainWindow::file_list_table_widget() {
  return ui->tableWidget_filelist;
}

void PngyuMainWindow::file_list_clear() {
  m_file_list.clear();
  update_file_table();
}

QString PngyuMainWindow::make_output_file_path_string(
    const QString &input_file_path) const {
  // if current option is invalid option
  // this function returns empty string

  const QFileInfo input_file_info(input_file_path);

  const pngyu::OutputOptionMode &output_option_mode =
      current_output_option_mode();

  if (output_option_mode == pngyu::OUTPUT_OPTION_OVERWITE_ORIGINAL) {
    return input_file_info.absoluteFilePath();
  } else if (output_option_mode == pngyu::OUTPUT_OPTION_CUSTOM) {
    QString output_dir_path;
    { // meke output dir path string
      const pngyu::OuputDirectoryMode output_dir_mode =
          current_output_directory_mode();
      const bool b_valid_dir = is_other_output_directory_valid();
      if (output_dir_mode == pngyu::OUTPUT_DIR_SAME) {
        output_dir_path = input_file_info.absolutePath();
        if (!QFileInfo(output_dir_path).isRoot()) {
          output_dir_path += "/";
        }
      } else if (output_dir_mode == pngyu::OUTPUT_DIR_OTHER && b_valid_dir) {
        output_dir_path = other_output_directory();
        if (!QFileInfo(output_dir_path).isRoot()) {
          output_dir_path += "/";
        }
      } else {
        return QString();
      }
    }

    QString output_file_name;
    { // make output file name string
      const pngyu::OutputFinenameMode output_filename_mode =
          current_output_filename_mode();
      if (output_filename_mode == pngyu::OUTPUT_FILE_SAME_AS_ORIGINAL) {
        output_file_name = input_file_info.fileName();
      } else if (output_filename_mode == pngyu::OUTPUT_FILE_CUSTOM) {
        const QString &prefix = output_filename_prefix();
        const QString &suffix = output_filename_suffix();

        const QString &base_name = input_file_info.completeBaseName();
        output_file_name = prefix + base_name + suffix;
      } else {
        return QString();
      }
    }

    return output_dir_path + output_file_name;
  }

  return QString();
}

pngyu::PngquantOption
PngyuMainWindow::make_pngquant_option(const QString &output_file_suffix) const {
  // make pngquant option from current compress option

  pngyu::PngquantOption option;
  if (current_compress_option_mode() == pngyu::COMPRESS_OPTION_CUSTOM) {
    option.set_ncolors(ncolor());
    if (!output_file_suffix.isEmpty()) {
      option.set_out_suffix(output_file_suffix);
    }
    // option.set_force_overwrite( true );
    option.set_speed(compress_speed());
    option.set_floyd_steinberg_dithering_disabled(!dither_enabled());
    option.set_ie6_alpha_support(ie6_alpha_support_enabled());
  }
  option.set_timeout_ms(timeout_ms());
  return option;
}

void PngyuMainWindow::set_executable_pngquant_path(const QString &path) {
  m_current_pngquant_path = path;
  m_preview_window->set_executable_pngquant_path(path);
}

QString PngyuMainWindow::executable_pngquant_path() const {
  return m_current_pngquant_path;
}

void PngyuMainWindow::set_executable_image_optim_path(const QString &path) {
  m_current_imageoptim_path = path;
}

QString PngyuMainWindow::executable_image_optim_path() const {
  return m_current_imageoptim_path;
}

void PngyuMainWindow::set_current_compress_option_mode(
    const pngyu::CompressOptionMode mode) {
  if (mode == pngyu::COMPRESS_OPTION_DEFAULT) {
    ui->toolButton_compress_option_default->setChecked(true);
  } else if (mode == pngyu::COMPRESS_OPTION_CUSTOM) {
    ui->toolButton_compress_option_custom->setChecked(true);
  }
}

pngyu::CompressOptionMode
PngyuMainWindow::current_compress_option_mode() const {
  const bool default_checked =
      ui->toolButton_compress_option_default->isChecked();
  const bool custom_checked =
      ui->toolButton_compress_option_custom->isChecked();
  if (default_checked && !custom_checked) {
    return pngyu::COMPRESS_OPTION_DEFAULT;
  } else if (!default_checked && custom_checked) {
    return pngyu::COMPRESS_OPTION_CUSTOM;
  }
  return pngyu::COMPRESS_OPTION_UNKNOWN;
}

void PngyuMainWindow::set_current_output_option_mode(
    const pngyu::OutputOptionMode mode) {
  if (mode == pngyu::OUTPUT_OPTION_OVERWITE_ORIGINAL) {
    ui->toolButton_output_option_overwrite_original->setChecked(true);
  } else if (mode == pngyu::OUTPUT_OPTION_CUSTOM) {
    ui->toolButton_output_option_custom->setChecked(true);
  }
}

pngyu::OutputOptionMode PngyuMainWindow::current_output_option_mode() const {
  const bool overwrite_origin_checked =
      ui->toolButton_output_option_overwrite_original->isChecked();
  const bool custom_checked = ui->toolButton_output_option_custom->isChecked();
  if (overwrite_origin_checked && !custom_checked) {
    return pngyu::OUTPUT_OPTION_OVERWITE_ORIGINAL;
  } else if (!overwrite_origin_checked && custom_checked) {
    return pngyu::OUTPUT_OPTION_CUSTOM;
  }
  return pngyu::OUTPUT_OPTION_UNKNOWN;
}

void PngyuMainWindow::set_current_output_directory_mode(
    const pngyu::OuputDirectoryMode mode) {
  if (mode == pngyu::OUTPUT_DIR_SAME) {
    ui->radioButton_output_same_directory->setChecked(true);
  } else if (mode == pngyu::OUTPUT_DIR_OTHER) {
    ui->radioButton_output_other_directory->setChecked(true);
  }
}

pngyu::OuputDirectoryMode
PngyuMainWindow::current_output_directory_mode() const {
  const bool same_checked = ui->radioButton_output_same_directory->isChecked();
  const bool other_checked =
      ui->radioButton_output_other_directory->isChecked();
  if (same_checked && !other_checked) {
    return pngyu::OUTPUT_DIR_SAME;
  } else if (!same_checked && other_checked) {
    return pngyu::OUTPUT_DIR_OTHER;
  }
  return pngyu::OUTPUT_DIR_UNKNOWN;
}

void PngyuMainWindow::set_current_outoput_filename_mode(
    const pngyu::OutputFinenameMode mode) {
  if (mode == pngyu::OUTPUT_FILE_SAME_AS_ORIGINAL) {
    ui->comboBox_output_filename_mode->setCurrentIndex(0);
  } else if (mode == pngyu::OUTPUT_FILE_CUSTOM) {
    ui->comboBox_output_filename_mode->setCurrentIndex(1);
  }
}

pngyu::OutputFinenameMode
PngyuMainWindow::current_output_filename_mode() const {
  const int index = ui->comboBox_output_filename_mode->currentIndex();
  if (index == 0) {
    return pngyu::OUTPUT_FILE_SAME_AS_ORIGINAL;
  } else if (index == 1) {
    return pngyu::OUTPUT_FILE_CUSTOM;
  }
  return pngyu::OUTPUT_FILE_UNKNOWN;
}

void PngyuMainWindow::set_other_output_directory(
    const QString &output_directory) {
  ui->lineEdit_output_directory->setText(output_directory);
}

QString PngyuMainWindow::other_output_directory() const {
  const QString &inputted_dir_path = ui->lineEdit_output_directory->text();
  if (inputted_dir_path.isEmpty()) {
    return QString();
  }
  return QDir(inputted_dir_path).absolutePath();
}

void PngyuMainWindow::set_output_filename_prefix(const QString &prefix) {
  ui->lineEdit_output_file_prefix->setText(prefix);
}

QString PngyuMainWindow::output_filename_prefix() const {
  return ui->lineEdit_output_file_prefix->text();
}

void PngyuMainWindow::set_output_filename_suffix(const QString &suffix) {
  ui->lineEdit_output_file_suffix->setText(suffix);
}

QString PngyuMainWindow::output_filename_suffix() const {
  return ui->lineEdit_output_file_suffix->text();
}

void PngyuMainWindow::set_ncolor(const int n) {
  ui->spinBox_colors->setValue(n);
}

int PngyuMainWindow::ncolor() const { return ui->spinBox_colors->value(); }

void PngyuMainWindow::set_file_overwrite_enabled(const bool b) {
  ui->checkBox_overwrite->setChecked(b);
}

bool PngyuMainWindow::file_overwrite_enabled() const {
  return ui->checkBox_overwrite->isChecked();
}

void PngyuMainWindow::set_dither_enabled(const bool b) {
  ui->checkBox_dithered->setChecked(b);
}

bool PngyuMainWindow::dither_enabled() const {
  return ui->checkBox_dithered->isChecked();
}

void PngyuMainWindow::set_compress_speed(const int speed) {
  ui->horizontalSlider_compress_speed->setValue(speed);
}

void PngyuMainWindow::set_ie6_alpha_support_enabled(const bool b) {
  ui->checkBox_ie6_support->setChecked(b);
}

bool PngyuMainWindow::ie6_alpha_support_enabled() const {
  return ui->checkBox_ie6_support->isChecked();
}

int PngyuMainWindow::compress_speed() const {
  return ui->horizontalSlider_compress_speed->value();
}

void PngyuMainWindow::set_image_optim_integration_mode(
    const pngyu::ImageOptimIntegration mode) {
#ifdef Q_OS_MACOS
  m_image_optim_integration = mode;
#else
  m_image_optim_integration = pngyu::IMAGEOPTIM_ALWAYS_DISABLED;
#endif
}

pngyu::ImageOptimIntegration
PngyuMainWindow::image_optim_integration_mode() const {
#ifdef Q_OS_MACOS
  return m_image_optim_integration;
#else
  return pngyu::IMAGEOPTIM_ALWAYS_DISABLED;
#endif
}

void PngyuMainWindow::set_num_thread(const int num) {
  m_num_thread = std::min(8, std::max(1, num));
}

int PngyuMainWindow::num_thread() const {
  return std::min(8, std::max(1, m_num_thread));
  ;
}

void PngyuMainWindow::set_force_execute_if_negative_enabled(const bool b) {
  m_force_execute_if_negative_enables = b;
}

bool PngyuMainWindow::is_force_execute_if_negative_enabled() const {
  return m_force_execute_if_negative_enables;
}

void PngyuMainWindow::set_timeout_ms(const int timeout_ms) {
  m_timeout_ms = timeout_ms;
}

int PngyuMainWindow::timeout_ms() const { return m_timeout_ms; }

void PngyuMainWindow::set_save_compress_options_enabled(const bool b) {
  m_save_compress_options_enabled = b;
}

bool PngyuMainWindow::is_save_compress_options_enabled() const {
  return m_save_compress_options_enabled;
}

void PngyuMainWindow::set_save_output_options_enabled(const bool b) {
  m_save_output_options_enabled = b;
}

bool PngyuMainWindow::is_save_output_options_enabled() const {
  return m_save_output_options_enabled;
}

void PngyuMainWindow::execute_compress_all(bool image_optim_enabled) {
  if (is_busy()) {
    return;
  }

  // disable ui operation temporary
  set_busy_mode(true);

  ui->widget_executing->setVisible(true);
  ui->pushButton_exec->setVisible(false);
  ui->pushButton_stop_exec->setVisible(true);

  clear_compress_result();

  const bool b_overwrite_enable = file_overwrite_enabled();
  const bool b_force_execute_if_negative =
      is_force_execute_if_negative_enabled();

  const pngyu::PngquantOption &option = make_pngquant_option(QString());
  const QString &pngquant_path = executable_pngquant_path();

  QQueue<pngyu::CompressQueueData> queue;

  QTableWidget *table_widget = file_list_table_widget();
  const int row_count = table_widget->rowCount();
  for (int row = 0; row < row_count; ++row) { // file list loop

    const QTableWidgetItem *const absolute_path_item =
        table_widget->item(row, pngyu::COLUMN_ABSOLUTE_PATH);
    if (!absolute_path_item) {
      qWarning() << "Item is null. row: " << row;
      continue;
    }

    const QString &src_path = absolute_path_item->text();
    const QString &dst_path = make_output_file_path_string(src_path);

    pngyu::CompressQueueData data;
    data.src_path = src_path;
    data.dst_path = dst_path;
    data.pngquant_path = pngquant_path;
    data.pngquant_option = option;
    data.overwrite_enabled = b_overwrite_enable;
    data.force_execute_if_negative = b_force_execute_if_negative;
    data.table_widget = table_widget;
    data.table_row = row;

    queue.enqueue(data);

  } // end of file list loop

  const int total_file_count = queue.size();

  update_execution_progress(0, total_file_count);

  const int n_thread = num_thread();

  QVector<ExecuteCompressWorkerThread *> workers;

  QMutex mutex;
  for (int i = 0; i < n_thread; ++i) {
    ExecuteCompressWorkerThread *worker = new ExecuteCompressWorkerThread();

    worker->set_queue_ptr(&queue, &mutex);
    workers.push_back(worker);
  }

  QApplication::processEvents();

  foreach (ExecuteCompressWorkerThread *worker, workers) {
    worker->start(QThread::HighPriority);
  }

  QApplication::processEvents();

  while (true) {
    bool all_finished = true;
    foreach (ExecuteCompressWorkerThread *worker, workers) {
      ui->spinner_exec->redraw();
      ui->spinner_exec->update();
      QApplication::processEvents();

      if (!worker->wait(30)) {
        all_finished = false;
        break;
      }
    }
    if (all_finished) {
      break;
    }

    // send stop request to workers
    if (m_stop_request) {
      foreach (ExecuteCompressWorkerThread *worker, workers) {
        worker->stop_request();
      }
    }

    int completed_count = 0;
    foreach (ExecuteCompressWorkerThread *worker, workers) {
      completed_count += worker->compress_results().size();
    }

    update_execution_progress(completed_count, total_file_count);
  }

  QList<pngyu::CompressResult> result_list;

  foreach (ExecuteCompressWorkerThread *worker, workers) {
    result_list += worker->compress_results();
  }

  foreach (ExecuteCompressWorkerThread *worker, workers) {
    delete worker;
  }

  workers.clear();

  // result
  qint64 total_saved_size = 0;
  QStringList succeed_src_filepaths;
  QStringList succeed_dst_filepaths;

  foreach (const pngyu::CompressResult &res, result_list) {
    if (res.result) {
      succeed_src_filepaths.push_back(res.src_path);
      succeed_dst_filepaths.push_back(res.dst_path);
      total_saved_size += res.src_size - res.dst_size;
    }
  }

  { // show result status bar
    const QString size_string =
        total_saved_size > 500 * 1024
            ? pngyu::util::size_to_string_mb(total_saved_size)
            : pngyu::util::size_to_string_kb(total_saved_size);
    ui->statusBar->showMessage(tr("Total %1 saved").arg(size_string));
  }

  if (image_optim_enabled) {
    pngyu::util::open_with_mac_app(succeed_dst_filepaths,
                                   executable_image_optim_path());
  }

  set_busy_mode(false);

  ui->widget_executing->setVisible(false);
  ui->pushButton_stop_exec->setVisible(false);
  ui->pushButton_exec->setVisible(true);
}

bool PngyuMainWindow::is_preview_window_visible() const {
  return m_preview_window->isVisible();
}

QString PngyuMainWindow::current_selected_filename() const {
  QTableWidget *const table_widget = ui->tableWidget_filelist;
  const int current_row = table_widget->currentRow();
  const QTableWidgetItem *const path_item =
      table_widget->item(current_row, pngyu::COLUMN_ABSOLUTE_PATH);
  return path_item ? path_item->text() : QString();
}

void PngyuMainWindow::set_busy_mode(const bool b) {
  ui->tableWidget_filelist->setSortingEnabled(!b);
  if (b) {
    ui->tableWidget_filelist->horizontalHeader()->setSortIndicator(
        -1, Qt::AscendingOrder);
  }

  const bool b_has_files = ui->tableWidget_filelist->rowCount() > 0;
  ui->pushButton_exec->setEnabled((!b) && b_has_files);
  ui->groupBox_compress_option->setEnabled(!b);
  ui->groupBox_output_option->setEnabled(!b);
  ui->pushButton_filelist_clear->setEnabled((!b) && b_has_files);
  ui->toolButton_add_file->setEnabled(!b);
  m_stop_request = false;
  ui->pushButton_stop_exec->setEnabled(true);
  m_is_busy = b;
}

bool PngyuMainWindow::is_busy() const { return m_is_busy; }

void PngyuMainWindow::clear_compress_result() {
  QTableWidget *table_widget = file_list_table_widget();
  const int row_count = table_widget->rowCount();
  for (int row = 0; row < row_count; ++row) {
    ui->tableWidget_filelist->setItem(row, pngyu::COLUMN_RESULT, 0);
    ui->tableWidget_filelist->setItem(row, pngyu::COLUMN_OUTPUT_SIZE, 0);
    ui->tableWidget_filelist->setItem(row, pngyu::COLUMN_SAVED_SIZE, 0);
  }
  ui->statusBar->showMessage(QString());
}

void PngyuMainWindow::update_execution_progress(const int completed_count,
                                                const int total_count) {
  ui->label_executing->setText(
      tr("Executing...(%1/%2)").arg(completed_count).arg(total_count));
}

//////////////////////
// protected functions
//////////////////////

void PngyuMainWindow::dragEnterEvent(QDragEnterEvent *event) {
  if (is_busy()) {
    return;
  }

  if (event->mimeData()->hasUrls()) {
    event->accept();
  }
}

void PngyuMainWindow::dragLeaveEvent(QDragLeaveEvent *event) {
  Q_UNUSED(event)
  if (is_busy()) {
    return;
  }

  { // disable ui effects that while file dragging
    pngyu::util::set_drop_enabled_palette(ui->tableWidget_filelist->viewport(),
                                          false);
    pngyu::util::set_drop_here_stylesheet(ui->tableWidget_filelist->viewport(),
                                          false);

    if (m_temporary_custom_output_custom_on &&
        !pngyu::util::is_under_mouse(this)) {
      m_temporary_custom_output_custom_on = false;
      set_current_output_option_mode(pngyu::OUTPUT_OPTION_OVERWITE_ORIGINAL);
    }
  }
}

void PngyuMainWindow::dragMoveEvent(QDragMoveEvent *event) {
  Q_UNUSED(event)
  if (is_busy()) {
    return;
  }

  { // enable file drogging ui effects

    { // linedit
      QLineEdit *const output_line_edit = ui->lineEdit_output_directory;
      pngyu::util::set_drop_enabled_palette(
          output_line_edit, pngyu::util::is_under_mouse(output_line_edit));
    }

    QWidget *const file_list_widget = ui->groupBox_filelist;
    pngyu::util::set_drop_here_stylesheet(
        ui->tableWidget_filelist->viewport(),
        pngyu::util::is_under_mouse(file_list_widget));

    // if dragg mouse is on output custom button open custom menu
    if (current_output_option_mode() != pngyu::OUTPUT_OPTION_CUSTOM &&
        pngyu::util::is_under_mouse(ui->toolButton_output_option_custom)) {
      m_temporary_custom_output_custom_on = true;
      set_current_output_option_mode(pngyu::OUTPUT_OPTION_CUSTOM);
    } else if (m_temporary_custom_output_custom_on &&
               !pngyu::util::is_under_mouse(ui->groupBox_output_option)) {
      m_temporary_custom_output_custom_on = false;
      set_current_output_option_mode(pngyu::OUTPUT_OPTION_OVERWITE_ORIGINAL);
    }
  }
}

void PngyuMainWindow::dropEvent(QDropEvent *event) {
  if (is_busy()) {
    return;
  }

  // disable file drogging ui effects (1st

  QLineEdit *const output_line_edit = ui->lineEdit_output_directory;
  const bool mouse_is_on_output = pngyu::util::is_under_mouse(output_line_edit);
  pngyu::util::set_drop_enabled_palette(output_line_edit, false);

  QWidget *const file_list_widget = ui->groupBox_filelist;
  const bool mouse_is_on_file_list =
      pngyu::util::is_under_mouse(file_list_widget);
  pngyu::util::set_drop_here_stylesheet(ui->tableWidget_filelist->viewport(),
                                        false);

  const QMimeData *const mimedata = event->mimeData();
  if (!mimedata->hasUrls()) {
    return;
  }

  const QList<QUrl> &url_list = mimedata->urls();

  if (mouse_is_on_output) { // set output directory path
    if (url_list.size() == 1) {
      set_other_output_directory(url_list.first().toLocalFile());
      set_current_output_directory_mode(pngyu::OUTPUT_DIR_OTHER);
    }
  } else if (mouse_is_on_file_list) { // append png files
    QList<QFileInfo> file_list;
    foreach (const QUrl &url, url_list) {
      file_list.push_back(QFileInfo(url.toLocalFile()));
    }
    append_file_info_list(file_list);
  }

  // disable file drogging ui effect (2nd
  if (!mouse_is_on_output && m_temporary_custom_output_custom_on) {
    m_temporary_custom_output_custom_on = false;
    set_current_output_option_mode(pngyu::OUTPUT_OPTION_OVERWITE_ORIGINAL);
  }
}

void PngyuMainWindow::moveEvent(QMoveEvent *event) {
  QMainWindow::moveEvent(event);
}

void PngyuMainWindow::showEvent(QShowEvent *event) {
  QMainWindow::showEvent(event);
}

void PngyuMainWindow::closeEvent(QCloseEvent *event) {
  stop_request();
  QMainWindow::closeEvent(event);
}

void PngyuMainWindow::update_file_table() {
  QTableWidget *const table_widget = file_list_table_widget();
  // temporary disconnect
  disconnect(table_widget, SIGNAL(currentCellChanged(int, int, int, int)), this,
             SLOT(table_widget_current_changed()));
  const QString &last_selected_filename = current_selected_filename();
  table_widget->setRowCount(0); // reset file list table
  table_widget->setRowCount(m_file_list.size());
  { // append files
    int row_index = 0;
    foreach (const QFileInfo &file_info, m_file_list) {
      QTableWidgetItem *const basename_item =
          new QTableWidgetItem(file_info.baseName());
      basename_item->setToolTip(file_info.baseName());
      table_widget->setItem(row_index, pngyu::COLUMN_BASENAME, basename_item);
      const QString &absolute_path = file_info.absoluteFilePath();
      QTableWidgetItem *const abspath_item =
          new QTableWidgetItem(file_info.absoluteFilePath());
      abspath_item->setToolTip(absolute_path);
      table_widget->setItem(row_index, pngyu::COLUMN_ABSOLUTE_PATH,
                            abspath_item);

      pngyu::TableValueCompareItem *const origin_size_item =
          new pngyu::TableValueCompareItem(
              pngyu::util::size_to_string_kb(file_info.size()));
      origin_size_item->setData(1, static_cast<double>(file_info.size()));
      table_widget->setItem(row_index, pngyu::COLUMN_ORIGINAL_SIZE,
                            origin_size_item);

      if (last_selected_filename == absolute_path) {
        table_widget->setCurrentItem(abspath_item);
      }
      ++row_index;
    }
    ui->pushButton_exec->setEnabled(row_index > 0);
    ui->pushButton_filelist_clear->setEnabled(row_index > 0);
  }
  // reconnect
  connect(table_widget, SIGNAL(currentCellChanged(int, int, int, int)), this,
          SLOT(table_widget_current_changed()));
  table_widget_current_changed();
}

void PngyuMainWindow::append_file_info_list(const QList<QFileInfo> &info_list) {
  if (is_busy()) {
    return;
  }

  m_stop_request = false;
  set_busy_mode(true);

  ui->widget_file_appending->setVisible(true);
  foreach (const QFileInfo &info, info_list) {
    append_file_info_recursive(info, 0);
  }
  update_file_table();
  set_busy_mode(false);
  ui->widget_file_appending->setVisible(false);
}

void PngyuMainWindow::append_file_info_recursive(
    const QFileInfo &file_info, const int recursive_directory_depth) {
  // internal implementation of "append_file_info_list"

  QApplication::processEvents();
  if (m_stop_request) {
    return;
  }
  if (file_info
          .isFile()) { // if file_info is file, png check and add to file_list
    const bool is_png = pngyu::util::has_png_extention(file_info);
    if (is_png && !m_file_list.contains(file_info)) {
      m_file_list.push_back(file_info);
    }
  } else if (file_info.isDir()) { // if file_info is dir, call this function
                                  // recursively by each contents.
    const QDir dir(file_info.absoluteFilePath());
    foreach (const QFileInfo &child_file_info,
             dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
      append_file_info_recursive(child_file_info,
                                 recursive_directory_depth + 1);
    }
  }
}

bool PngyuMainWindow::is_other_output_directory_valid() const {
  QLineEdit *const line_edit = ui->lineEdit_output_directory;
  const QFileInfo output_directory(line_edit->text());
  const bool valid_directory =
      output_directory.isDir() && output_directory.exists();
  return valid_directory;
}

////////
// slots
////////

void PngyuMainWindow::exec_pushed() {
  if (!pngyu::is_executable_pnqguant(QFileInfo(executable_pngquant_path()))) {
    QMessageBox::warning(this, "", "pngquant path is invalid");
    menu_preferences_pushed();
    return;
  }

#ifdef Q_OS_MACOS
  bool b_yes_pushed = false;
  if (image_optim_integration_mode() == pngyu::IMAGEOPTIM_ASK_EVERY_TIME &&
      QFile::exists(
          executable_image_optim_path())) { // confirm optimize with ImageOptim
    PngyuImageOptimIntegrationQuestionDialog dlg;
    const int res = dlg.exec();
    b_yes_pushed = (res == QDialog::Accepted);
    if (dlg.is_dont_ask_again_checked()) {
      set_image_optim_integration_mode(b_yes_pushed
                                           ? pngyu::IMAGEOPTIM_ALWAYS_ENABLED
                                           : pngyu::IMAGEOPTIM_ALWAYS_DISABLED);
    }
  }
  const bool b_image_optim =
      image_optim_integration_mode() == pngyu::IMAGEOPTIM_ALWAYS_ENABLED ||
      b_yes_pushed;
#else
  const bool b_image_optim = false;
#endif

  // Save last used options before executing compression
  save_last_used_options();

  QElapsedTimer t;
  t.start();
  execute_compress_all(b_image_optim);
  qDebug() << "execute" << t.elapsed() << "ms elapsed";
}

void PngyuMainWindow::compress_option_changed() {
  m_preview_window->set_current_pngquant_option(
      make_pngquant_option(QString()));
}

void PngyuMainWindow::other_output_directory_changed() {
  QLineEdit *const line_edit = ui->lineEdit_output_directory;
  QPalette palette = line_edit->palette();
  if (is_other_output_directory_valid() || other_output_directory().isEmpty()) {
    palette.setBrush(QPalette::Text, QBrush());
  } else { // if output directory is invalid, change text color
    palette.setBrush(QPalette::Text, QBrush(Qt::red));
  }
  line_edit->setPalette(palette);
}

void PngyuMainWindow::open_output_directory_pushed() {
  const QString &current_dir = other_output_directory();

  const QString default_dir =
      QFile::exists(current_dir) ? current_dir : QDir::homePath();

  const QString path =
      QFileDialog::getExistingDirectory(this, QString(), default_dir);
  if (!path.isEmpty()) {
    set_other_output_directory(path);
  }
}

void PngyuMainWindow::compress_option_mode_changed() {
  const pngyu::CompressOptionMode mode = current_compress_option_mode();
  ui->widget_compress_option_custom_options->setVisible(
      mode == pngyu::COMPRESS_OPTION_CUSTOM);
}

void PngyuMainWindow::output_option_mode_changed() {
  const pngyu::OutputOptionMode mode = current_output_option_mode();
  ui->widget_output_option_custom->setVisible(mode ==
                                              pngyu::OUTPUT_OPTION_CUSTOM);
}

void PngyuMainWindow::output_directory_mode_changed() {
  const pngyu::OuputDirectoryMode mode = current_output_directory_mode();

  ui->toolButton_open_output_directory->setEnabled(mode ==
                                                   pngyu::OUTPUT_DIR_OTHER);
  ui->lineEdit_output_directory->setEnabled(mode == pngyu::OUTPUT_DIR_OTHER);
  QPalette p = ui->lineEdit_output_directory->palette();
  p.setBrush(QPalette::Base, mode == pngyu::OUTPUT_DIR_OTHER
                                 ? QPalette().brush(QPalette::Base)
                                 : // QBrush(Qt::lightGray) );
                                 QPalette().brush(QPalette::Window));
  ui->lineEdit_output_directory->setPalette(p);
}

void PngyuMainWindow::output_filename_mode_changed() {
  const pngyu::OutputFinenameMode mode = current_output_filename_mode();
  ui->widget_custom_output_filename->setVisible(mode ==
                                                pngyu::OUTPUT_FILE_CUSTOM);

  if (mode == pngyu::OUTPUT_FILE_SAME_AS_ORIGINAL &&
      current_output_directory_mode() == pngyu::OUTPUT_DIR_SAME)
    set_file_overwrite_enabled(true);
}

void PngyuMainWindow::file_list_clear_pushed() { file_list_clear(); }

void PngyuMainWindow::ncolor_spinbox_changed() {
  // for syncronize ncolor spinbox and slider

  const int n = ncolor();
  int slider_value = 256;
  if (n < 4)
    slider_value = 0;
  else if (n < 8)
    slider_value = 1;
  else if (n < 16)
    slider_value = 2;
  else if (n < 32)
    slider_value = 3;
  else if (n < 64)
    slider_value = 4;
  else if (n < 128)
    slider_value = 5;
  else if (n < 256)
    slider_value = 6;
  else
    slider_value = 7;
  ui->horizontalSlider_colors->setValue(slider_value);
}

void PngyuMainWindow::ncolor_slider_changed() {
  // for syncronize ncolor spinbox and slider

  const int slider_value = ui->horizontalSlider_colors->value();
  int ncolor = 256;
  switch (slider_value) {
  case 0:
    ncolor = 2;
    break;
  case 1:
    ncolor = 4;
    break;
  case 2:
    ncolor = 8;
    break;
  case 3:
    ncolor = 16;
    break;
  case 4:
    ncolor = 32;
    break;
  case 5:
    ncolor = 64;
    break;
  case 6:
    ncolor = 128;
    break;
  case 7:
    ncolor = 256;
    break;
  }
  set_ncolor(ncolor);
}

void PngyuMainWindow::table_widget_current_changed() {
  const QString current_path = current_selected_filename();

  m_preview_window->set_png_file(current_path);

  if (!current_path.isEmpty()) {

    { // set icon to preview window button
      ui->pushButton_preview->setEnabled(true);
      const QSize &icon_size = ui->pushButton_preview->iconSize();
      const QImage &icon_image =
          pngyu::util::read_thumbnail_image(current_path, icon_size.width());

      ui->pushButton_preview->setIcon(QPixmap::fromImage(icon_image));
    }
  } else {
    ui->pushButton_preview->setEnabled(false);
    ui->pushButton_preview->setIcon(QIcon());
  }
}

void PngyuMainWindow::preview_button_toggled(bool b) {
  m_preview_window->setVisible(b);
}

void PngyuMainWindow::preview_window_closed() {
  ui->pushButton_preview->setChecked(false);
}

void PngyuMainWindow::add_file_button_pushed() {
  const QStringList filepath_list = QFileDialog::getOpenFileNames(
      this, QString(), QDir::homePath(), "PNG file (*.png);;");

  if (filepath_list.empty()) {
    return;
  }

  QList<QFileInfo> fileinfo_list;
  foreach (const QString &path, filepath_list) {
    fileinfo_list.push_back(QFileInfo(path));
  }

  append_file_info_list(fileinfo_list);
}

void PngyuMainWindow::menu_preferences_pushed() {

  m_preferences_dialog->set_n_jobs(num_thread());
  m_preferences_dialog->set_image_optim_integrate_mode(
      image_optim_integration_mode());
  m_preferences_dialog->set_image_optim_path(executable_image_optim_path());
  m_preferences_dialog->set_pngquant_paths(pngyu::find_executable_pngquant());
  m_preferences_dialog->set_pngquant_path(executable_pngquant_path());
  m_preferences_dialog->set_force_execute_if_negative_enabled(
      is_force_execute_if_negative_enabled());
  m_preferences_dialog->set_timeout_ms(timeout_ms());
  m_preferences_dialog->set_save_compress_options_enabled(
      is_save_compress_options_enabled());
  m_preferences_dialog->set_save_output_options_enabled(
      is_save_output_options_enabled());

  m_preferences_dialog->set_apply_button_enabled(false);

  m_preferences_dialog->show();
}

void PngyuMainWindow::menu_quit_pushed() { close(); }

void PngyuMainWindow::preferences_apply_pushed() {
  set_num_thread(m_preferences_dialog->n_jobs());
  set_image_optim_integration_mode(
      m_preferences_dialog->image_optim_integrate_mode());
  set_executable_image_optim_path(m_preferences_dialog->image_optim_path());
  set_executable_pngquant_path(m_preferences_dialog->pngquant_path());
  set_force_execute_if_negative_enabled(
      m_preferences_dialog->is_force_execute_if_negative_enabled());
  set_timeout_ms(m_preferences_dialog->timeout_ms());
  set_save_compress_options_enabled(
      m_preferences_dialog->is_save_compress_options_enabled());
  set_save_output_options_enabled(
      m_preferences_dialog->is_save_output_options_enabled());
}

void PngyuMainWindow::stop_request() {
  m_stop_request = true;
  ui->pushButton_stop_exec->setEnabled(false);
}
