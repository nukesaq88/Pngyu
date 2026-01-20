#include "pngyu_execute_pngquant_command.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>

namespace pngyu {

QStringList find_executable_pngquant_from_dir(const QDir& dir) {
  QStringList found_paths;
  if (!dir.exists()) {
    return found_paths;
  }
  for (const QFileInfo& child_file_info : dir.entryInfoList()) {
    if (!child_file_info.baseName().contains(QRegularExpression(
            "pngquant", QRegularExpression::CaseInsensitiveOption)) ||
        !child_file_info.isExecutable()) {
      continue;
    }
    if (pngyu::is_executable_pnqguant(child_file_info)) {
      found_paths.push_back(child_file_info.absoluteFilePath());
    }
  }
  return found_paths;
}

QString pngquant_version(const QString& pngquant_path) {
  QProcess process;
  process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  process.start(pngquant_path, QStringList() << "--version");
  process.waitForFinished();
  const QString& version = process.readAllStandardOutput();
  return version.trimmed();
}

bool is_executable_pnqguant(const QFileInfo pngquant_path) {
  if (!pngquant_path.exists() || !pngquant_path.isExecutable() ||
      pngquant_path.isBundle()) {
    return false;
  }
  const QString& version = pngquant_version(pngquant_path.absoluteFilePath());
  return !version.isEmpty();
}

QStringList find_executable_pngquant() {
  QStringList search_dirs;

#ifdef Q_OS_MACOS
  search_dirs << (QApplication::applicationDirPath() + "/../Resources");
#endif
#ifdef Q_OS_UNIX
  search_dirs << "/usr/bin"
              << "/usr/local/bin"
              << "/usr/sbin";
#endif
#ifdef Q_OS_WIN
  search_dirs << (QApplication::applicationDirPath() + "/pngquant");
#endif

  QStringList found_paths;
  for (const QString& dir : search_dirs) {
    found_paths.append(find_executable_pngquant_from_dir(QDir(dir)));
  }

  return found_paths;
}

QPair<bool, QString> execute_compress(const QString& pngquant_command) {
  typedef QPair<bool, QString> result_t;

  bool compress_succeed = true;
  QString error_string;

  try {
    QProcess process;
    process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process.start(pngquant_command);

    if (!process.waitForStarted()) {
      throw QObject::tr("Error: %1").arg(QObject::tr("Process cannot be started."));
    }
    if (!process.waitForFinished(30000)) {
      throw QObject::tr("Error: %1").arg(QObject::tr("Process timeout."));
    }
    const int exit_code = process.exitCode();
    if (process.exitCode() != 0) {
      throw QObject::tr("Error: %1")
          .arg(QObject::tr("Code %1 - %2")
                   .arg(exit_code)
                   .arg(QString(process.readAllStandardError().trimmed())));
    }
  } catch (const QString& e) {
    compress_succeed = false;
    error_string = e;
  }
  return result_t(compress_succeed, error_string);
}

QPair<QByteArray, QString> execute_compress_stdio_mode(
    const QString& pngquant_command, const QByteArray& src_png_data) {
  typedef QPair<QByteArray, QString> result_t;

  QByteArray dst_png_data;
  QString error_string;
  try {
    if (src_png_data.isEmpty()) {
      throw QObject::tr("Error: %1").arg(QObject::tr("Original data is empty."));
    }

    QProcess process;
    process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process.start(pngquant_command);

    if (!process.waitForStarted()) {
      throw QObject::tr("Error: %1").arg(QObject::tr("Process cannot be started."));
    }
    process.write(src_png_data);
    process.closeWriteChannel();
    if (!process.waitForFinished()) {
      throw QObject::tr("Error: %1").arg(QObject::tr("Process timeout."));
    }
    const int exit_code = process.exitCode();
    if (process.exitCode() != 0) {
      throw QObject::tr("Error: %1")
          .arg(QObject::tr("Code %1 - %2")
                   .arg(exit_code)
                   .arg(QString(process.readAllStandardError().trimmed())));
    }
    dst_png_data = process.readAllStandardOutput();
  } catch (const QString& e) {
    dst_png_data.clear();
    error_string = e;
  }
  return result_t(dst_png_data, error_string);
}

}  // namespace pngyu
