#include "executecompressworkerthread.h"


#include <QFile>
#include <QDebug>
#include <QMutexLocker>

#include "executecompressthread.h"
#include "pngyu_util.h"

namespace
{
QMutex g_mutex;
}

ExecuteCompressWorkerThread::ExecuteCompressWorkerThread(QObject *parent) :
  QThread(parent),
  m_queue_ptr( 0 ),
  m_stop_request(false)
{
}

pngyu::CompressResult ExecuteCompressWorkerThread::execute_compress(
    const QString &src_path,
    const QString &dst_path,
    const QString &pngquant_path,
    const pngyu::PngquantOption &option,
    const bool overwrite_enable )
{
  pngyu::CompressResult res;
  res.src_path = src_path;
  res.dst_path = dst_path;

  try
  {

    if( dst_path.isEmpty() )
    {
      throw tr( "Error: Output option is invalid" );
    }

    const bool dst_path_exists = QFile::exists( dst_path );
    if( dst_path_exists && ! overwrite_enable )
    {
      throw tr( "Error: \"%1\" is already exists" ).arg( dst_path );
    }

    const QByteArray &src_png_data = pngyu::util::png_file_to_bytearray( src_path );


    // init compress thred instance,
    // compress process will execute in other thread via this instance.
    ExecuteCompressThread compress_thread;
    compress_thread.set_executable_pngquant_path( pngquant_path );
    compress_thread.set_pngquant_option( option );

    { // exetute pngquant command
      compress_thread.clear_result();
      compress_thread.set_original_png_data( src_png_data );
      compress_thread.start();
      // waiting for compress finished
      while( ! compress_thread.wait(50) )
      {
        // avoid "aplication no response"
        QApplication::processEvents();
      }

      // compres result check
      if( ! compress_thread.is_compress_succeeded() )
      {
        throw compress_thread.error_string();
      }

    }

    const QByteArray &dst_png_data = compress_thread.output_png_data();

    if( dst_path_exists )
    {
      // remove old file
      if( ! QFile::remove( dst_path ) )
      {
        throw tr( "Error: Couldn't overwrite" );
      }
    }

    // copy result file to dst_path
    if( ! pngyu::util::write_png_data( dst_path, dst_png_data ) )
    {
       throw tr( "Couldn't save output file" );
    }

    res.result = true;
    res.src_size = src_png_data.size();
    res.dst_size = dst_png_data.size();
  }
  catch( const QString &ex )
  {
    res.result = false;
    res.error_message = ex;
  }

  return res;
}

pngyu::CompressResult ExecuteCompressWorkerThread::execute_compress(
    const pngyu::CompressQueueData &data )
{
  return execute_compress(
        data.src_path,
        data.dst_path,
        data.pngquant_path,
        data.pngquant_option,
        data.overwrite_enabled );
}

void ExecuteCompressWorkerThread::show_compress_result( QTableWidget *table_widget, const int row, const pngyu::CompressResult &result )
{
  if( ! table_widget )
  {
    return;
  }

  QTableWidgetItem * const  resultItem = new QTableWidgetItem();
  table_widget->setItem( row, pngyu::COLUMN_RESULT, resultItem );

  if( result.result )
  { // Succeed

    resultItem->setIcon( pngyu::util::success_icon() );

    const qint64 src_size = result.src_size;
    const qint64 dst_size = result.dst_size;

    table_widget->setItem( row, pngyu::COLUMN_ORIGINAL_SIZE,
                           new QTableWidgetItem( pngyu::util::size_to_string_kb( src_size ) ) );
    table_widget->setItem( row, pngyu::COLUMN_OUTPUT_SIZE,
                           new QTableWidgetItem( pngyu::util::size_to_string_kb( dst_size ) ) );
    const double saving_rate = static_cast<double>( src_size - dst_size ) / ( src_size );

    table_widget->setItem( row, pngyu::COLUMN_SAVED_SIZE,
                           new QTableWidgetItem( QString( "%1%" ).arg( static_cast<int>(saving_rate * 100) ) ) );
  }
  else
  {
    resultItem->setText( result.error_message );
    resultItem->setToolTip( result.error_message );
    resultItem->setIcon( pngyu::util::failure_icon() );
  }

  table_widget->scrollToItem( resultItem );
}


void ExecuteCompressWorkerThread::set_queue_ptr( QQueue<pngyu::CompressQueueData> *queue_ptr )
{
  m_queue_ptr = queue_ptr;
}

QList<pngyu::CompressResult> ExecuteCompressWorkerThread::compress_results() const
{
  return m_result_list;
}

void ExecuteCompressWorkerThread::clear_result()
{
  m_result_list.clear();
}

void ExecuteCompressWorkerThread::stop_request()
{
  m_stop_request = true;
}


void ExecuteCompressWorkerThread::run()
{
  if( ! m_queue_ptr )
  {
    return;
  }
  m_stop_request = false;

  while( ! m_stop_request && ! m_queue_ptr->isEmpty() )
  {
    pngyu::CompressQueueData data;
    {
      QMutexLocker locker( &g_mutex );
      if( m_queue_ptr->isEmpty() )
      {
        break;
      }
      data = m_queue_ptr->dequeue();
    }
    const pngyu::CompressResult &res = execute_compress( data );

    show_compress_result( data.table_widget, data.table_row, res );
    m_result_list.push_back( res );
  }
}
