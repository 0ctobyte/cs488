#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>

#include <vector>
#include <cstdint>

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.

    // *** Fill in these functions (in viewer.cpp) ***

    enum Mode {
      M_TRANSLATE,
      M_ROTATE,
      M_SCALE,
      V_TRANSLATE,
      V_ROTATE,
      V_PERSPECTIVE
    };


    // Set the parameters of the current perspective projection using
    // the semantics of gluPerspective().
    void set_perspective(double fov, double aspect, double near, double far);

    // Restore all the transforms and perspective parameters to their
    // original state. Set the viewport to its initial size.
    void reset_view();

    // Set the control mode
    void set_mode(Mode mode) { m_Mode = mode; }

protected:

    // Events we implement

    // Called when GL is first initialized
    virtual void initializeGL();
    // Called when our window needs to be redrawn
    virtual void paintGL();
    // Called when the window is resized
    virtual void resizeGL(int width, int height);
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    // Called when the mouse moves
    virtual void mouseMoveEvent ( QMouseEvent * event );
 
    // Draw a line -- call draw_init first!
    void draw_line(const QVector2D& p1, const QVector2D& p2) ;

    // Set the current colour
    void set_colour(const QColor& col);

    // Call this before you begin drawing. Width and height are the width
    // and height of the GL window.
    void draw_init();
private:
    void drawArrays(QVector3D *points, size_t num, QMatrix4x4 transform);
    bool clipLine(QVector4D& P, QVector4D& Q);

    Mode m_Mode;

    GLuint mVAO;
    GLuint mVBO;

    QGLShaderProgram mProgram;

    int mColorLocation;
    
    // *** Fill me in ***
    // You will want to declare some more matrices here
    QMatrix4x4 m_Projection;    
    QMatrix4x4 m_View;
    QMatrix4x4 m_Model;
    QMatrix4x4 m_ModelScale;

    // Model Gnomon points
    QVector3D m_ModelGnomon[6];
    QVector3D m_WorldGnomon[6];
    QVector3D m_Box[24];

    QTimer *m_Timer;
    QVector2D m_MouseCoord;
    QVector3D m_CamPos;

    float m_zNear;
    float m_zFar;
};

#endif
