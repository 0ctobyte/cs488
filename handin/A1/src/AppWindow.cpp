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
    if (event->key() == Qt::Key_Escape) {
        QCoreApplication::instance()->quit();
    } else if(event->key() == Qt::Key_Q) {
      close();
    } else if (event->key() == Qt::Key_T) {
        std::cerr << "Hello!" << std::endl;
    } else if(event->key() == Qt::Key_R) {
      m_viewer->resetView();
    } else if(event->key() == Qt::Key_N) {
      newGame();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void AppWindow::createActions() {
    // Creates a new action for quiting and pushes it onto the menu actions vector 
    QAction* newGameAct = new QAction(tr("&New game"), this);
    QAction* resetAct = new QAction(tr("&Reset"), this);
    QAction* quitAct = new QAction(tr("&Quit"), this);
    m_menu_actions.push_back(newGameAct);
    m_menu_actions.push_back(resetAct);
    m_menu_actions.push_back(quitAct);

    // We set the accelerator keys
    // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P); 
    newGameAct->setShortcut(QKeySequence(Qt::Key_N));
    resetAct->setShortcut(QKeySequence(Qt::Key_R));
    quitAct->setShortcuts(QKeySequence::Quit);

    // Set the tip
    newGameAct->setStatusTip(tr("Starts a new game"));
    resetAct->setStatusTip(tr("Resets the view of the game"));
    quitAct->setStatusTip(tr("Exits the file"));

    // Connect the action with the signal and slot designated
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(resetAct, SIGNAL(triggered()), m_viewer, SLOT(resetView()));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));

    for (auto& action : m_menu_actions) {
        m_menu_app->addAction(action);
    }
}

void AppWindow::newGame() {
  m_viewer->game().reset();
}

