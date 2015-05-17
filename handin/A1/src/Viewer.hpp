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
  // This is called when the R button is pressed
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

    enum DrawMode {
      WIREFRAME,
      FACE,
      MULTICOLOURED
    };

    enum SpeedMode {
      SLOW,
      MEDIUM,
      FAST
    };

    // These functions change the draw mode and speed of gameplay respectively
    void drawMode(DrawMode mode);
    void speedMode(SpeedMode mode);

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
  // Called on a timer to update the game logic
  void updateGame();

private:
    enum VBO {
      CUBE,
      MAX_VBO
    };

    // Cubes will have different colours for each face if this is true
    bool mMultiColCube;
      
    // Instance of the game and timer to update game logic
    Game mGame;
    QTimer *mTimerGame;

    // These functions draw the game content
    void drawGameBorder();
    void drawGameBoard();

    QMatrix4x4 getCameraMatrix();
    void translateWorld(float x, float y, float z);
    void rotateWorld(float angle, float x, float y, float z);
    void scaleWorld(float x, float y, float z);

    // Draws a unit cube translated to the specified coordinates
    void drawCube(float x, float y, float z);

    // Vertex array and buffer objects
    GLuint mVAO;
    GLuint mVBO[MAX_VBO]; 

    int mVertexLocation;
    int mMvpMatrixLocation;
    int mColorLocation;

    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mTransformMatrix;
    QMatrix4x4 mCubeModelMatrix;
    
    QTimer* mTimer;
    QGLShaderProgram mProgram;

    // These vars are used to keep track of the state of rendering
    QPoint mMouseCoord;
    float mScale;
    QVector3D mRotAngle;
    bool mPersistence;
    bool mMouseMoving;

#ifdef __APPLE__
    // This is needed to correct middle mouse persistence rotation
    float mRotAngleMB[2];
#endif
};

#endif
