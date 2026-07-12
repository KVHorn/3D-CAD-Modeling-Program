#ifndef SKETCHERWORKSPACE_H
#define SKETCHERWORKSPACE_H

#include "Workspace.h"

// 2D sketch tools: geometry entry and, eventually, the constraint solver.
class SketcherWorkspace : public Workspace
{
    Q_OBJECT

public:
    explicit SketcherWorkspace(QMainWindow* mainWindow);

    WorkspaceType type() const override { return WorkspaceType::Sketcher; }
};

#endif // SKETCHERWORKSPACE_H
