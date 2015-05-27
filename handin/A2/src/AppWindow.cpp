#include <QtWidgets>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Assignment Two");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    // m_menubar = new QMenuBar;
    m_viewer = new Viewer(glFormat, this);
    m_infoLabel = new QLabel(this);
    layout->addWidget(m_viewer);
    layout->addWidget(m_infoLabel);
    layout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createActions();
    createMenu();

    // Set the font size, alignment to center, and fixed height for label
    QFont font;
    font.setPointSize(11);
    m_infoLabel->setAlignment(Qt::AlignHCenter);
    m_infoLabel->setFont(font);
    m_infoLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // Update info label 30 times a second
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateInfoLabel()));
    timer->start(1000/30);
}

void AppWindow::keyPressEvent(QKeyEvent *event) {
  int key = event->key();

  switch(key) {
  case Qt::Key_Q:
    close();
    break;
  case Qt::Key_A:
    m_viewer->reset_view();
  case Qt::Key_R:
    setMode(modelModeRotateAct);
    modelModeRotateAct->toggle();
    break;
  case Qt::Key_T:
    setMode(modelModeTranslateAct);
    modelModeTranslateAct->toggle();
    break;
  case Qt::Key_S:
    setMode(modelModeScaleAct);
    modelModeScaleAct->toggle();
    break;
  case Qt::Key_O:
    setMode(viewModeRotateAct);
    viewModeRotateAct->toggle();
    break;
  case Qt::Key_N:
    setMode(viewModeTranslateAct);
    viewModeTranslateAct->toggle();
    break;
  case Qt::Key_P:
    setMode(viewModePerspectiveAct);
    viewModePerspectiveAct->toggle();
    break;
  case Qt::Key_V:
    setMode(viewportModeAct);
    viewportModeAct->toggle();
    break;
  default:
    QWidget::keyPressEvent(event);
    break;
  }
}

