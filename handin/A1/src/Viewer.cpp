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
    , mGame(10, 20)
    , mVAO(0)
{
    mPieceColour[0][0] = 1.0f; mPieceColour[0][1] = 0.6f; mPieceColour[0][2] = 0.6f;
    mPieceColour[1][0] = 1.0f; mPieceColour[1][1] = 0.8f; mPieceColour[1][2] = 0.6f;
    mPieceColour[2][0] = 1.0f; mPieceColour[2][1] = 1.0f; mPieceColour[2][2] = 0.6f;
    mPieceColour[3][0] = 0.8f; mPieceColour[3][1] = 1.0f; mPieceColour[3][2] = 0.6f;
    mPieceColour[4][0] = 0.6f; mPieceColour[4][1] = 1.0f; mPieceColour[4][2] = 0.6f;
    mPieceColour[5][0] = 0.6f; mPieceColour[5][1] = 1.0f; mPieceColour[5][2] = 0.8f;
    mPieceColour[6][0] = 0.6f; mPieceColour[6][1] = 1.0f; mPieceColour[6][2] = 1.0f;
    mPieceColour[7][0] = 0.6f; mPieceColour[7][1] = 0.8f; mPieceColour[7][2] = 1.0f;
    mPieceColour[8][0] = 0.5f; mPieceColour[8][1] = 0.5f; mPieceColour[8][2] = 0.5f;

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
      // front quad
      0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      0.5f, 0.5f, 0.0f,
      -0.5, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      // right quad
      0.5f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, -1.0f,
      0.5f, 0.5f, 0.0f,
      0.5f, -0.5f, -1.0f,
      0.5f, 0.5f, -1.0f,
      // back quad
      0.5f, -0.5, 0.0f,
      -0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, -1.0f,
      0.5f, -0.5, 0.0f,
      -0.5f, -0.5f, -1.0f,
      0.5f, -0.5f, -1.0f,
      // left quad
      -0.5f, 0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, -1.0f,
      -0.5f, 0.5f, 0.0f,
      -0.5f, -0.5f, -1.0f,
      -0.5f, 0.5f, -1.0f,
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
    mColorLocation = mProgram.uniformLocation("frag_color");
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
    mProgram.setUniformValue(mColorLocation, 1.0f, 0.0f, 0.0f);

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
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " released\n";
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
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

void Viewer::drawCube(float x, float y, float z) {
  glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::CUBE]);

  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  mCubeModelMatrix.setToIdentity();
  mCubeModelMatrix.translate(x, y, z);
  mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mCubeModelMatrix);
  
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Viewer::drawGameBorder() {
  mProgram.setUniformValue(mColorLocation, mPieceColour[8][0], mPieceColour[8][1], mPieceColour[8][2]);

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
}

void Viewer::drawGameBoard() {
  int width = mGame.getWidth();
  int height = mGame.getHeight() + 4;

  for(int r = 0; r < height; r++) {
    for(int c = 0; c < width; c++) {
      int type = mGame.get(r, c);
      if(type >= 0) {
        mProgram.setUniformValue(mColorLocation, mPieceColour[type][0], mPieceColour[type][1], mPieceColour[type][2]);
        drawCube((float)c-(float)width/2.0f+0.5f, (float)r-(float)height/2.0f+0.5f, 0.0f);
      }
    }
  }
}

void Viewer::updateGame() {
  int r = mGame.tick();
  if(r < 0) {
    // Game over
    mGame.reset();
  }
}

