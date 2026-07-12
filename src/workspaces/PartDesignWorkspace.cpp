#include "workspaces/PartDesignWorkspace.h"
#include "app/Document.h"
#include "model/FeatureFactory.h"

#include <QToolBar>
#include <QAction>
#include <QDebug>

PartDesignWorkspace::PartDesignWorkspace(QMainWindow* mainWindow)
    : Workspace(QStringLiteral("Part Design"), mainWindow)
{
    for (const QString& typeName : FeatureFactory::primitiveTypes())
    {
        QAction* action = m_toolbar->addAction(typeName);
        action->setToolTip(QStringLiteral("Insert a %1 (opens %1 Definition)").arg(typeName));
        connect(action, &QAction::triggered, this,
                [this, typeName]() { onCreateFeature(typeName); });
    }

    m_toolbar->addSeparator();

    for (const QString& typeName : FeatureFactory::dressUpTypes())
    {
        QAction* action = m_toolbar->addAction(typeName);
        action->setToolTip(QStringLiteral("%1 all edges of the part (edge selection in Phase 4)")
                               .arg(typeName));
        connect(action, &QAction::triggered, this,
                [this, typeName]() { onCreateFeature(typeName); });
    }
}

void PartDesignWorkspace::onCreateFeature(const QString& typeName)
{
    if (!m_activeDocument)
    {
        qWarning() << typeName << "requested with no active document";
        return;
    }

    m_activeDocument->insertFeatureInteractive(typeName, m_toolbar);
}
