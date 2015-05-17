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

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
  void newGame();
  void setDrawMode(QAction* act);
  void setSpeedMode(QAction* act);

private:
    void createActions();
    void createMenu();

    // Draw action group actions
    QAction* wireFrameModeAct;
    QAction* faceModeAct;
    QAction* multiColModeAct;

    // Speed action group actions
    QAction* slowSpeedAct;
    QAction* mediumSpeedAct;
    QAction* fastSpeedAct;

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_draw;
    QMenu* m_menu_speed;

    std::vector<QAction*> m_menu_app_actions;
    std::vector<QAction*> m_menu_draw_actions;
    std::vector<QAction*> m_menu_speed_actions;

    Viewer* m_viewer;
};

#endif
