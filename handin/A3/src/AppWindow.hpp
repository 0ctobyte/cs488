#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <vector>
#include "Viewer.hpp"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow();
    AppWindow(int argc, char** argv);

private slots:
    void setMode(QAction* action) { m_viewer->setMode((Viewer::Mode)action->data().toInt()); }

private:
    void createActions();
    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_mode;
    QMenu* m_menu_edit;

    std::vector<QAction*> m_menu_app_actions;
    std::vector<QAction*> m_menu_mode_actions;
    std::vector<QAction*> m_menu_edit_actions;
    Viewer* m_viewer;
};

#endif
