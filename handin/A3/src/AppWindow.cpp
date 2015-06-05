#include <QtWidgets>
#include <string>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Assignment Two");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    m_viewer = new Viewer(glFormat, this);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createActions();
    createMenu();
}

AppWindow::AppWindow(int argc, char** argv) 
  : AppWindow() {
    std::string filename = "puppet.lua";
    if (argc >= 2) {
        filename = argv[1];
    }

    bool ret = m_viewer->load_scene(filename);
    if (!ret) {
        std::cerr << "Could not open " << filename << std::endl;
        close();
    }
}

void AppWindow::createActions() {
    // Creates a new action for quiting and pushes it onto the menu actions vector 
    QAction* resetPositionAct = new QAction(tr("&Reset Position"), this);
    QAction* resetOrientationAct = new QAction(tr("&Reset Orientation"), this);
    QAction* resetJointsAct = new QAction(tr("&Reset Joints"), this);
    QAction* resetAllAct = new QAction(tr("&Reset All"), this);
    QAction* quitAct = new QAction(tr("&Quit"), this);
    
    QActionGroup* modeActions = new QActionGroup(this);
    modeActions->setExclusive(true);
    QAction* positionOrientationAct = new QAction(tr("&Position/Orientation"), this);
    positionOrientationAct->setCheckable(true);
    positionOrientationAct->setChecked(true);
    positionOrientationAct->setData(Viewer::Mode::TRANSFORM);
    QAction* jointsAct = new QAction(tr("&Joints"), this);
    jointsAct->setCheckable(true);
    jointsAct->setData(Viewer::Mode::JOINTS);

    QAction* undoAct = new QAction(tr("&Undo"), this);
    QAction* redoAct = new QAction(tr("&Redo"), this);

    // We set the accelerator keys
    // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P); 
    resetPositionAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_I) << QKeySequence(Qt::SHIFT + Qt::Key_I));
    resetOrientationAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_O) << QKeySequence(Qt::SHIFT + Qt::Key_O));
    resetJointsAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_N) << QKeySequence(Qt::SHIFT + Qt::Key_N));
    resetAllAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_A) << QKeySequence(Qt::SHIFT + Qt::Key_A));
    quitAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_Q) << QKeySequence(Qt::SHIFT + Qt::Key_Q) << QKeySequence::Quit);

    positionOrientationAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_P) << QKeySequence(Qt::SHIFT + Qt::Key_P));
    jointsAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_J) << QKeySequence(Qt::SHIFT + Qt::Key_J));

    undoAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_U) << QKeySequence(Qt::SHIFT + Qt::Key_U));
    redoAct->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_R) << QKeySequence(Qt::SHIFT + Qt::Key_R));

    // Set the tip
    resetPositionAct->setStatusTip(tr("Reset the origin of the puppet to its initial position"));
    resetOrientationAct->setStatusTip(tr("Reset the puppet to its initial orientation"));
    resetJointsAct->setStatusTip(tr("Reset the joint angles and clear the undo/redo stack"));
    resetAllAct->setStatusTip(tr("Reset position, orientation, joint angles and clear the undo/redo stack"));
    quitAct->setStatusTip(tr("Exits the program"));

    positionOrientationAct->setStatusTip(tr("Translate and rotate the entire puppet"));
    jointsAct->setStatusTip(tr("Control joint angles"));

    undoAct->setStatusTip(tr("Undo the previous transformation on the undo/redo stack"));
    redoAct->setStatusTip(tr("Redo the previous transformation on the undo/redo stack"));

    // Connect the action with the signal and slot designated
    connect(resetPositionAct, SIGNAL(triggered()), m_viewer, SLOT(resetPosition()));
    connect(resetOrientationAct, SIGNAL(triggered()), m_viewer, SLOT(resetOrientation()));
    connect(resetJointsAct, SIGNAL(triggered()), m_viewer, SLOT(resetJoints()));
    connect(resetAllAct, SIGNAL(triggered()), m_viewer, SLOT(resetAll()));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    connect(undoAct, SIGNAL(triggered()), m_viewer, SLOT(undoTransform()));
    connect(redoAct, SIGNAL(triggered()), m_viewer, SLOT(redoTransform()));

    // Connect the signals
    m_menu_app_actions.push_back(quitAct);
    m_menu_app_actions.push_back(resetPositionAct);
    m_menu_app_actions.push_back(resetOrientationAct);
    m_menu_app_actions.push_back(resetJointsAct);
    m_menu_app_actions.push_back(resetAllAct);

    m_menu_mode_actions.push_back(modeActions->addAction(positionOrientationAct));
    m_menu_mode_actions.push_back(modeActions->addAction(jointsAct));

    m_menu_edit_actions.push_back(undoAct);
    m_menu_edit_actions.push_back(redoAct);
}

void AppWindow::createMenu() {
    #ifndef __APPLE__
      menuBar()->setNativeMenuBar(false);
    #endif

    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_mode = menuBar()->addMenu(tr("&Mode"));
    m_menu_edit = menuBar()->addMenu(tr("&Edit"));

    for (auto& action : m_menu_app_actions) {
      m_menu_app->addAction(action);
    }

    for(auto& action : m_menu_mode_actions) {
      m_menu_mode->addAction(action);
    }

    for(auto& action : m_menu_edit_actions) {
      m_menu_edit->addAction(action);
    }

    connect(m_menu_mode, SIGNAL(triggered(QAction*)), this, SLOT(setMode(QAction*)));
}

