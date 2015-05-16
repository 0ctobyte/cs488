#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else 
#include <QGLBuffer>
#endif

#include "game.hpp"

class Viewer : public QGLWidget {
    
    Q_OBJECT

public slots:
  void resetView();

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // Return a referene to the game instance
    // AppWindow can use this when the left and right keys are pressed 
    // to move the cubes
    Game& game() { return mGame; }

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.
  
protected:

    // Events we implement

    // Called when GL is first initialized
    virtual void initializeGL();
    // Called when our window needs to be redrawn
    virtual void paintGL();
    // Called when the window is resized (formerly on_configure_event)
    virtual void resizeGL(int width, int height);
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    // Called when the mouse moves
    virtual void mouseMoveEvent ( QMouseEvent * event );

private slots:
  void updateGame();

private:
    enum VBO {
      TRIANGLE,
      CUBE,
      MAX_VBO
    };

    // Colour map for the pieces
    float mPieceColour[9][3];
      
    Game mGame;
    QTimer*mTimerGame;
    void drawGameBorder();
    void drawGameBoard();

    QMatrix4x4 getCameraMatrix();
    void translateWorld(float x, float y, float z);
    void rotateWorld(float angle, float x, float y, float z);
    void scaleWorld(float x, float y, float z);

    void drawCube(float x, float y, float z);

    GLuint mVAO;
    GLuint mVBO[MAX_VBO]; 

    int mVertexLocation;
    int mMvpMatrixLocation;
    int mColorLocation;

    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mModelMatrices[4];
    QMatrix4x4 mTransformMatrix;
    QMatrix4x4 mCubeModelMatrix;
    
    QTimer* mTimer;
    QGLShaderProgram mProgram;
};

#endif
