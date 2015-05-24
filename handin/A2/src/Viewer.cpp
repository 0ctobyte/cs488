#include <QtWidgets>
#include <QtOpenGL>
#include <iostream>
#include <cmath>

#include "Viewer.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#define DELTA2ANGLE(s) (((s)/(double)width())*360.0f)
#define DELTA2TRANSFORM(axis, s) ((axis)*(s)*2.0/(double)width())

using namespace std;

Viewer::Viewer(const QGLFormat& format, QWidget *parent) 
    : QGLWidget(format, parent) 
    , m_Mode(M_ROTATE)
    , mVAO(0)
    , mVBO(0)
    , m_ModelGnomon({QVector3D(0.0, 0.0, 0.0), QVector3D(0.5, 0.0, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.5, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 0.5)})
    , m_WorldGnomon({QVector3D(0.0, 0.0, 0.0), QVector3D(0.5, 0.0, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.5, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 0.5)})
    , m_Box({QVector3D(1.0, 1.0, 1.0), QVector3D(-1.0, 1.0, 1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, -1.0, 1.0), QVector3D(1.0, -1.0, 1.0), QVector3D(-1.0, -1.0, 1.0), QVector3D(-1.0, -1.0, 1.0), QVector3D(-1.0, 1.0, 1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, 1.0, -1.0), QVector3D(1.0, -1.0, 1.0), QVector3D(1.0, -1.0, -1.0), QVector3D(-1.0, 1.0, 1.0), QVector3D(-1.0, 1.0, -1.0), QVector3D(-1.0, -1.0, 1.0), QVector3D(-1.0, -1.0, -1.0), QVector3D(1.0, 1.0, -1.0), QVector3D(-1.0, 1.0, -1.0), QVector3D(1.0, 1.0, -1.0), QVector3D(1.0, -1.0, -1.0), QVector3D(1.0, -1.0, -1.0), QVector3D(-1.0, -1.0, -1.0), QVector3D(-1.0, -1.0, -1.0), QVector3D(-1.0, 1.0, -1.0)})
    , m_MouseCoord(0, 0)
    , m_CamPos(0.0f, 0.0f, 5.0f)
{
  reset_view();

  m_Timer = new QTimer(this);
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(update()));
  m_Timer->start(1000/30);
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

void Viewer::set_perspective(double fov, double aspect, double near, double far)
{
  m_Projection.setToIdentity();
  m_Projection.perspective(fov, aspect, near, far); 
}

void Viewer::reset_view()
{
    // Reset the model matrices
    m_Model.setToIdentity();
    m_ModelScale.setToIdentity();

    // Reset the view matrix
    m_CamPos = QVector3D(0.0f, 0.0f, 5.0f);
    m_View.setToIdentity();
    m_View.translate(-m_CamPos);
    
    // Reset the projection matrix
    set_perspective(30.0, (double)width()/(double)height(), 0.001, 1000);
}

void Viewer::initializeGL() {
    // Do some OpenGL setup
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.7, 0.7, 0.7, 0.0);
    
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

    /*
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of 
     * QOpenGLBuffer.
     */
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
     
    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");
    
    glGenBuffers = (_glGenBuffers) QGLWidget::context()->getProcAddress("glGenBuffers");
    glBindBuffer = (_glBindBuffer) QGLWidget::context()->getProcAddress("glBindBuffer");

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    if (mVBO == 0) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    mColorLocation = mProgram.uniformLocation("frag_color");
}

void Viewer::resizeGL(int width, int height) {
    if(height == 0) height = 1;

    set_perspective(30.0, (double)width/(double)height, 0.001, 1000);
}

void Viewer::paintGL() {    
    draw_init();

    // Here is where your drawing code should go.
    
    /* A few of lines are drawn below to show how it's done. */
    set_colour(QColor(1.0, 1.0, 1.0));

    // Draw the world gnomon
    drawArrays(m_WorldGnomon, 6, m_Projection * m_View);

    // Draw the model gnomon
    drawArrays(m_ModelGnomon, 6, m_Projection * m_View * m_Model);

    // Draw the box
    drawArrays(m_Box, 24, m_Projection * m_View * m_Model * m_ModelScale);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " pressed\n";

    m_MouseCoord.setX(event->x());
    m_MouseCoord.setY(event->y());
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " released\n";
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
    
    float s = event->x() - m_MouseCoord.x();
    QVector3D axis (0.0f, 0.0f, 0.0f);

    if(event->buttons() & Qt::LeftButton) {
      axis.setX(1.0);
    }

    if(event->buttons() & Qt::MidButton) {
      axis.setY(1.0);
    }

    if(event->buttons() & Qt::RightButton) {
      axis.setZ(1.0);
    }

    QVector3D transform = DELTA2TRANSFORM(axis, s);

    switch(m_Mode) {
    case M_ROTATE:
      m_Model.rotate(DELTA2ANGLE(s), axis);
      break;
    case M_TRANSLATE:
      m_Model.translate(transform);
      break;
    case M_SCALE:
      m_ModelScale.scale(transform+QVector3D(1.0f, 1.0f, 1.0f));
      break;
    case V_ROTATE:
      m_View.translate(m_CamPos);
      m_View.rotate(DELTA2ANGLE(-s), axis);
      m_View.translate(-m_CamPos);
      break;
    case V_TRANSLATE:
      m_CamPos = m_CamPos + transform;
      m_View.translate(-transform);
      break;
    default:
      break;
    }

    // Update the coords to calculate the delta next time this event is triggered
    m_MouseCoord.setX(event->x());
    m_MouseCoord.setY(event->y());
}

void Viewer::drawArrays(QVector3D *points, size_t num, QMatrix4x4 transform) {
  // Round to previous even number
  if(num & 0x1) num &= ~(0x1);
  
  QVector4D P, Q;
  for(uint32_t i = 0; i < num; i += 2) {
    P = transform * QVector4D(points[i], 1.0f);
    Q = transform * QVector4D(points[i+1], 1.0f);
    P = P / P.w();
    Q = Q / Q.w();
    draw_line(QVector2D(P.x(), P.y()), QVector2D(Q.x(), Q.y()));
  }
}

// Drawing Functions
// ******************* Do Not Touch ************************ // 

void Viewer::draw_line(const QVector2D& p1, const QVector2D& p2) {

  const GLfloat lineVertices[] = {
    p1.x(), p1.y(), 1.0,
    p2.x(), p2.y(), 1.0
  };

  typedef void (APIENTRY *_glBufferData) (GLenum, GLsizeiptr, const GLvoid*, GLenum);
  _glBufferData glBufferData;
  glBufferData = (_glBufferData) QGLWidget::context()->getProcAddress("glBufferData");

  glBufferData(GL_ARRAY_BUFFER, 2*3*sizeof(float), lineVertices, GL_STATIC_DRAW); 

  glDrawArrays(GL_LINES, 0, 2);
}

void Viewer::set_colour(const QColor& col)
{
  mProgram.setUniformValue(mColorLocation, col.red(), col.green(), col.blue());
}

void Viewer::draw_init() {
    glClearColor(0.7, 0.7, 0.7, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width(), height());

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
}
