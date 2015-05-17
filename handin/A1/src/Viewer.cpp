// #include <GL/glew.h>
#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"
#include <iostream>
#include <math.h>
// #include <GL/gl.h>
#ifndef _APPLE_
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
    , mMultiColCube(false)
    , mGame(10, 20)
    , mVAO(0)
    , mMouseCoord(0, 0)
    , mScale(1.0f)
{
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
    mTimer->start(1000/30);

    mModelMatrices[0].translate(-5,-10,0);
    mModelMatrices[1].translate(5,-10,0);
    mModelMatrices[1].rotate(90, QVector3D(0,0,1));
    mModelMatrices[2].translate(-5,10,0);
    mModelMatrices[2].rotate(270, QVector3D(0,0,1));
    mModelMatrices[3].translate(5,10,0);
    mModelMatrices[3].rotate(180, QVector3D(0,0,1));

    mCubeModelMatrix.setToIdentity();

    mGame.reset();

    mTimerGame = new QTimer(this);
    connect(mTimerGame, SIGNAL(timeout()), this, SLOT(updateGame()));
    mTimerGame->start(1000);
}

Viewer::~Viewer() {

}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(325, 575);
}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.7, 0.7, 1.0, 0.0);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
        std::cerr << "Cannot load vertex shader." << std::endl;
        return;
    }

    if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
        std::cerr << "Cannot load fragment shader." << std::endl;
        return;
    }

    if ( !mProgram.link() ) {
        std::cerr << "Cannot link shaders." << std::endl;
        return;
    }

    float triangleData[] = {
        //  X     Y     Z
         0.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
    };

    float cubeData[] = {
      // back quad
      0.5f, 0.5f, -1.0f,
      -0.5f, 0.5f, -1.0f,
      -0.5f, -0.5f, -1.0f,
      0.5f, 0.5f, -1.0f,
      -0.5, -0.5f, -1.0f,
      0.5f, -0.5f, -1.0f,
      // top quad
      0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, -1.0f,
      0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, -1.0f,
      0.5f, 0.5f, -1.0f,
      // bottom quad
      0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, -1.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, -1.0f,
      0.5f, -0.5f, -1.0f,
      // right quad
      0.5f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, -1.0f,
      0.5f, 0.5f, 0.0f,
      0.5f, -0.5f, -1.0f,
      0.5f, 0.5f, -1.0f,
      // left quad
      -0.5f, 0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, -1.0f,
      -0.5f, 0.5f, 0.0f,
      -0.5f, -0.5f, -1.0f,
      -0.5f, 0.5f, -1.0f,
      // front quad
      0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      0.5f, 0.5f, 0.0f,
      -0.5, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
    };


#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

#else

    /*
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of
     * QOpenGLBuffer.
     */
    typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);

    _glGenVertexArrays glGenVertexArrays;
    _glBindVertexArray glBindVertexArray;

    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

#endif

    glGenBuffers(VBO::MAX_VBO, mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::TRIANGLE]);

    if (mVBO[VBO::TRIANGLE] == 0) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(float), triangleData, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::CUBE]);

    if(mVBO[VBO::CUBE] == 0) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    glBufferData(GL_ARRAY_BUFFER, 36 * 3  * sizeof(float), cubeData, GL_STATIC_DRAW);

    mProgram.bind();

    // mPerspMatrixLocation = mProgram.uniformLocation("cameraMatrix");
    mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
    mColorLocation = mProgram.uniformLocation("cindex");
}

void Viewer::paintGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    glBindVertexArray(mVAO);
#else
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);
    _glBindVertexArray glBindVertexArray;
    glBindVertexArray(mVAO);
#endif

    glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::TRIANGLE]);

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);
    mProgram.setUniformValue(mColorLocation, 9);

    for (int i = 0; i < 4; i++) {

        mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mModelMatrices[i]);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // Draw game well
    drawGameBorder();

    // Draw game board
    drawGameBoard();
}

