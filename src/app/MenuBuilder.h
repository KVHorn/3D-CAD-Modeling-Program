#ifndef MENUBUILDER_H
#define MENUBUILDER_H

#include <QObject>

class QMenu;
class QAction;
class MainWindow;

// Constructs the seven-tab menu system (File, Edit, View, Insert,
// Workspace, Window, Help) agreed in docs/SPEC.md, keeping MainWindow.cpp
// lean. Items whose features haven't landed yet appear disabled with a
// "(Phase N)" tooltip, so the full shape of the program is visible from
// day one and items light up as phases ship.
class MenuBuilder : public QObject
{
    Q_OBJECT

public:
    explicit MenuBuilder(MainWindow* mainWindow);

private:
    void buildFileMenu();
    void buildEditMenu();
    void buildViewMenu();
    void buildInsertMenu();
    void buildWorkspaceMenu();
    void buildWindowMenu();
    void buildHelpMenu();

    // A visible-but-disabled item for a future phase.
    QAction* addPlanned(QMenu* menu, const QString& text, int phase);

    MainWindow* m_mainWindow;
};

#endif // MENUBUILDER_H
