#include <QtWidgets>
#include <QtOpenGL>
#include <QTimer>
#include <iostream>
#include <math.h>
#include <vector>
#include "Viewer.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(const QGLFormat& format, QWidget *parent) 
    : QGLWidget(format, parent) 
    , mVAO(0)
    , mVBO({0, 0})
    , mIBO(0)
    , mCameraPosition(0.0, 0.0, 1.0)
    , m_sceneRoot(new SceneNode("sceneRoot"))
    , mMode(Mode::TRANSFORM)
    , mMouseCoord(0, 0)
{
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(1000/30);
}

Viewer::~Viewer() {
    // Nothing to do here right now.
}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 300);
}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor( 0.4, 0.4, 0.4, 0.0 );
    glEnable(GL_DEPTH_TEST);
    
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

    float circleData[120];

    for(size_t i=0; i<40; ++i) {
        circleData[i*3] = cos(i*2*M_PI/40);
        circleData[i*3 + 1] = sin(i*2*M_PI/40);
        circleData[i*3 + 2] = 0.0;
    }

    typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);

    // Have to get function pointers for these functions as well -_-
    typedef void (APIENTRY *_glGenBuffers) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindBuffer) (GLenum, GLuint);
    typedef void (APIENTRY *_glBufferData) (GLenum, GLsizeiptr, const GLvoid*, GLenum);
     
    _glGenVertexArrays glGenVertexArrays;
    _glBindVertexArray glBindVertexArray;

    _glGenBuffers glGenBuffers;
    _glBindBuffer glBindBuffer;
    _glBufferData glBufferData;
     
    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");
    
    glGenBuffers = (_glGenBuffers) QGLWidget::context()->getProcAddress("glGenBuffers");
    glBindBuffer = (_glBindBuffer) QGLWidget::context()->getProcAddress("glBindBuffer");
    glBufferData = (_glBufferData) QGLWidget::context()->getProcAddress("glBufferData");

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO[VBO::CIRCLE]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::CIRCLE]);

    if (mVBO[VBO::CIRCLE] == 0) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    glBufferData(GL_ARRAY_BUFFER, 40 * 3 * sizeof(float), circleData, GL_STATIC_DRAW);

    // Generate a sphere with radius 1
    // No need to generate normals since the normals for a unit sphere is just the direction
    // from the sphere's origin to the vertex which means the normal is just the vertex itself
    // (normalized). So this can be generated in the shaders
    glGenBuffers(1, &mVBO[VBO::SPHERE]);
    glGenBuffers(1, &mIBO);
    generate_sphere(3);

    mProgram.bind();

    mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
    mDiffuseColorLocation = mProgram.uniformLocation("material.diffuse");

    // These are static uniforms
    // The light source tracks the cameras position
    mProgram.setUniformValue("camera.position", mCameraPosition);
    mProgram.setUniformValue("lightSource.position", mCameraPosition);
    mProgram.setUniformValue("lightSource.intensity", 1.0, 1.0, 1.0);

    // Initial camera position
    resetPosition();
}

void Viewer::paintGL() {
    // Clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the scene
    m_sceneRoot->walk_gl(this);

    draw_trackball_circle();
}

void Viewer::drawSphere() {
  // Bind vertex and index buffer
  typedef void (APIENTRY *_glBindBuffer) (GLenum, GLuint);
  _glBindBuffer glBindBuffer;
  glBindBuffer = (_glBindBuffer) QGLWidget::context()->getProcAddress("glBindBuffer");
  glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::SPHERE]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  // Draw the indices
  glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
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

    mMouseCoord.setX(event->x());
    mMouseCoord.setY(height() - event->y());

    if(mMode == Mode::JOINTS) {
      // Clear the redo stack and push another matrix on the stack
      mRedoMatrixStack.clear();
      (mMatrixStack.size() > 0) ? mMatrixStack.push_back(mMatrixStack.back()) : mMatrixStack.push_back(QMatrix4x4());
    }
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " released\n";
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;

    float dx = (float)(event->x() - mMouseCoord.x())/(float)abs(width());
    float dy = (float)((height()-event->y()) - mMouseCoord.y())/(float)abs(height());

    if(mMode == Mode::TRANSFORM) {
      if(event->buttons() & Qt::LeftButton) translateWorld(dx*10.0, dy*10.0, 0);
      if(event->buttons() & Qt::MidButton) translateWorld(0, 0, dy*10.0);

      if(event->buttons() & Qt::RightButton) {
        QVector3D axis = virtual_trackball(mMouseCoord, QVector2D(event->x(), height() - event->y()));
        m_sceneRoot->rotate(axis.length() * 180.0 / M_PI, axis);
      }
    } else if(mMode == Mode::JOINTS) {
      if(event->buttons() & Qt::MidButton) {
        mMatrixStack.back().rotate(dy * 180, 0.0, 1.0, 0.0);
      }

      if(event->buttons() & Qt::RightButton) {
        mMatrixStack.back().rotate(dx * 180, 1.0, 0.0, 0.0);
      }
    }

    mMouseCoord.setX(event->x());
    mMouseCoord.setY(height() - event->y());
}

