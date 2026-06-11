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
  void createToolBar();
  void createStatusBar();
};