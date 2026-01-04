#ifndef EXECUTECOMPRESSTHREAD_H
#define EXECUTECOMPRESSTHREAD_H

#include <QByteArray>
#include <QThread>

#include "pngyu_defines.h"
#include "pngyu_pngquant_option.h"

class ExecuteCompressThread : public QThread {
  Q_OBJECT
public:
  explicit ExecuteCompressThread(QObject *parent = 0);

  void clear_result();

  void clear_all();

  void set_executable_pngquant_path(const QString &path);

  void set_pngquant_option(const pngyu::PngquantOption &option);

  void set_original_png_data(const QByteArray &data);
  QByteArray original_png_data() const;

  bool is_compress_succeeded() const;

  QByteArray output_png_data() const;

  int saved_size() const;

  QString error_string() const;

  void stop_request();

protected:
  virtual void run();

private:
  pngyu::PngquantOption m_pngquant_option;
  QString m_pngquant_path;
  QByteArray m_src_png_data;
  QByteArray m_dst_png_data;
  QString m_error_string;
  bool m_stop_request;

signals:
  void compress_finished();
public slots:
};

#endif // EXECUTECOMPRESSTHREAD_H
