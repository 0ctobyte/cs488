#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>
#include <vector>
#include <stdint.h>
#include "scene_lua.hpp"

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    bool load_scene(std::string filename);
    void pushMatrix() { mMatrixStack.push_back((mMatrixStack.size() > 0) ? mMatrixStack.back() : QMatrix4x4()); }
    void multMatrix(QMatrix4x4 m) { mMatrixStack[mMatrixStack.size()-1] = mMatrixStack.back() * m; }
    void popMatrix() { mMatrixStack.pop_back(); }
    void setLighting(QColor diffuse, QColor specular, float shininess, bool enable_lighting) {
      mProgram.setUniformValue("material.diffuse", diffuse.redF(), diffuse.greenF(), diffuse.blueF());
      mProgram.setUniformValue("material.specular", specular.redF(), specular.greenF(), specular.blueF());
      mProgram.setUniformValue("material.shininess", shininess);
      mProgram.setUniformValue("lighting_enabled", enable_lighting);
    }
    void drawSphere();
    bool picker();

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.
 
    enum Mode {
      TRANSFORM,
      JOINTS,
    };

public slots:
    void resetPosition() { mTransformMatrix.setToIdentity(); }
    void resetOrientation() { mRotationMatrix.setToIdentity(); }
    void resetJoints() { m_sceneRoot->clear_joint_rotation(); }
    void resetAll() { resetPosition(); resetOrientation(); resetJoints(); }
    void setMode(Mode mode) { mMode = mode; }
    void undoTransform() { m_sceneRoot->undo_joint_rotation(); }
    void redoTransform() { m_sceneRoot->redo_joint_rotation(); }

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
    QMatrix4x4 getCameraMatrix();
    QVector3D virtual_trackball(const QVector2D& P, const QVector2D& Q);
    void generate_sphere(int detailLevel);

    GLuint mVAO;
    GLuint mVBO[VBO::SPHERE+1];
    GLuint mIBO;

    std::vector<float> mSphereVertices;
    std::vector<uint32_t> mSphereIndices;
    
    int mMvpMatrixLocation;
    int mModelViewMatrixLocation;
    int mNormalModelViewMatrixLocation;
    int mDiffuseColorLocation;
    int mSpecularColorLocation;
    int mShininessLocation;
    int mCameraPositionLocation;
    int mLightSourcePositionLocation;
    int mLightSourceIntensityLocation;
    int mLightingEnabledLocation;

    QVector3D mCameraPosition;

    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mTransformMatrix;
    QGLShaderProgram mProgram;

    SceneNode* m_sceneRoot;
    Mode mMode;
    QVector2D mMouseCoord;
    std::vector<QMatrix4x4> mMatrixStack;
    QMatrix4x4 mRotationMatrix;
};

#endif
