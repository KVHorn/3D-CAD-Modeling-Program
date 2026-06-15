#include "Document.h"

Document::Document(const QString &name)
{
    // Constructor body - set things up
}

OcctViewport *Document::viewport() const
{
    return m_viewport; // The Body: what the function actually does
}

WorkspaceType Document::currentWorkspace() const
{
    return m_workspace;
}

void Document::setCurrentWorkspace(WorkspaceType type)
{
    m_workspace = type;
}