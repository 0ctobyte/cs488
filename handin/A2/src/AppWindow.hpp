#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
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
    void setMode(QAction *act);
    void resetView();
    void updateInfoLabel(); 

private:
    void createActions();
    void createMenu();

    // Mode Actions
    QAction* viewModeTranslateAct;
    QAction* viewModeRotateAct;
    QAction* viewModePerspectiveAct;
    QAction* modelModeTranslateAct;
    QAction* modelModeRotateAct;
    QAction* modelModeScaleAct;
    QAction* viewportModeAct;

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_mode;

    std::vector<QAction*> m_menu_app_actions;
    std::vector<QAction*> m_menu_mode_actions;
    Viewer* m_viewer;
    QLabel* m_infoLabel;
};

#endif