QMatrix4x4 Viewer::getCameraMatrix() {
    // Todo: Ask if we want to keep this.
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    QVector3D cameraPosition = cameraTransformation * mCameraPosition;
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
    // Todo: Ask if we want to keep this.
    mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float x, float y, float z) {
    // Todo: Ask if we want to keep this.
    mTransformMatrix.rotate(x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
    // Todo: Ask if we want to keep this.
    mTransformMatrix.scale(x, y, z);
}

void Viewer::set_colour(const QColor& col)
{
  mProgram.setUniformValue(mDiffuseColorLocation, col.red(), col.green(), col.blue());
}

void Viewer::draw_trackball_circle()
{
    int current_width = width();
    int current_height = height();

    // Set up for orthogonal drawing to draw a circle on screen.
    // You'll want to make the rest of the function conditional on
    // whether or not we want to draw the circle this time around.

    set_colour(QColor(0.0, 0.0, 0.0));

    // Set orthographic Matrix
    QMatrix4x4 orthoMatrix;
    orthoMatrix.ortho(0.0, (float)current_width, 0.0, (float)current_height, -0.1, 0.1);

    // Translate the view to the middle
    QMatrix4x4 transformMatrix;
    transformMatrix.translate(width()/2.0, height()/2.0, 0.0);

    // Scale the circle to have a radius r
    double r = (width() < height()) ? 0.25*(float)width() : 0.25*(float)height();
    transformMatrix.scale(r, r, 1);

    // Bind buffer object
    typedef void (APIENTRY *_glBindBuffer) (GLenum, GLuint);
    _glBindBuffer glBindBuffer;
    glBindBuffer = (_glBindBuffer) QGLWidget::context()->getProcAddress("glBindBuffer");
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::CIRCLE]);
    mProgram.setUniformValue(mMvpMatrixLocation, orthoMatrix * transformMatrix);
    mProgram.setUniformValue("lighting_enabled", 0);
    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    // Draw buffer
    glDrawArrays(GL_LINE_LOOP, 0, 40);    
}

bool Viewer::load_scene(std::string filename) {
  // Import scene
  SceneNode* root = import_lua(filename);

  if(!root) return false;

  // m_sceneRoot is a general transform node above the root of the scene
  m_sceneRoot->add_child(root);

  return true;
}

QVector3D Viewer::virtual_trackball(const QVector2D& P, const QVector2D& Q) {
  float d = (width() > height()) ? 0.5*width() : 0.5*height();

  QVector3D A ((P.x()-(float)width()/2.0) * 2.0 / d, (P.y()-(float)height()/2.0) * 2.0 / d, 0);
  A.setZ(1.0 - A.x()*A.x() - A.y()*A.y());

  if(A.z() < 0.0) {
    float len = sqrt(1.0 - A.z());
    A = QVector3D(A.x() / len, A.y() / len, 0.0);
  } else {
    A.setZ(sqrt(A.z()));
  }

  QVector3D B ((Q.x()-(float)width()/2.0) * 2.0 / d, (Q.y()-(float)height()/2.0) * 2.0 / d, 0);
  B.setZ(1.0 - B.x()*B.x() - B.y()*B.y());

  if(B.z() < 0.0) {
    float len = sqrt(1.0 - B.z());
    B = QVector3D(B.x() / len, B.y() / len, 0.0);
  } else {
    B.setZ(sqrt(B.z()));
  }

  return QVector3D::crossProduct(A, B);
}

