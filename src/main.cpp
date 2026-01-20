#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QFont>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QStyleHints>
#include <QTranslator>

#include "pngyumainwindow.h"

class PngyuApplication : public QApplication {
 public:
  PngyuApplication(int& argc, char** argv)
      : QApplication(argc, argv), m_pngyu_main(0) {}

  void set_pngyu_main(PngyuMainWindow* p) { m_pngyu_main = p; }

 protected:
  virtual bool event(QEvent* e) {
    if (e->type() == QEvent::FileOpen) {  // for MAC OS X Drag and Drop event
      const QFileOpenEvent* const file_event = static_cast<QFileOpenEvent*>(e);
      if (m_pngyu_main) {
        m_pngyu_main->append_file_info_list({QFileInfo(file_event->file())});
        return true;
      } else {
        return QApplication::event(e);
      }
    }
    return QApplication::event(e);
  }

 private:
  PngyuMainWindow* m_pngyu_main;
};

int main(int argc, char* argv[]) {
  PngyuApplication a(argc, argv);

  // Set application information for QSettings
  QCoreApplication::setOrganizationName("Pngyu");
  QCoreApplication::setApplicationName("Pngyu");

  // Load language setting
  QSettings settings(PngyuMainWindow::get_settings_file_path(),
                     QSettings::IniFormat);
  settings.beginGroup("options");
  const QString language = settings.value("language", "auto").toString();
  settings.endGroup();

  // Determine locale to use
  const QLocale locale =
      (language == "auto") ? QLocale::system() : QLocale(language);

  // Load Qt translations
  QTranslator qtTranslator;
  if (qtTranslator.load(locale, "qt", "_",
                        QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
    a.installTranslator(&qtTranslator);
  }

  // Load app translations
  QTranslator appTranslator;
  if (language == "auto") {
    // For auto mode, use system locale
    // QTranslator will try: en_US, en, ja_JP, ja, zh_CN, zh, etc.
    if (appTranslator.load(locale, ":/translations")) {
      a.installTranslator(&appTranslator);
    }
  } else {
    // For explicit language, load by language code directly
    if (appTranslator.load(":/translations/" + language)) {
      a.installTranslator(&appTranslator);
    }
  }

  PngyuMainWindow w;
  a.set_pngyu_main(&w);
  w.show();

  // Hack: Fix color scheme issue in CI build
  QPalette pal = qApp->palette();
  if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light) {
    pal.setColor(QPalette::Window, QColor(255, 255, 255));
  } else {
    pal.setColor(QPalette::Window, QColor(30, 30, 30));
  }
  qApp->setPalette(pal);

  qDebug() << "Locale:" << locale.name();
  qDebug() << "Style:" << a.style()->objectName();
  qDebug() << "Palette window:" << qApp->palette().color(QPalette::Window);
  qDebug() << "Palette base:" << qApp->palette().color(QPalette::Base);
  qDebug() << "Color scheme:" << QGuiApplication::styleHints()->colorScheme();

  const QStringList argments = a.arguments();
  QFileInfoList file_list;
  for (int i = 1; i < argments.size(); ++i) {
    const QFileInfo finfo(argments.at(i));
    if (finfo.exists()) {
      file_list.push_back(finfo);
    }
  }
  w.append_file_info_list(file_list);
  return a.exec();
}
