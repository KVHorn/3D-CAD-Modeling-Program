#include "Workspace.h"

#include <QToolBar>
#include <QMainWindow>
#include <QDebug>

Workspace::Workspace(const QString& name, QMainWindow* mainWindow)
    : QObject(mainWindow)
    , m_toolbar(new QToolBar(name, mainWindow))
    , m_activeDocument(nullptr)
    , m_name(name)
{
    // The toolbar is added to the main window once, hidden. activate() and
    // deactivate() just toggle visibility - no add/remove churn.
    mainWindow->addToolBar(m_toolbar);
    m_toolbar->setVisible(false);
}

void Workspace::activate(Document* document)
{
    m_activeDocument = document;
    m_toolbar->setVisible(true);
    qDebug() << "Workspace" << m_name << "activated";
}

void Workspace::deactivate()
{
    m_toolbar->setVisible(false);
    m_activeDocument = nullptr;
    qDebug() << "Workspace" << m_name << "deactivated";
}
