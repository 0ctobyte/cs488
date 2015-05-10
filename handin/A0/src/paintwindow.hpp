#ifndef PAINTWINDOW_HPP
#define PAINTWINDOW_HPP

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include "paintcanvas.hpp"

// class PaintCanvas;

class PaintWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum HelpOption {
      HELP_LINE,
      HELP_OVAL,
      HELP_RECTANGLE
    };

    PaintWindow();
    virtual ~PaintWindow() {}
    
protected:

private slots:
    // Display a message box providing assistance with the shape drawing
    void help(QAction *action);

    void set_line();
    void set_rect();
    void set_oval();
    void set_fill_colour(QAction *action);
    void clear_canvas();

private:

    void createMenu();

    // Note: QMainWindow has its own QMenuBar where as QWidget does not
    // Each drop down menus
    QMenu* m_menu_app;
    QMenu* m_menu_tools;
    QMenu* m_menu_colour;
    QMenu* m_menu_help;

    // The canvas onto which the user draws.
    PaintCanvas* m_canvas;

    // The quit button
    QPushButton* m_quit;
};

#endif
