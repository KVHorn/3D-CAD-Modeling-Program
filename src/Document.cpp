#include "Document.h"

Document::Document(const QString &name) : m_name(name)
{
    /*
        m_viewport stays nullptr for now — there's no rendering viewport to attach yet.
        m_workspace defaults to PartDesign (set in the header).
    */
}

QString Document::name() const
{
    return m_name;
}

OcctViewport *Document::viewport() const
{
    return m_viewport;
}

WorkspaceType Document::currentWorkspace() const
{
    return m_workspace;
}
