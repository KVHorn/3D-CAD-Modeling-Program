#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QObject>

class QMenu;

// File > Document History: the last few opened/saved .infinity files,
// rebuilt from AppSettings every time the menu opens.
class RecentFiles : public QObject
{
    Q_OBJECT

public:
    explicit RecentFiles(QObject* parent = nullptr);

    QMenu* menu() const { return m_menu; }
    void add(const QString& path);

signals:
    void openRequested(const QString& path);

private slots:
    void populate();

private:
    QMenu* m_menu;
};

#endif // RECENTFILES_H
