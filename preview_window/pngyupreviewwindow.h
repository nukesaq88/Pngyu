#ifndef PNGYUPREVIEWWINDOW_H
#define PNGYUPREVIEWWINDOW_H

#include <QMainWindow>

#include "pngyu_pngquant_option.h"

#include "executecompressthread.h"

namespace Ui {
class PngyuPreviewWindow;
}

class PngyuPreviewWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit PngyuPreviewWindow(QWidget *parent = 0);
  ~PngyuPreviewWindow();

  void set_executable_pngquant_path( const QString &path );

  void set_png_file( const QString &filename );

  void set_current_pngquant_option( const pngyu::PngquantOption &option );

  bool is_original_show_mode() const;
  
protected:
  void clear();

  void clear_compress_result();

  void set_current_preview_image( const QImage &image );

  void load_png_file();

  void execute_compress_start();

  void set_compress_result_failed( const QString &error_message );

  bool is_execute_compress_working();


  void update_preview();

  virtual void showEvent(QShowEvent *);
  virtual void closeEvent(QCloseEvent *);

private:
  Ui::PngyuPreviewWindow *ui;

  ExecuteCompressThread m_execute_compress_thread;

  QString m_pngquant_path;
  QString m_png_filename;
  pngyu::PngquantOption m_pngquant_option;
  bool m_is_image_loaded;
  bool m_is_compress_finished;
  QImage m_src_image;
  QImage m_dst_image;

private slots:
  void show_original_toggled( bool );
  void compress_finished();
  void background_select_button_pressed();
  void reset_view_scaling();
  void zoomin_pushed();
  void zoomout_pushed();

signals:
  void closed();
};

#endif // PNGYUPREVIEWWINDOW_H
