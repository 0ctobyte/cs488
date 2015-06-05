#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>
#include "scene_lua.hpp"

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    bool load_scene(std::string filename);
    QGLShaderProgram& getShaderProgram() { return mProgram; }
    QMatrix4x4 getProjectionMatrix() { return mPerspMatrix; }
    QMatrix4x4 getCameraMatrix();
    void drawSphere();

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.
 
    enum Mode {
      TRANSFORM,
      JOINTS,
    };

public slots:
    void resetPosition() { mTransformMatrix.setToIdentity(); }
    void resetOrientation() { m_sceneRoot->set_transform(QMatrix4x4()); }
    void resetJoints() {}
    void resetAll() { resetPosition(); resetOrientation(); resetJoints(); }
    void setMode(Mode mode) { mMode = mode; }
    void undoTransform() {}
    void redoTransform() {}

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
    
    // Draw a circle for the trackball, with OpenGL commands.
    // Assumes the context for the viewer is active.
    void draw_trackball_circle();

private:

    enum VBO {
      CIRCLE,
      SPHERE,
    };

    void translateWorld(float x, float y, float z);
    void rotateWorld(float x, float y, float z);
    void scaleWorld(float x, float y, float z);
    void set_colour(const QColor& col);
    void generate_sphere(int detailLevel);

    GLuint mVAO;
    GLuint mVBO[VBO::SPHERE+1];
    GLuint mIBO;

    int mIndexCount;
    int mVertexCount;
    
    int mMvpMatrixLocation;
    int mDiffuseColorLocation;

    QVector3D mCameraPosition;

    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mTransformMatrix;
    QGLShaderProgram mProgram;

    SceneNode* m_sceneRoot;
    Mode mMode;
    QVector2D mMouseCoord;
};

#endif
