#ifndef EXECUTECOMPRESSWORKERTHREAD_H
#define EXECUTECOMPRESSWORKERTHREAD_H

#include <QThread>
#include <QQueue>
#include <QTableWidget>
#include <QMutex>

#include "pngyu_defines.h"

class ExecuteCompressWorkerThread : public QThread
{
  Q_OBJECT
public:
  explicit ExecuteCompressWorkerThread(QObject *parent = 0);

  static pngyu::CompressResult execute_compress(
      const QString &src_path,
      const QString &dst_path,
      const QString &pngquant_path,
      const pngyu::PngquantOption &option,
      const bool overwrite_enable,
      const bool force_execute_if_negative,
      const bool * const stop_request
    );

  static pngyu::CompressResult execute_compress(
      const pngyu::CompressQueueData &data,
      const bool * const stop_request
    );

  static void show_compress_result( QTableWidget *table_widget, const int row, const pngyu::CompressResult &result );

  void set_queue_ptr( QQueue<pngyu::CompressQueueData> *queue_ptr,
                      QMutex *mutex = 0);

  QList<pngyu::CompressResult> compress_results() const;
  void clear_result();

  void stop_request();
  
protected:
  virtual void run();

private:
  QQueue<pngyu::CompressQueueData> *m_queue_ptr;
  QMutex *m_mutex_ptr;

  QList<pngyu::CompressResult> m_result_list;

  bool m_stop_request;

signals:
  
public slots:
  
};

#endif // EXECUTECOMPRESSWORKERTHREAD_H
