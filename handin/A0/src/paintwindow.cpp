#include <QtWidgets>
#include <iostream>
#include "paintwindow.hpp"

PaintWindow::PaintWindow() {
    setWindowTitle("488 Paint");

    QVBoxLayout *layout = new QVBoxLayout;

    m_canvas = new PaintCanvas(this);
    layout->addWidget(m_canvas);

    m_quit = new QPushButton("Quit", this);
    layout->addWidget(m_quit);
    connect(m_quit, SIGNAL(clicked()), this, SLOT(close()));

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createMenu();
}

void PaintWindow::createMenu() {
    // Adding the drop down menu to the menubar
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_tools = menuBar()->addMenu(tr("&Tools"));
    m_menu_colour = menuBar()->addMenu(tr("&Colour"));
    m_menu_help = menuBar()->addMenu(tr("&Help"));

    // Adding the menu items for each drop down menu
    QAction* quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Exits the program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    
    QAction *clearAct = new QAction(tr("&Clear"), this);
    clearAct->setShortcut(QKeySequence(tr("c", "Clear")));
    clearAct->setStatusTip(tr("Clears the canvas"));
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear_canvas()));
    
    m_menu_app->addAction(clearAct);
    m_menu_app->addAction(quitAct);

    QActionGroup* toolsActions = new QActionGroup(this);
    toolsActions->setExclusive(true);

    QAction* drawLineAct = new QAction(tr("&Line"), this);
    drawLineAct->setShortcut(QKeySequence(tr("l", "Line mode")));
    drawLineAct->setStatusTip(tr("Draws a line"));
    drawLineAct->setCheckable(true);
    drawLineAct->setChecked(true);
    connect(drawLineAct, SIGNAL(triggered()), this, SLOT(set_line()));

    QAction* drawOvalAct = new QAction(tr("&Oval"), this);
    drawOvalAct->setShortcut(QKeySequence(tr("o", "Oval mode")));
    drawOvalAct->setStatusTip(tr("Draws an Oval"));
    drawOvalAct->setCheckable(true);
    connect(drawOvalAct, SIGNAL(triggered()), this, SLOT(set_oval()));

    QAction* drawRectangleAct = new QAction(tr("&Rectangle"), this);
    drawRectangleAct->setShortcut(QKeySequence(tr("r", "Rectangle mode")));
    drawRectangleAct->setStatusTip(tr("Draws a rectangle"));
    drawRectangleAct->setCheckable(true);
    connect(drawRectangleAct, SIGNAL(triggered()), this, SLOT(set_rect()));

    m_menu_tools->addAction(toolsActions->addAction(drawLineAct));
    m_menu_tools->addAction(toolsActions->addAction(drawOvalAct));
    m_menu_tools->addAction(toolsActions->addAction(drawRectangleAct));

    QAction* setColourBlackAct = new QAction(tr("&Black"), this);
    setColourBlackAct->setStatusTip(tr("Set fill colour to black"));
    setColourBlackAct->setData(PaintCanvas::FILL_BLACK);

    QAction* setColourRedAct = new QAction(tr("&Red"), this);
    setColourRedAct->setStatusTip(tr("Set fill colour to red"));
    setColourRedAct->setData(PaintCanvas::FILL_RED);

    QAction* setColourGreenAct = new QAction(tr("&Green"), this);
    setColourGreenAct->setStatusTip(tr("Set fill colour to green"));
    setColourGreenAct->setData(PaintCanvas::FILL_GREEN);

    QAction* setColourBlueAct = new QAction(tr("&Blue"), this);
    setColourBlueAct->setStatusTip(tr("Set fill colour to blue"));
    setColourBlueAct->setData(PaintCanvas::FILL_BLUE);

    m_menu_colour->addAction(setColourBlackAct);
    m_menu_colour->addAction(setColourRedAct);
    m_menu_colour->addAction(setColourGreenAct);
    m_menu_colour->addAction(setColourBlueAct);
    connect(m_menu_colour, SIGNAL(triggered(QAction*)), this, SLOT(set_fill_colour(QAction*)));

    QAction* helpLineAct = new QAction(tr("&Line Help"), this);
    helpLineAct->setStatusTip(tr("Help Instructions"));
    helpLineAct->setData(HELP_LINE);

    QAction* helpOvalAct = new QAction(tr("&Oval Help"), this);
    helpOvalAct->setStatusTip(tr("Help Instructions"));
    helpOvalAct->setData(HELP_OVAL);

    QAction* helpRectangleAct = new QAction(tr("&Rectangle Help"), this);
    helpRectangleAct->setStatusTip(tr("Help Instructions"));
    helpRectangleAct->setData(HELP_RECTANGLE);

    m_menu_help->addAction(helpLineAct);
    m_menu_help->addAction(helpOvalAct);
    m_menu_help->addAction(helpRectangleAct);
    connect(m_menu_help, SIGNAL(triggered(QAction*)), this, SLOT(help(QAction*)));
}

void PaintWindow::help(QAction *action) {
    QMessageBox msgBox;
    
    switch(action->data().toInt()) {
    case HELP_LINE:
      {
        const char* message =
        "Drawing a Line\n"
        "\n"
        "To draw a line, press the left mouse button to mark the beginning of the line.  Drag the mouse to the end of the line and release the button.";
        msgBox.setText(QString(message));
        break;
      }
    case HELP_OVAL:
      {
        const char* message =
        "Drawing a Oval\n"
        "\n"
        "To draw a oval, press the left mouse button to mark a corner of the oval.  Drag the mouse and release the button to set the oval.";
        msgBox.setText(QString(message));
        break;
      }
    case HELP_RECTANGLE:
      {
        const char* message =
        "Drawing a Rectangle\n"
        "\n"
        "To draw a rectangle, press the left mouse button to mark a corner of the rectangle.  Drag the mouse and release the button to set the rectangle.";
        msgBox.setText(QString(message));
        break;
      }
    }
    msgBox.exec();
}

void PaintWindow::set_line() {
    m_canvas->set_mode(PaintCanvas::DRAW_LINE);
}

void PaintWindow::set_oval() {
    m_canvas->set_mode(PaintCanvas::DRAW_OVAL);
}

void PaintWindow::set_rect() {
    m_canvas->set_mode(PaintCanvas::DRAW_RECTANGLE);
}

void PaintWindow::set_fill_colour(QAction *action) {
    m_canvas->set_fill_colour((PaintCanvas::FillColour)action->data().toInt());
}

void PaintWindow::clear_canvas() {
  m_canvas->clear();
}

