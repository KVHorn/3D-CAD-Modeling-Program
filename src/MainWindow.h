#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

private:
  void createMenus();

  // Sketcher Workspace Toolbars
  void sketcherWorkspaceLineDrawingToolBar();
  void sketcherWorkspaceCircleToolBar();
  void sketcherWorkspacConstraintsToolBar();

  void createStatusBar();
  void setWorkspace();
};