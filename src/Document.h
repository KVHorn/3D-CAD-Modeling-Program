#pragma once
#include "QString"
// #include "PartDesign.h"

class OcctViewport; // Forward declaration - only hold a point to one.

enum class WorkspaceType
{
    PartDesign,
    Sketcher,
    Assembly
};

class Document
/*
    Note:
    - Everything within public is callable from other code (doc->viewport())
    - Everything within private is accessible only from inside the class's own functions.
    Outside code interacts with the Document through a controled set of funcitons (the public interface), never by reaching directly into its data.
*/
{
public:
    explicit Document(const QString &name); // The explicit workspace subclass name

    QString name() const;
    OcctViewport *viewport() const; // The 3d view this obect renders into.
    WorkspaceType currentWorkspace() const;
    void setCurrentWorkspace(WorkspaceType type);

    /*
    Later: The model itself (part, planes, sketches, feature tree), filename, "modified since save" flag, etc.
    */

private:
    QString m_name;

    /*
    This is a pointer to a viewport. The * means he doc doesnt contain the viewport by value, it points to one that lives elsewhere.
    - The m_ prefix is a naming convention maring it as a member variable -> done to make it easier to tell it apart from local variables and parameters at a glance.
    */
    OcctViewport *m_viewport = nullptr;

    /*
    This is a member with an in-class default initializer.
    - The initial WorkspaceType will always be the PartDesign Workspace.
    "= WorkspaceType::PartDesign" means the memebr starts as PartDesign unless teh constructor explicitly sets it otherwise.
    Note: The default "view" or default Workspace will always be the PartDesign Workspace.
    */
    WorkspaceType m_workspace = WorkspaceType::PartDesign;
};