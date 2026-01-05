#include "executecompressthread.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QProcess>
#include <QRegularExpression>

ExecuteCompressThread::ExecuteCompressThread(QObject* parent)
    : QThread(parent), m_stop_request(false) {
  clear_all();
}

void ExecuteCompressThread::clear_result() {
  m_stop_request = false;
  m_error_string = QString();
  m_dst_png_data.clear();
}

void ExecuteCompressThread::clear_all() {
  m_src_png_data.clear();
  clear_result();
}

void ExecuteCompressThread::set_executable_pngquant_path(const QString& path) {
  m_pngquant_path = path;
}

void ExecuteCompressThread::set_pngquant_option(
    const pngyu::PngquantOption& option) {
  m_pngquant_option = option;
}

bool ExecuteCompressThread::is_compress_succeeded() const {
  return !m_dst_png_data.isEmpty();
}

QByteArray ExecuteCompressThread::output_png_data() const {
  return m_dst_png_data;
}

int ExecuteCompressThread::saved_size() const {
  return static_cast<int>(m_src_png_data.size() - m_dst_png_data.size());
}

QString ExecuteCompressThread::error_string() const { return m_error_string; }

void ExecuteCompressThread::set_original_png_data(const QByteArray& data) {
  m_src_png_data = data;
}

QByteArray ExecuteCompressThread::original_png_data() const {
  return m_src_png_data;
}

void ExecuteCompressThread::stop_request() { m_stop_request = true; }

void ExecuteCompressThread::run() {
  m_stop_request = false;

  clear_result();

  const QString& command =
      m_pngquant_option.make_pngquant_command_option_stdio_mode();
  // m_pngquant_path );

  QString error_string;

  bool b_stop_request_called = false;

  QProcess process;
  process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());

  try {
    if (!QFile::exists(m_pngquant_path)) {
      throw tr("Error: pngquant path not found : ") + m_pngquant_path;
    }

    if (m_src_png_data.isEmpty()) {
      throw tr("Error: Original data is empty");
    }

    process.start(m_pngquant_path,
                  command.trimmed().split(QRegularExpression("[ ]+")));

    {  // waiting for process started or timeout or stop request.
      QElapsedTimer t;
      t.start();
      const int timeout_ms = m_pngquant_option.get_timeout_ms();
      while (!process.waitForStarted(30)) {
        if (m_stop_request) {
          b_stop_request_called = true;
          throw tr("Error: stop request");
        }

        if (t.elapsed() > timeout_ms) {
          throw tr("Error: Process cannot started");
        }
      }
    }

    // send src png file data to pngquant with std input
    process.write(m_src_png_data);
    process.closeWriteChannel();

    {  // waiting for process started or timeout or stop request.
      QElapsedTimer t;
      t.start();
      const int timeout_ms = m_pngquant_option.get_timeout_ms();
      while (!process.waitForFinished(30)) {
        if (m_stop_request) {
          b_stop_request_called = true;
          throw tr("Error: stop request");
        }

        if (t.elapsed() > timeout_ms) {
          throw tr("Error: Process timeout");
        }
      }
    }
    if (m_stop_request) {
      throw tr("Error: stop request");
    }

    const int exit_code = process.exitCode();
    if (exit_code != 0) {
      throw tr("Error code : %1\nCause : %2")
          .arg(exit_code)
          .arg(QString(process.readAllStandardError()));
    }

    m_dst_png_data = process.readAllStandardOutput();
  } catch (const QString& e) {
    if (process.state() == QProcess::Running) {
      process.kill();
      process.waitForFinished(1000);
    }
    m_dst_png_data.clear();
    error_string = e;
  }

  m_error_string = error_string;
  if (m_stop_request) {
    b_stop_request_called = true;
  }

  m_stop_request = false;

  if (!b_stop_request_called) {
    emit compress_finished();
  }
}
