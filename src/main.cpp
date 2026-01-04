#include "pngyumainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QFileInfo>
#include <QFileOpenEvent>

class PngyuApplication : public QApplication {
public:
  PngyuApplication(int &argc, char **argv)
      : QApplication(argc, argv), m_pngyu_main(0) {}

  void set_pngyu_main(PngyuMainWindow *p) { m_pngyu_main = p; }

protected:
  virtual bool event(QEvent *e) {
    if (e->type() == QEvent::FileOpen) { // for MAC OS X Drag and Drop event
      const QFileOpenEvent *const file_event = static_cast<QFileOpenEvent *>(e);
      if (m_pngyu_main) {
        QList<QFileInfo> info_list;
        const QString file = file_event->file();
        info_list.push_back(QFileInfo(file_event->file()));
        m_pngyu_main->append_file_info_list(info_list);
        return true;
      } else {
        return QApplication::event(e);
      }
    }
    return QApplication::event(e);
  }

private:
  PngyuMainWindow *m_pngyu_main;
};

int main(int argc, char *argv[]) {
  PngyuApplication a(argc, argv);

  // Set application information for QSettings
  QCoreApplication::setOrganizationName("Pngyu");
  QCoreApplication::setApplicationName("Pngyu");

  PngyuMainWindow w;
  a.set_pngyu_main(&w);
  w.show();

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
