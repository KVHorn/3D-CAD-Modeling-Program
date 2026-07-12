#include "SketcherWorkspace.h"

#include <QToolBar>
#include <QAction>

SketcherWorkspace::SketcherWorkspace(QMainWindow* mainWindow)
    : Workspace(QStringLiteral("Sketcher"), mainWindow)
{
    // Placeholder tool set - proves toolbar swapping visually. Real sketch
    // geometry and constraints come with the sketcher milestone.
    for (const QString& name : { QStringLiteral("Line"),
                                 QStringLiteral("Circle"),
                                 QStringLiteral("Arc"),
                                 QStringLiteral("Rectangle"),
                                 QStringLiteral("Constraint") })
    {
        QAction* action = m_toolbar->addAction(name);
        action->setEnabled(false);
        action->setToolTip(name + QStringLiteral(" (coming with the sketcher milestone)"));
    }
}
