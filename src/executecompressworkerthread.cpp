#include "executecompressworkerthread.h"

#include <QDebug>
#include <QFile>
#include <QMutexLocker>

#include "executecompressthread.h"
#include "pngyu_custom_tablewidget_item.h"
#include "pngyu_util.h"

ExecuteCompressWorkerThread::ExecuteCompressWorkerThread(QObject* parent)
    : QThread(parent), m_queue_ptr(0), m_mutex_ptr(0), m_stop_request(false) {}

pngyu::CompressResult ExecuteCompressWorkerThread::execute_compress(
    const QString& src_path, const QString& dst_path,
    const QString& pngquant_path, const pngyu::PngquantOption& option,
    const bool overwrite_enable, const bool force_execute_if_negative,
    const bool* const stop_request) {
  pngyu::CompressResult res;
  res.src_path = src_path;
  res.dst_path = dst_path;

  try {
    if (dst_path.isEmpty()) {
      throw tr("Error: Output option is invalid");
    }

    const bool dst_path_exists = QFile::exists(dst_path);
    if (dst_path_exists && !overwrite_enable) {
      throw tr("Error: \"%1\" is already exists").arg(dst_path);
    }

    const QByteArray& src_png_data =
        pngyu::util::png_file_to_bytearray(src_path);

    // init compress thred instance,
    // compress process will execute in other thread via this instance.
    ExecuteCompressThread compress_thread;
    compress_thread.set_executable_pngquant_path(pngquant_path);
    compress_thread.set_pngquant_option(option);

    {  // exetute pngquant command
      compress_thread.clear_result();
      compress_thread.set_original_png_data(src_png_data);
      compress_thread.start();
      // waiting for compress finished
      bool canceled = false;
      while (!compress_thread.wait(100)) {
        if (stop_request && (*stop_request)) {
          canceled = true;
          break;
        }
      }

      // If canceled, terminate the thread and wait for it to finish
      if (canceled) {
        compress_thread.terminate();
        compress_thread.wait();
        throw tr("Canceled");
      }

      // compres result check
      if (!compress_thread.is_compress_succeeded()) {
        throw compress_thread.error_string();
      }
    }

    const QByteArray& dst_png_data =
        (force_execute_if_negative || compress_thread.saved_size() >= 0)
            ? compress_thread.output_png_data()
            : compress_thread.original_png_data();

    if (dst_path_exists) {
      // remove old file
      if (!QFile::remove(dst_path)) {
        throw tr("Error: Couldn't overwrite");
      }
    }

    // copy result file to dst_path
    if (!pngyu::util::write_png_data(dst_path, dst_png_data)) {
      throw tr("Error: Couldn't save output file");
    }

    res.result = true;
    res.src_size = src_png_data.size();
    res.dst_size = dst_png_data.size();
  } catch (const QString& ex) {
    res.result = false;
    res.error_message = ex;
  }

  return res;
}

pngyu::CompressResult ExecuteCompressWorkerThread::execute_compress(
    const pngyu::CompressQueueData& data, const bool* const stop_request) {
  return execute_compress(data.src_path, data.dst_path, data.pngquant_path,
                          data.pngquant_option, data.overwrite_enabled,
                          data.force_execute_if_negative, stop_request);
}

void ExecuteCompressWorkerThread::show_compress_result(
    QTableWidget* table_widget, const int row,
    const pngyu::CompressResult& result) {
  if (!table_widget) {
    return;
  }

  QTableWidgetItem* const resultItem = new QTableWidgetItem();
  table_widget->setItem(row, pngyu::COLUMN_RESULT, resultItem);

  if (result.result) {  // Succeed

    resultItem->setIcon(pngyu::util::success_icon());

    const qint64 src_size = result.src_size;
    const qint64 dst_size = result.dst_size;

    pngyu::TableValueCompareItem* const origin_size_item =
        new pngyu::TableValueCompareItem(
            pngyu::util::size_to_string_kb(src_size));
    origin_size_item->setData(1, static_cast<double>(src_size));

    pngyu::TableValueCompareItem* const output_size_item =
        new pngyu::TableValueCompareItem(
            pngyu::util::size_to_string_kb(dst_size));
    output_size_item->setData(1, static_cast<double>(dst_size));

    const double saving_size = static_cast<double>(src_size - dst_size);
    const double saving_rate = saving_size / (src_size);

    pngyu::TableValueCompareItem* const saving_size_item =
        new pngyu::TableValueCompareItem(
            pngyu::util::size_to_string_kb(static_cast<qint64>(saving_size)));
    saving_size_item->setData(1, static_cast<double>(saving_size));

    pngyu::TableValueCompareItem* const saving_rate_item =
        new pngyu::TableValueCompareItem(
            QString("%1%").arg(static_cast<int>(saving_rate * 100)));
    saving_rate_item->setData(1, static_cast<double>(saving_rate));

    table_widget->setItem(row, pngyu::COLUMN_ORIGINAL_SIZE, origin_size_item);
    table_widget->setItem(row, pngyu::COLUMN_OUTPUT_SIZE, output_size_item);
    table_widget->setItem(row, pngyu::COLUMN_SAVED_SIZE, saving_size_item);
    table_widget->setItem(row, pngyu::COLUMN_SAVED_SIZE_RATE, saving_rate_item);
  } else {
    resultItem->setText(result.error_message);
    resultItem->setToolTip(result.error_message);
    resultItem->setIcon(pngyu::util::failure_icon());
  }

  table_widget->scrollToItem(resultItem);
}

void ExecuteCompressWorkerThread::set_queue_ptr(
    QQueue<pngyu::CompressQueueData>* queue_ptr, QMutex* mutex) {
  m_queue_ptr = queue_ptr;
  m_mutex_ptr = mutex;
}

QList<pngyu::CompressResult> ExecuteCompressWorkerThread::compress_results()
    const {
  return m_result_list;
}

void ExecuteCompressWorkerThread::clear_result() { m_result_list.clear(); }

void ExecuteCompressWorkerThread::stop_request() { m_stop_request = true; }

void ExecuteCompressWorkerThread::run() {
  if (!m_queue_ptr) {
    return;
  }
  m_stop_request = false;

  while (!m_stop_request && !m_queue_ptr->isEmpty()) {
    pngyu::CompressQueueData data;
    {
      QMutexLocker locker(m_mutex_ptr);
      if (m_queue_ptr->isEmpty()) {
        break;
      }
      data = m_queue_ptr->dequeue();
    }
    const pngyu::CompressResult& res = execute_compress(data, &m_stop_request);

    show_compress_result(data.table_widget, data.table_row, res);
    m_result_list.push_back(res);
  }
}
