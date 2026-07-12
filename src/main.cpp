#include "app/MainWindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

// File-based logging. The exe is a GUI-subsystem app (WIN32 in CMake), so
// there is no console; every qDebug/qWarning/qCritical line is appended to
// debug.log next to the executable. Tail it live in PowerShell:
//   Get-Content debug.log -Wait
static void fileMessageHandler(QtMsgType type, const QMessageLogContext& /*context*/,
                               const QString& msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    QFile logFile(QStringLiteral("debug.log"));
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    const char* level = "DEBUG";
    switch (type)
    {
        case QtDebugMsg:    level = "DEBUG";    break;
        case QtInfoMsg:     level = "INFO";     break;
        case QtWarningMsg:  level = "WARNING";  break;
        case QtCriticalMsg: level = "CRITICAL"; break;
        case QtFatalMsg:    level = "FATAL";    break;
    }

    QTextStream stream(&logFile);
    stream << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz"))
           << " [" << level << "] " << msg << "\n";
}

int main(int argc, char* argv[])
{
    qInstallMessageHandler(fileMessageHandler);

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Infinity Creator"));
    app.setOrganizationName(QStringLiteral("KVHorn"));

    qDebug() << "=== Infinity Creator starting ===";

    MainWindow window;
    window.show();

    const int result = app.exec();
    qDebug() << "=== Infinity Creator exiting with code" << result << "===";
    return result;
}
