#ifndef PARTDESIGNWORKSPACE_H
#define PARTDESIGNWORKSPACE_H

#include "workspaces/Workspace.h"

// The Part Design workbench: primitives and dress-ups now; sketch-based
// features (Pad, Pocket, Shaft, Hole) arrive with the sketcher phases.
class PartDesignWorkspace : public Workspace
{
    Q_OBJECT

public:
    explicit PartDesignWorkspace(QMainWindow* mainWindow);

    WorkspaceType type() const override { return WorkspaceType::PartDesign; }

private slots:
    void onCreateFeature(const QString& typeName);
};

#endif // PARTDESIGNWORKSPACE_H
