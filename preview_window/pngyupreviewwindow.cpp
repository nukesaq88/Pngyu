#include "pngyupreviewwindow.h"
#include "ui_pngyupreviewwindow.h"

#include <QPalette>
#include <QFile>
#include <QDebug>

#include "pngyu_util.h"

namespace
{
}

PngyuPreviewWindow::PngyuPreviewWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::PngyuPreviewWindow),
  m_is_image_loaded( false ),
  m_is_compress_finished( false )
{
  ui->setupUi(this);


  { // init image view
    QPalette p = ui->imageview->palette();
    p.setBrush( QPalette::Base, QBrush(QColor("gray")) );
    ui->imageview->setPalette( p );
  }

//  const QString &temporary_path = pngyu::util::app_temporay_path();
//  pngyu::util::make_app_temporary_path();

  connect( &m_execute_compress_thread, SIGNAL(compress_finished()),
           this, SLOT(compress_finished()) );

  connect( ui->checkBox_show_original, SIGNAL(toggled(bool)),
           this, SLOT(show_original_toggled(bool)) );

  // connecst background select buttons
  connect( ui->toolButton_background_checkerboard, SIGNAL(toggled(bool)),
           this, SLOT(background_select_button_pressed()) );
  connect( ui->toolButton_background_black, SIGNAL(toggled(bool)),
           this, SLOT(background_select_button_pressed()) );
  connect( ui->toolButton_background_white, SIGNAL(toggled(bool)),
           this, SLOT(background_select_button_pressed()) );

  connect( ui->toolButton_zoomin, SIGNAL(clicked()),
           this, SLOT(zoomin_pushed()) );
  connect( ui->toolButton_zoomout, SIGNAL(clicked()),
           this, SLOT(zoomout_pushed()) );

  connect( ui->pushButton_original_size, SIGNAL(clicked()),
           this, SLOT(reset_view_scaling()) );

  background_select_button_pressed();
}

PngyuPreviewWindow::~PngyuPreviewWindow()
{
  delete ui;
}

void PngyuPreviewWindow::set_executable_pngquant_path( const QString &path )
{
  m_pngquant_path = path;
  m_execute_compress_thread.set_executable_pngquant_path( path );
}

void PngyuPreviewWindow::set_png_file( const QString &filename )
{
  if( m_png_filename == filename )
  {
    return;
  }

  clear();

  m_png_filename = filename;

  if( filename.isEmpty() )
  {
    m_is_image_loaded = true;
    m_src_image = QImage();
    m_is_compress_finished = true;
    m_dst_image = QImage();
    update_preview();
    return;
  }

  if( isVisible() )
  {
    load_png_file();
    update_preview();
  }
}

void PngyuPreviewWindow::set_current_pngquant_option( const pngyu::PngquantOption &option )
{
  if( m_pngquant_option == option )
  {
    return;
  }
  m_pngquant_option = option;

  clear_compress_result();
  update_preview();
}

bool PngyuPreviewWindow::is_original_show_mode() const
{
  return ui->checkBox_show_original->isChecked();
}

void PngyuPreviewWindow::clear()
{
  ui->spinner->setVisible( false );
  m_png_filename = QString();
  m_is_image_loaded = false;
  m_src_image = QImage();
  clear_compress_result();
  set_current_preview_image( QImage() );
}

void PngyuPreviewWindow::clear_compress_result()
{
  m_is_compress_finished = false;
  m_dst_image = QImage();
  ui->statusbar->showMessage( QString() );
}


void PngyuPreviewWindow::set_current_preview_image( const QImage &image )
{
  ui->imageview->setImage( image );
  ui->imageview->repaint();
}

void PngyuPreviewWindow::load_png_file()
{
  m_src_image = QImage( m_png_filename );
  m_is_image_loaded = true;
}

