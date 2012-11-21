#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QCoreApplication>
#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QTextEdit>

#include <QDebug>

#include "graphic_editor/gl_base.h"
#include "project_navigator/project_navigator.h"
#include "property_browser/property_browser.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

  // Draw actions
  QActionGroup *draw_group;
  QAction *selection_action;
  QAction *circle_action;
  QAction *rect_action;

  // Sim actions
  QAction *time_action;
  QAction *play_action;
  QAction *pause_action;
  QAction *stop_action;

  // Display primary tab
  void MaximizeEditor();

  // Property browser
  PropertyBrowser *property_browser;

protected:
  void KeyPressEvent(QKeyEvent *event);
  void CloseEvent(QCloseEvent *event);

private:

  void CreateFileActions();
  void CreateEditActions();
  void CreateViewActions();
  void CreateDrawActions();
  void CreateSimActions();
  void CreateHelpActions();
  void CreateMenus();
  void CreateToolBars();
  void CreateStatusBar();

  // Editor maximized?
  bool editor_maximized;

  // Create windows
  QDockWidget *navigator_widget;
  QDockWidget *console_widget;
  QDockWidget *browser_widget;
  GLBase *gl_base;
  ProjectNavigator *project_navigator;
  QTextEdit *console;

  // Menus
  QMenu *file_menu;
  QMenu *edit_menu;
  QMenu *view_menu;
  QMenu *draw_menu;
  QMenu *sim_menu;
  QMenu *git_menu;
  QMenu *script_menu;
  QMenu *help_menu;

  // Toolbars
  QToolBar *file_bar;
  QToolBar *edit_bar;
  QToolBar *view_bar;
  QToolBar *sim_bar;
  QToolBar *draw_bar;
  QToolBar *help_bar;

  // File actions
  QAction *new_file_action;
  QAction *open_file_action;
  QAction *save_file_action;
  QAction *save_as_action;
  QAction *print_action;
  QAction *exit_action;

  // Edit actions
  QAction *cut_action;
  QAction *copy_action;
  QAction *paste_action;

  // View actions
  QAction *zoom_in_action;
  QAction *zoom_out_action;

  // Help
  QAction *about_action;  
  
  // Current file
  QString current_file;
  QString image_dir;

private slots:

// File menu slots
  void NewFile();
  void Open();
  bool Save();
  bool SaveAs();
  void Print();

// Help menu slots
  void About();
};
#endif // MAIN_WINDOW_H