void Viewer::generate_sphere(int detailLevel) {
  // This algorithm was derived from: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
  // Generate a sphere of radius 1 starting from a icosahedron with golden ratio t
  // The triangles are then subdivided generating 4 triangles from each original triangle
  float t = (1.0 + sqrt(5.0)) / 2.0;
  float icosahedronVertices[36] = {
    -1, t, 0,
    1, t, 0,
    -1, -t, 0,
    1, -t, 0,
    0, -1, t,
    0, 1, t,
    0, -1, -t,
    0, 1, -t,
    t, 0, -1,
    t, 0, 1,
    -t, 0, -1,
    -t, 0, 1,
  };

  uint32_t icosahedronIndices[60] = {
    0, 11, 5,
    0, 5, 1,
    0, 1, 7,
    0, 7, 10,
    0, 10, 11,
    1, 5, 9,
    5, 11, 4,
    11, 10, 2,
    10, 7, 6,
    7, 1, 8,
    3, 9, 4,
    3, 4, 2,
    3, 2, 6,
    3, 6, 8,
    3, 8, 9,
    4, 9, 5,
    2, 4, 11,
    6, 2, 10,
    8, 6, 7,
    9, 8, 1,
  };

  // Normalize the vertices
  for(int i = 0; i < 36; i += 3) {
    QVector3D v (icosahedronVertices[i], icosahedronVertices[i+1], icosahedronVertices[i+2]);
    v.normalize();
    icosahedronVertices[i] = v.x();
    icosahedronVertices[i+1] = v.y();
    icosahedronVertices[i+2] = v.z();
  }

  // Place generated vertices and indices in these vectors
  std::vector<float> sphereVertices (36, 0);
  std::vector<unsigned int> sphereIndices (60, 0);

  // Copy in icosahedron data
  float* vData = sphereVertices.data();
  memcpy(vData, icosahedronVertices, 36*sizeof(float));

  unsigned int *iData = sphereIndices.data();
  memcpy(iData, icosahedronIndices, 60*sizeof(unsigned int));

  // For each triple of indices, get the vertices (3 float triples)
  // Calculate midpoint of each edge
  // Place back each vertex (float triples) in the vertex array
  // Place back 4 index triples for the 4 triangles generated from the original triangle
  // Do this detailLevel number of times
  for(int i = 0; i < detailLevel; i++) {
    std::vector<float> genVerts;
    std::vector<unsigned int> genIndices;
    for(size_t i = 0; i < sphereIndices.size(); i += 3) {
      QVector3D p1 (sphereVertices[sphereIndices[i]*3], sphereVertices[sphereIndices[i]*3+1], sphereVertices[sphereIndices[i]*3+2]);
      QVector3D p2 (sphereVertices[sphereIndices[i+1]*3], sphereVertices[sphereIndices[i+1]*3+1], sphereVertices[sphereIndices[i+1]*3+2]);
      QVector3D p3 (sphereVertices[sphereIndices[i+2]*3], sphereVertices[sphereIndices[i+2]*3+1], sphereVertices[sphereIndices[i+2]*3+2]);

      QVector3D a = ((p1 + p2) / 2.0).normalized();
      QVector3D b = ((p2 + p3) / 2.0).normalized();
      QVector3D c = ((p3 + p1) / 2.0).normalized();

      genVerts.push_back(p1.x()); genVerts.push_back(p1.y()); genVerts.push_back(p1.z());
      genVerts.push_back(p2.x()); genVerts.push_back(p2.y()); genVerts.push_back(p2.z());
      genVerts.push_back(p3.x()); genVerts.push_back(p3.y()); genVerts.push_back(p3.z());
      genVerts.push_back(a.x()); genVerts.push_back(a.y()); genVerts.push_back(a.z());
      genVerts.push_back(b.x()); genVerts.push_back(b.y()); genVerts.push_back(b.z());
      genVerts.push_back(c.x()); genVerts.push_back(c.y()); genVerts.push_back(c.z());

      genIndices.push_back(i*2); genIndices.push_back(i*2+3); genIndices.push_back(i*2+5);
      genIndices.push_back(i*2+3); genIndices.push_back(i*2+1); genIndices.push_back(i*2+4);
      genIndices.push_back(i*2+3); genIndices.push_back(i*2+4); genIndices.push_back(i*2+5);
      genIndices.push_back(i*2+4); genIndices.push_back(i*2+2); genIndices.push_back(i*2+5);
    }

    // Swap the vectors
    sphereVertices.swap(genVerts);
    sphereIndices.swap(genIndices);
  }

  // Update the index and vertex counts
  mIndexCount = sphereIndices.size();
  mVertexCount = sphereVertices.size();
  
  // Upload the sphere data to the GPU
  // Have to get function pointers for these functions as well -_-
  typedef void (APIENTRY *_glBindBuffer) (GLenum, GLuint);
  typedef void (APIENTRY *_glBufferData) (GLenum, GLsizeiptr, const GLvoid*, GLenum);

  _glBindBuffer glBindBuffer;
  _glBufferData glBufferData;

  glBindBuffer = (_glBindBuffer) QGLWidget::context()->getProcAddress("glBindBuffer");
  glBufferData = (_glBufferData) QGLWidget::context()->getProcAddress("glBufferData");

  glBindBuffer(GL_ARRAY_BUFFER, mVBO[VBO::SPHERE]);

  if(mVBO[VBO::SPHERE] == 0) {
    std::cerr << "could not bind vertex buffer to the context." << std::endl;
    return;
  }

  glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);

  if(mIBO == 0) {
    std::cerr << "could not bind vertex buffer to the context." << std::endl;
    return;
  }

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);
}
