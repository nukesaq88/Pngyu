#ifndef PNGYUMAINWINDOW_H
#define PNGYUMAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QProcess>
#include <QFileInfo>

#include "pngyu_defines.h"
#include "pngyu_pngquant_option.h"

class PngyuPreviewWindow;
class PngyuPreferencesDialog;

namespace Ui {
class PngyuMainWindow;
}

// Main window class of Pngyu application
class PngyuMainWindow : public QMainWindow
{
  Q_OBJECT
public:

  
  explicit PngyuMainWindow(QWidget *parent = 0);
  ~PngyuMainWindow();

  QTableWidget* file_list_table_widget();

  void file_list_clear();

  QString make_output_file_path_string( const QString &input_file_path ) const;

  pngyu::PngquantOption make_pngquant_option( const QString &output_file_suffix ) const;

  void set_executable_pngquant_path( const QString &path );
  QString executable_pngquant_path() const;

  void set_executable_image_optim_path( const QString &path );
  QString executable_image_optim_path() const;

  void set_current_compress_option_mode( const pngyu::CompressOptionMode mode );
  pngyu::CompressOptionMode current_compress_option_mode() const;

  void set_current_output_option_mode( const pngyu::OutputOptionMode mode );
  pngyu::OutputOptionMode current_output_option_mode() const;

  void set_current_output_directory_mode( const pngyu::OuputDirectoryMode mode );
  pngyu::OuputDirectoryMode current_output_directory_mode() const;

  void set_current_outoput_filename_mode( const pngyu::OutputFinenameMode mode );
  pngyu::OutputFinenameMode current_output_filename_mode() const;

  void set_other_output_directory( const QString &other_output_directory );
  QString other_output_directory() const;

  void set_output_filename_prefix( const QString &prefix );
  QString output_filename_prefix() const;

  void set_output_filename_suffix( const QString &suffix );
  QString output_filename_suffix() const;

  void set_ncolor( const int n );
  int ncolor() const;

  void set_file_overwrite_enabled( const bool b );
  bool file_overwrite_enabled() const;

  void set_dither_enabled( const bool b );
  bool dither_enabled() const;

  void set_ie6_alpha_support_enabled( const bool b );
  bool ie6_alpha_support_enabled() const;

  void set_compress_speed( const int speed );
  int compress_speed() const;

  void set_image_optim_integration_mode( const pngyu::ImageOptimIntegration mode );
  pngyu::ImageOptimIntegration image_optim_integration_mode() const;

  void set_num_thread( const int num );
  int num_thread() const;

  void set_force_execute_if_negative_enabled( const bool b );
  bool is_force_execute_if_negative_enabled() const;

  void set_timeout_ms( const int timeout_ms );
  int timeout_ms() const;

  void set_save_compress_options_enabled( const bool b );
  bool is_save_compress_options_enabled() const;

  void set_save_output_options_enabled( const bool b );
  bool is_save_output_options_enabled() const;

  void execute_compress_all( bool image_optim_enabled );

  bool is_preview_window_visible() const;

  QString current_selected_filename() const;

  void set_busy_mode( const bool b);
  bool is_busy() const;

  void clear_compress_result();

  void append_file_info_list( const QList<QFileInfo> &info_list );


protected:
  virtual void dragEnterEvent( QDragEnterEvent *event );
  virtual void dragLeaveEvent( QDragLeaveEvent *event );
  virtual void dragMoveEvent( QDragMoveEvent *event );
  virtual void dropEvent( QDropEvent *event );

  virtual void moveEvent( QMoveEvent *event );
  virtual void showEvent( QShowEvent *event );
  virtual void closeEvent( QCloseEvent *event );

  void read_settings();
  void write_settings();

  void save_last_used_options();
  void load_last_used_options();

  QString get_settings_file_path() const;

  void update_file_table();

  void append_file_info_recursive( const QFileInfo &file_info,
                                   const int recursive_directory_depth );

  bool is_other_output_directory_valid() const;
  
  void update_execution_progress( const int completed_count, const int total_count );
  
private:
  Ui::PngyuMainWindow *ui;
  PngyuPreviewWindow *m_preview_window;
  PngyuPreferencesDialog *m_preferences_dialog;
  QString m_current_pngquant_path;
  QString m_current_imageoptim_path;
  QFileInfoList m_file_list;
  bool m_stop_request;
  bool m_is_busy;
  bool m_temporary_custom_output_custom_on;
  int m_num_thread;
  bool m_image_optim_enabled;
  pngyu::ImageOptimIntegration m_image_optim_integration;
  bool m_force_execute_if_negative_enables;
  int m_timeout_ms;
  bool m_save_compress_options_enabled;
  bool m_save_output_options_enabled;

private slots:
  void exec_pushed();
  void compress_option_changed();
  void other_output_directory_changed();
  void open_output_directory_pushed();
  void compress_option_mode_changed();
  void output_option_mode_changed();
  void output_directory_mode_changed();
  void output_filename_mode_changed();
  void file_list_clear_pushed();
  void ncolor_spinbox_changed();
  void ncolor_slider_changed();
  void table_widget_current_changed();
  void preview_button_toggled( bool );
  void preview_window_closed();
  void add_file_button_pushed();
  void menu_preferences_pushed();
  void menu_quit_pushed();
  void preferences_apply_pushed();
  void stop_request();
};

#endif // PNGYUMAINWINDOW_H