void AppWindow::createActions() {
    // Creates a new action for quiting and pushes it onto the menu actions vector 
    QAction* quitAct = new QAction(tr("&Quit"), this);
    QAction* resetAct = new QAction(tr("&Reset"), this);

    QActionGroup* modeActions = new QActionGroup(this);
    modeActions->setExclusive(true);

    viewModeTranslateAct = new QAction(tr("&Translate View"), this);
    viewModeTranslateAct->setCheckable(true);
    viewModeTranslateAct->setData(Viewer::Mode::V_TRANSLATE);

    viewModeRotateAct = new QAction(tr("&Rotate View"), this);
    viewModeRotateAct->setCheckable(true);
    viewModeRotateAct->setData(Viewer::Mode::V_ROTATE);

    viewModePerspectiveAct = new QAction(tr("&Set Perspective"), this);
    viewModePerspectiveAct->setCheckable(true);
    viewModePerspectiveAct->setData(Viewer::Mode::V_PERSPECTIVE);

    modelModeTranslateAct = new QAction(tr("&Translate Model"), this);
    modelModeTranslateAct->setCheckable(true);
    modelModeTranslateAct->setData(Viewer::Mode::M_TRANSLATE);

    modelModeRotateAct = new QAction(tr("&Rotate Model"), this);
    modelModeRotateAct->setCheckable(true);
    modelModeRotateAct->setChecked(true);
    modelModeRotateAct->setData(Viewer::Mode::M_ROTATE);

    modelModeScaleAct = new QAction(tr("&Scale Model"), this);
    modelModeScaleAct->setCheckable(true);
    modelModeScaleAct->setData(Viewer::Mode::M_SCALE);

    viewportModeAct = new QAction(tr("&Viewport mode"), this);
    viewportModeAct->setCheckable(true);
    viewportModeAct->setData(Viewer::Mode::VIEWPORT_MODE);

    // We set the accelerator keys
    // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P); 
    resetAct->setShortcut(QKeySequence(Qt::Key_A));
    quitAct->setShortcuts(QKeySequence::Quit);

    viewModeRotateAct->setShortcut(QKeySequence(Qt::Key_O));
    viewModeTranslateAct->setShortcut(QKeySequence(Qt::Key_N));
    viewModePerspectiveAct->setShortcut(QKeySequence(Qt::Key_P));

    modelModeRotateAct->setShortcut(QKeySequence(Qt::Key_R));
    modelModeTranslateAct->setShortcut(QKeySequence(Qt::Key_T));
    modelModeScaleAct->setShortcut(QKeySequence(Qt::Key_S));

    viewportModeAct->setShortcut(QKeySequence(Qt::Key_V));

    // Set the tip
    resetAct->setStatusTip(tr("Resets the view and model transformations to their defaults"));
    quitAct->setStatusTip(tr("Exits the file"));

    viewModeTranslateAct->setStatusTip(tr("Translate the camera"));
    viewModeRotateAct->setStatusTip(tr("Rotate the camera"));
    viewModePerspectiveAct->setStatusTip(tr("Change the vertical field of view, near plane and far plane"));

    modelModeTranslateAct->setStatusTip(tr("Translate the box"));
    modelModeRotateAct->setStatusTip(tr("Rotate the box"));
    modelModeScaleAct->setStatusTip(tr("Scale the box"));

    viewportModeAct->setStatusTip(tr("Adjust the viewport"));

    // Connect the action with the signal and slot designated
    connect(resetAct, SIGNAL(triggered()), this, SLOT(resetView()));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Add the actions to the list
    m_menu_app_actions.push_back(resetAct);
    m_menu_app_actions.push_back(quitAct);

    m_menu_mode_actions.push_back(modeActions->addAction(viewModeTranslateAct));
    m_menu_mode_actions.push_back(modeActions->addAction(viewModeRotateAct));
    m_menu_mode_actions.push_back(modeActions->addAction(viewModePerspectiveAct));
    m_menu_mode_actions.push_back(modeActions->addAction(modelModeTranslateAct));
    m_menu_mode_actions.push_back(modeActions->addAction(modelModeRotateAct));
    m_menu_mode_actions.push_back(modeActions->addAction(modelModeScaleAct));
    m_menu_mode_actions.push_back(modeActions->addAction(viewportModeAct));
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_mode = menuBar()->addMenu(tr("&Mode"));

    for (auto& action : m_menu_app_actions) {
        m_menu_app->addAction(action);
    }

    for(auto& action : m_menu_mode_actions) {
      m_menu_mode->addAction(action);
    }

    connect(m_menu_mode, SIGNAL(triggered(QAction*)), this, SLOT(setMode(QAction*)));
}

void AppWindow::setMode(QAction* act) {
  m_viewer->set_mode((Viewer::Mode)act->data().toInt());
}

void AppWindow::resetView() {
  m_viewer->set_mode(Viewer::Mode::M_ROTATE);
  modelModeRotateAct->toggle();
  m_viewer->reset_view();
}

void AppWindow::updateInfoLabel() {
  Viewer::Mode mode;
  float fovy, znear, zfar;

  m_viewer->get_parameters(&mode, &fovy, &znear, &zfar);

  QString modestr;
  switch(mode) {
  case Viewer::Mode::M_ROTATE:
    modestr = QString("model_rotate");
    break;
  case Viewer::Mode::M_TRANSLATE:
    modestr = QString("model_translate");
    break;
  case Viewer::Mode::M_SCALE:
    modestr = QString("model_scale");
    break;
  case Viewer::Mode::V_ROTATE:
    modestr = QString("view_rotate");
    break;
  case Viewer::Mode::V_TRANSLATE:
    modestr = QString("view_translate");
    break;
  case Viewer::Mode::V_PERSPECTIVE:
    modestr = QString("view_perspective");
    break;
  case Viewer::Mode::VIEWPORT_MODE:
    modestr = QString("viewport_mode");
    break;
  }

  QString info = QString("mode: %1 | fov: %2 | znear: %3 | zfar: %4").arg(modestr).arg(fovy).arg(znear).arg(zfar);
  
  m_infoLabel->setText(info);
}      
        
