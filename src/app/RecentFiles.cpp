#include "app/RecentFiles.h"
#include "app/AppSettings.h"

#include <QMenu>
#include <QFileInfo>

RecentFiles::RecentFiles(QObject* parent)
    : QObject(parent)
    , m_menu(new QMenu(QStringLiteral("Document &History")))
{
    connect(m_menu, &QMenu::aboutToShow, this, &RecentFiles::populate);
}

void RecentFiles::add(const QString& path)
{
    AppSettings::instance().addRecentFile(path);
}

void RecentFiles::populate()
{
    m_menu->clear();

    const QStringList recent = AppSettings::instance().recentFiles();
    if (recent.isEmpty())
    {
        QAction* none = m_menu->addAction(QStringLiteral("(no recent documents)"));
        none->setEnabled(false);
        return;
    }

    for (const QString& path : recent)
    {
        QAction* action = m_menu->addAction(QFileInfo(path).fileName());
        action->setToolTip(path);
        connect(action, &QAction::triggered, this,
                [this, path]() { emit openRequested(path); });
    }
}