void PngyuPreviewWindow::execute_compress_start()
{
  clear_compress_result();

  if( m_execute_compress_thread.isRunning() )
  {
    m_execute_compress_thread.stop_request();
    while( m_execute_compress_thread.isRunning() )
    {
    }
  }

  if( ! QFile::exists( m_png_filename ) )
  {
    set_compress_result_failed( tr("Error : Original file doesn't exists.") );
    return;
  }

  m_execute_compress_thread.set_pngquant_option( m_pngquant_option );
  m_execute_compress_thread.set_original_png_data(
        pngyu::util::png_file_to_bytearray( m_png_filename ) );
  m_execute_compress_thread.start();

  ui->spinner->setVisible( true );
}

void PngyuPreviewWindow::set_compress_result_failed( const QString &error_message )
{
  ui->spinner->setVisible( false );
  m_is_compress_finished = true;
  m_dst_image = m_src_image;
  ui->statusbar->showMessage( error_message );
}

bool PngyuPreviewWindow::is_execute_compress_working()
{
  return m_execute_compress_thread.isRunning();
}

void PngyuPreviewWindow::update_preview()
{
  if( ! m_is_image_loaded )
  {
    set_current_preview_image( QImage() );
    return;
  }

  if( is_original_show_mode() )
  {
    set_current_preview_image( m_src_image );
  }
  else
  {
    if( m_is_compress_finished )
    {
      set_current_preview_image( m_dst_image );
    }
    else
    {
      set_current_preview_image( m_src_image );
      if( ! is_execute_compress_working() )
      {
        execute_compress_start();
      }
    }
  }
}

void PngyuPreviewWindow::showEvent( QShowEvent * )
{
  if( ! m_is_image_loaded )
  {
    load_png_file();
  }
  update_preview();
}

void PngyuPreviewWindow::closeEvent( QCloseEvent * )
{
  emit closed();
}

void PngyuPreviewWindow::show_original_toggled( bool )
{
  update_preview();
}

void PngyuPreviewWindow::compress_finished()
{
  if( m_execute_compress_thread.is_compress_succeeded() )
  {
    const QByteArray &src_data = m_execute_compress_thread.original_png_data();
    const QByteArray &dst_data = m_execute_compress_thread.output_png_data();
    const qint64 src_size = src_data.size();
    const qint64 dst_size = dst_data.size();

    const double saved_rate = static_cast<double>( src_size - dst_size ) / ( src_size );

    QString result_test =
        tr("Original Size : ") + pngyu::util::size_to_string_kb( src_size ) + " " +
        tr("Output Size : ") + pngyu::util::size_to_string_kb( dst_size ).rightJustified( 8, ' ' ) + " " +
        tr("Saved : ") + QString::number( static_cast<int>(saved_rate * 100) ).rightJustified( 3, ' ' ) + "% " +
        QString("");

    ui->statusbar->showMessage( result_test );
    m_is_compress_finished = true;
    m_dst_image = QImage::fromData( dst_data );
  }
  else
  {
    set_compress_result_failed( m_execute_compress_thread.error_string() );
  }

  ui->spinner->setVisible( false );

  m_execute_compress_thread.clear_all();
  update_preview();
}

void PngyuPreviewWindow::background_select_button_pressed()
{
  const bool is_checker_checked = ui->toolButton_background_checkerboard->isChecked();
  const bool is_black_checked = ui->toolButton_background_black->isChecked();
  const bool is_white_checked = ui->toolButton_background_white->isChecked();
  if( is_checker_checked && ! is_black_checked && ! is_white_checked )
  {
    ui->imageview->setBackgroundBrush( QBrush( QPixmap(":background/checkerboard.png") ) );
  }
  else if( ! is_checker_checked && is_black_checked && ! is_white_checked )
  {
    ui->imageview->setBackgroundBrush( QBrush( QPixmap(":background/black.png") ) );
  }
  else if( ! is_checker_checked && ! is_black_checked && is_white_checked )
  {
    ui->imageview->setBackgroundBrush( QBrush( QPixmap(":background/white.png") ) );
  }
}

void PngyuPreviewWindow::reset_view_scaling()
{
  ui->imageview->resetMatrix();
}

void PngyuPreviewWindow::zoomin_pushed()
{
  ui->imageview->scale( 1.2, 1.2 );
}

void PngyuPreviewWindow::zoomout_pushed()
{
  ui->imageview->scale( 1.0 / 1.2, 1.0 / 1.2 );
}


