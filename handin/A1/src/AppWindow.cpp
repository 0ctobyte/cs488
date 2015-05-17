#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Tetrominoes on the Wall");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    // m_menubar = new QMenuBar;
    m_viewer = new Viewer(glFormat, this);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
    m_viewer->show();

    createActions();
    createMenu();
}

void AppWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();

    switch(key) {
    case Qt::Key_Escape:
      QCoreApplication::instance()->quit();
      break;
    case Qt::Key_Q:
      close();
      break;
    case Qt::Key_R:
      m_viewer->resetView();
      break;
    case Qt::Key_N:
      newGame();
      break;
    case Qt::Key_W:
      // TODO: uppercase?
      wireFrameModeAct->toggle();
      break;
    case Qt::Key_F:
      faceModeAct->toggle();
      break;
    case Qt::Key_M:
      multiColModeAct->toggle();
      break;
    case Qt::Key_Left:
      m_viewer->game().moveLeft();
      break;
    case Qt::Key_Right:
      m_viewer->game().moveRight();
      break;
    case Qt::Key_Up:
      m_viewer->game().rotateCCW();
      break;
    case Qt::Key_Down:
      m_viewer->game().rotateCW();
      break;
    case Qt::Key_Space:
      m_viewer->game().drop();
      break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void AppWindow::createActions() {
    // Creates a new action for quiting and pushes it onto the menu actions vector 
    QAction* newGameAct = new QAction(tr("&New game"), this);
    QAction* resetAct = new QAction(tr("&Reset"), this);
    QAction* quitAct = new QAction(tr("&Quit"), this);

    QActionGroup* drawActions = new QActionGroup(this);
    drawActions->setExclusive(true);
    wireFrameModeAct = new QAction(tr("&Wire-frame"), this);
    wireFrameModeAct->setCheckable(true);
    wireFrameModeAct->setData(Viewer::DrawMode::WIREFRAME);
    faceModeAct = new QAction(tr("&Face"), this);
    faceModeAct->setCheckable(true);
    faceModeAct->setChecked(true);
    faceModeAct->setData(Viewer::DrawMode::FACE);
    multiColModeAct = new QAction(tr("&Multicoloured"), this);
    multiColModeAct->setCheckable(true);
    multiColModeAct->setData(Viewer::DrawMode::MULTICOLOURED);

    QActionGroup* speedActions = new QActionGroup(this);
    speedActions->setExclusive(true);
    slowSpeedAct = new QAction(tr("&Slow"), this);
    slowSpeedAct->setCheckable(true);
    slowSpeedAct->setChecked(true);
    slowSpeedAct->setData(Viewer::SpeedMode::SLOW);
    mediumSpeedAct = new QAction(tr("&Medium"), this);
    mediumSpeedAct->setCheckable(true);
    mediumSpeedAct->setData(Viewer::SpeedMode::MEDIUM);
    fastSpeedAct = new QAction(tr("&Fast"), this);
    fastSpeedAct->setCheckable(true);
    fastSpeedAct->setData(Viewer::SpeedMode::FAST);

    // We set the accelerator keys
    // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P); 
    newGameAct->setShortcut(QKeySequence(Qt::Key_N));
    resetAct->setShortcut(QKeySequence(Qt::Key_R));
    quitAct->setShortcuts(QKeySequence::Quit);

    wireFrameModeAct->setShortcut(QKeySequence(Qt::Key_W));
    faceModeAct->setShortcut(QKeySequence(Qt::Key_F));
    multiColModeAct->setShortcut(QKeySequence(Qt::Key_M));

    slowSpeedAct->setShortcut(QKeySequence(Qt::Key_1));
    mediumSpeedAct->setShortcut(QKeySequence(Qt::Key_2));
    fastSpeedAct->setShortcut(QKeySequence(Qt::Key_3));

    // Set the tip
    newGameAct->setStatusTip(tr("Starts a new game"));
    resetAct->setStatusTip(tr("Resets the view of the game"));
    quitAct->setStatusTip(tr("Exits the file"));

    wireFrameModeAct->setStatusTip(tr("Renders the scene in wire-frame mode"));
    faceModeAct->setStatusTip(tr("Renders each piece with a distinct colour"));
    multiColModeAct->setStatusTip(tr("Renders each face of each cube with a distinct colour"));

    slowSpeedAct->setStatusTip(tr("The pieces drop at a slow pace"));
    mediumSpeedAct->setStatusTip(tr("The pieces drop at a medium pace"));
    fastSpeedAct->setStatusTip(tr("The pieces drop at a fast pace"));

    // Connect the action with the signal and slot designated
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(resetAct, SIGNAL(triggered()), m_viewer, SLOT(resetView()));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Add the actions to the appropriate list
    m_menu_app_actions.push_back(newGameAct);
    m_menu_app_actions.push_back(resetAct);
    m_menu_app_actions.push_back(quitAct);

    m_menu_draw_actions.push_back(drawActions->addAction(wireFrameModeAct));
    m_menu_draw_actions.push_back(drawActions->addAction(faceModeAct));
    m_menu_draw_actions.push_back(drawActions->addAction(multiColModeAct));

    m_menu_speed_actions.push_back(speedActions->addAction(slowSpeedAct));
    m_menu_speed_actions.push_back(speedActions->addAction(mediumSpeedAct));
    m_menu_speed_actions.push_back(speedActions->addAction(fastSpeedAct));
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_draw = menuBar()->addMenu(tr("&Draw Mode"));
    m_menu_speed = menuBar()->addMenu(tr("&Speed"));

    for (auto& action : m_menu_app_actions) {
        m_menu_app->addAction(action);
    }

    for(auto& action : m_menu_draw_actions) {
      m_menu_draw->addAction(action);
    }

    connect(m_menu_draw, SIGNAL(triggered(QAction*)), this, SLOT(setDrawMode(QAction*))); 

    for(auto& action : m_menu_speed_actions) {
      m_menu_speed->addAction(action);
    }

    connect(m_menu_speed, SIGNAL(triggered(QAction*)), this, SLOT(setSpeedMode(QAction*)));
}

void AppWindow::newGame() {
  m_viewer->resetView();
  m_viewer->game().reset();
}

void AppWindow::setDrawMode(QAction* act) {
  m_viewer->drawMode((Viewer::DrawMode)act->data().toInt());
}

void AppWindow::setSpeedMode(QAction* act) {
  m_viewer->speedMode((Viewer::SpeedMode)act->data().toInt());
}