void Viewer::resizeGL(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    mPerspMatrix.setToIdentity();
    mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " pressed\n";
    std::cerr << "Stub: Press at " << event->x() << ", " << event->y() << std::endl;

    mMouseCoord.setX(event->x());
    mMouseCoord.setY(event->y());
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " released\n";
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
    
    float s = (event->x()-mMouseCoord.x());
    
    if((event->modifiers() & Qt::ShiftModifier) && s != 0) {
      s = 1.0f + s/100.0f;
      mScale *= s;
      if(mScale >= 0.20f && mScale <= 1.20f) scaleWorld(s, s, s);
      else mScale /= s;
    } else if(event->buttons() & Qt::LeftButton) {
      rotateWorld(s, 1.0f, 0.0f, 0.0f);
    } else if(event->buttons() & Qt::MiddleButton) {
      rotateWorld(s, 0.0f, 1.0f, 0.0f);
    } else if(event->buttons() & Qt::RightButton) {
      rotateWorld(s, 0.0f, 0.0f, 1.0f);
    }

    mMouseCoord.setX(event->x());
    mMouseCoord.setY(event->y());
}

QMatrix4x4 Viewer::getCameraMatrix() {
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
    mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float angle, float x, float y, float z) {
    mTransformMatrix.rotate(angle, x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
    mTransformMatrix.scale(x, y, z);
}

void Viewer::resetView() {
  mTransformMatrix.setToIdentity();
}

void Viewer::drawMode(DrawMode mode) {
  switch(mode) {
  case WIREFRAME:
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  case MULTICOLOURED:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mMultiColCube = true;
    break;
  case FACE:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mMultiColCube = false;
    break;
  }
}

void Viewer::speedMode(SpeedMode mode) {
  switch(mode) {
  case SLOW:
    mTimerGame->start(1000/2);
    break;
  case MEDIUM:
    mTimerGame->start(1000/4);
    break;
  case FAST:
    mTimerGame->start(1000/8);
    break;
  }
}

void Viewer::drawCube(float x, float y, float z) {
  glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::CUBE]);

  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);
  
  mCubeModelMatrix.setToIdentity();
  mCubeModelMatrix.translate(x, y, z);
  mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mCubeModelMatrix);

  if(mMultiColCube) {
    for(int i = 0; i < 6; i++) {
      mProgram.setUniformValue(mColorLocation, i);
      glDrawArrays(GL_TRIANGLES, i*6,  6);
    }
  } else {
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

void Viewer::drawGameBorder() {
  mProgram.setUniformValue(mColorLocation, 8);

  // Temporarily disable multicoloured cubes when drawing the border
  bool multiColCube = mMultiColCube;
  mMultiColCube = false;

  // Draw left side of well
  for(float x = -5.5f, y = 9.5f; y >= -10.5f; y -= 1.0f) {
    drawCube(x, y, 0.0f);
  }

  // Draw bottom of well
  for(float x = -4.5f, y = -10.5f; x <= 5.5f; x += 1.0f) {
    drawCube(x, y, 0.0f);
  }

  // Draw the right side of the well
  for(float x = 5.5f, y = -9.5f; y <= 9.5f; y += 1.0f) {
    drawCube(x, y, 0.0f);
  }

  mMultiColCube = multiColCube;
}

void Viewer::drawGameBoard() {
  int width = mGame.getWidth();
  int height = mGame.getHeight() + 4;

  for(int r = 0; r < height; r++) {
    for(int c = 0; c < width; c++) {
      int type = mGame.get(r, c);
      if(type >= 0) {
        mProgram.setUniformValue(mColorLocation, type);
        drawCube((float)c-(float)width/2.0f+0.5f, (float)r-(float)(height-4)/2.0f+0.5f, 0.0f);
      }
    }
  }
}

void Viewer::updateGame() {
  mGame.tick();
}

