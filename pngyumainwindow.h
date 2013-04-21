#ifndef PNGYUMAINWINDOW_H
#define PNGYUMAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QProcess>
#include <QFileInfo>

#include "pngyu_difines.h"
#include "pngyu_option.h"

namespace Ui {
class PngyuMainWindow;
}

class PngyuMainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit PngyuMainWindow(QWidget *parent = 0);
  ~PngyuMainWindow();

  QTableWidget* file_list_table_widget();

  void file_list_clear();

  QString make_output_file_path_string( const QString &input_file_path ) const;

  pngyu::PngquantOption make_pngquant_option() const;

  QString executable_pngquant_path() const;

  void set_current_output_directory_mode( const pngyu::OuputDirectoryMode mode );
  pngyu::OuputDirectoryMode current_output_directory_mode() const;

  void set_current_outoput_filename_mode( const pngyu::OutputFinenameMode mode );
  pngyu::OutputFinenameMode current_outoput_filename_mode() const;

  void set_output_directory( const QString &output_directory );
  QString output_directory() const;

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

  void execute_compress();

protected:
  virtual void dragEnterEvent( QDragEnterEvent *event );
  virtual void dragLeaveEvent( QDragLeaveEvent *event );
  virtual void dragMoveEvent( QDragMoveEvent *event );
  virtual void dropEvent( QDropEvent *event );

  void update_file_table();

  void append_file_info_recursive( const QFileInfo &file_info,
                                   const int recursive_directory_depth = 0 );

  bool is_output_directory_valid() const;
  
private:
  Ui::PngyuMainWindow *ui;
  QFileInfoList m_file_list;

private slots:
  void exec_pushed();
  void output_directory_changed();
  void open_output_directory_pushed();
  void output_directory_mode_changed();
  void output_filename_mode_changed();
  void file_list_clear_pushed();
  void ncolor_spinbox_changed();
  void ncolor_slider_changed();
  void table_widget_current_changed();
};

#endif // PNGYUMAINWINDOW_H
