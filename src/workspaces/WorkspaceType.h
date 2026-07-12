#ifndef WORKSPACETYPE_H
#define WORKSPACETYPE_H

// Which "crafter's desk" tool set a Document is currently using.
// Lives in its own header so Document, Workspace, and MainWindow can all
// include it without circular-include headaches.
enum class WorkspaceType
{
    PartDesign,
    Sketcher
};

#endif // WORKSPACETYPE_H
