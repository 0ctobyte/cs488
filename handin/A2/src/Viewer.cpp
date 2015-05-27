#include <QtWidgets>
#include <QtOpenGL>
#include <iostream>
#include <cmath>

#include "a2.hpp"
#include "Viewer.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#define DELTA2ANGLE(s) (((s)/(double)fabs(m_Viewport.width()))*360.0f)
#define DELTA2TRANSFORM(axis, s) ((axis)*(s)*2.0/(double)fabs(m_Viewport.width()))

using namespace std;

Viewer::Viewer(const QGLFormat& format, QWidget *parent) 
    : QGLWidget(format, parent) 
    , m_Mode(M_ROTATE)
    , mVAO(0)
    , mVBO(0)
    , m_ModelGnomon({QVector3D(0.0, 0.0, 0.0), QVector3D(0.5, 0.0, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.5, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 0.5)})
    , m_WorldGnomon({QVector3D(0.0, 0.0, 0.0), QVector3D(0.5, 0.0, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.5, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 0.5)})
    , m_Box({QVector3D(1.0, 1.0, 1.0), QVector3D(-1.0, 1.0, 1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, -1.0, 1.0), QVector3D(1.0, -1.0, 1.0), QVector3D(-1.0, -1.0, 1.0), QVector3D(-1.0, -1.0, 1.0), QVector3D(-1.0, 1.0, 1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, 1.0, -1.0), QVector3D(1.0, -1.0, 1.0), QVector3D(1.0, -1.0, -1.0), QVector3D(-1.0, 1.0, 1.0), QVector3D(-1.0, 1.0, -1.0), QVector3D(-1.0, -1.0, 1.0), QVector3D(-1.0, -1.0, -1.0), QVector3D(1.0, 1.0, -1.0), QVector3D(-1.0, 1.0, -1.0), QVector3D(1.0, 1.0, -1.0), QVector3D(1.0, -1.0, -1.0), QVector3D(1.0, -1.0, -1.0), QVector3D(-1.0, -1.0, -1.0), QVector3D(-1.0, -1.0, -1.0), QVector3D(-1.0, 1.0, -1.0)})
    , m_ViewportBorder({QVector3D(-1.0, -1.0, 0.0), QVector3D(1.0, -1.0, 0.0), QVector3D(1.0, -1.0, 0.0), QVector3D(1.0, 1.0, 0.0), QVector3D(1.0, 1.0, 0.0), QVector3D(-1.0, 1.0, 0.0), QVector3D(-1.0, 1.0, 0.0), QVector3D(-1.0, -1.0, 0.0)})
    , m_MouseCoord(0, 0)
{
  m_Timer = new QTimer(this);
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(update()));
  m_Timer->start(1000/30);

  // This is needed since the widget defaults to a smaller size before being resized
  // to sizeHint size. This forces the widget to just resize to sizeHint right away
  resize(sizeHint());

  reset_view();
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
  identity(m_Projection);
  perspective(m_Projection, fov, aspect, near, far); 
}

void Viewer::reset_view()
{
    // Reset the model matrices
    identity(m_Model);
    identity(m_ModelScale);

    // Reset the view matrix
    m_CamPos = QVector3D(0.0f, 0.0f, 5.0f);
    identity(m_View);
    translate(m_View, -m_CamPos);
    
    // Reset the projection matrix
    QSize size = sizeHint();
    m_zNear = 1.0f;
    m_zFar = 10.0f;
    m_vFov = 30.0f;
    set_perspective(m_vFov, (double)size.width()/(double)size.height(), m_zNear, m_zFar);

    // Set the viewport
    // 5% margins around the viewport
    float hmargin = 0.05*(float)size.height();
    float wmargin = 0.05*(float)size.width();
    m_Viewport.setX(wmargin);
    m_Viewport.setY(hmargin);
    m_Viewport.setWidth(size.width()-2*wmargin);
    m_Viewport.setHeight(size.height()-2*hmargin);
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

void Viewer::paintGL() {    
    draw_init();

    // Here is where your drawing code should go.
    
    /* A few of lines are drawn below to show how it's done. */
    set_colour(QColor(1.0, 1.0, 1.0));

    // Draw the viewport border
    set_colour(QColor(0.0, 0.0, 0.0));
    drawArrays(m_ViewportBorder, 8, QMatrix4x4());

    // Draw the world gnomon
    set_colour(QColor(0.0, 1.0, 0.0));
    drawArrays(m_WorldGnomon, 6, m_Projection * m_View);

    // Draw the model gnomon
    set_colour(QColor(1.0, 0.0, 0.0));
    drawArrays(m_ModelGnomon, 6, m_Projection * m_View * m_Model);

    // Draw the box
    set_colour(QColor(1.0, 1.0, 1.0));
    drawArrays(m_Box, 24, m_Projection * m_View * m_Model * m_ModelScale);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    m_MouseCoord.setX(event->x());
    m_MouseCoord.setY(event->y());

    if(m_Mode == VIEWPORT_MODE) {
      m_Viewport.setX(m_MouseCoord.x());
      m_Viewport.setY(height()-m_MouseCoord.y());
      m_Viewport.setWidth(0);
      m_Viewport.setHeight(0);
    }
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
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
      rotate(m_Model, DELTA2ANGLE(s), axis);
      break;
    case M_TRANSLATE:
      translate(m_Model, transform);
      break;
    case M_SCALE:
      scale(m_ModelScale, transform+QVector3D(1.0f, 1.0f, 1.0f));
      break;
    case V_ROTATE:
      // Need to translate camera back to origin before rotating
      // A transformation on the camera means an equal but opposite transformation for objects in the world
      translate(m_View, m_CamPos);
      rotate(m_View, DELTA2ANGLE(-s), axis);
      translate(m_View, -m_CamPos);
      break;
    case V_TRANSLATE:
      // Keep track of the camera position
      // A transformation on the camera means an equal but opposite transformation for objects in the world
      m_CamPos = m_CamPos + transform;
      translate(m_View, -transform);
      break;
    case V_PERSPECTIVE:
      if(event->buttons() & Qt::LeftButton) {
        m_vFov = ((m_vFov + s) >= 5) ? (((m_vFov + s) <= 160) ? m_vFov + s : 160) : 5;
      }
      if(event->buttons() & Qt::MidButton) {
        m_zNear = m_zNear + s;
      }
      if(event->buttons() & Qt::RightButton) {
        m_zFar = m_zFar + s;
      }
      set_perspective(m_vFov, (float)abs(m_Viewport.width())/(float)abs(m_Viewport.height()), m_zNear, m_zFar);
      break;
    case VIEWPORT_MODE:
      // Clamp the viewport to the edges of the widget
      m_Viewport.setWidth((event->x() >= 0 ) ? ((event->x() <= width()) ? (event->x()-m_Viewport.x()) : (width()-m_Viewport.x())) : (0-m_Viewport.x()));
      m_Viewport.setHeight((event->y() >= 0 ) ? ((event->y() <= height()) ? ((height()-event->y())-m_Viewport.y()) : (0-m_Viewport.y())) : (height()-m_Viewport.y()));
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

  // For each pair of point:
  // 1. Apply any (model, view & projection) transformations
  // 2. Clip the line described by the point pair to the viewing volume in homogeneous coordinate space
  // 3. Perspective divide
  // 4. Map the NDC points to the viewport

  QVector4D P, Q;
  for(uint32_t i = 0; i < num; i += 2) {
    // Apply model, view and projection transformations.The points are now in clip space
    P = transform * QVector4D(points[i], 1.0f);
    Q = transform * QVector4D(points[i+1], 1.0f);

    // Clip the line to the clipping volume.
    bool reject = clipLine(P, Q);

    if(!reject) {
      // Perspective divde. The points are now in NDC space
      P = P / P.w();
      Q = Q / Q.w();

      // Map the points from NDC space to screen space
      viewportMap(P, Q);

      // Finally draw the line
      draw_line(QVector2D(P.x(), P.y()), QVector2D(Q.x(), Q.y()));
    }
  }
}

bool Viewer::clipLine(QVector4D& A, QVector4D& B) {
  // All 6 clip planes
  float P[6] = {A.x(), -A.x(), A.y(), -A.y(), A.z(), -A.z()};
  float Q[6] = {B.x(), -B.x(), B.y(), -B.y(), B.z(), -B.z()};

  // Loop through each clip plane and test if the points are within bounds
  for(uint32_t i = 0; i < 6; i++) {
    // Mirror the coordinates on the W axis if they are behind the W=0 projection plane
    if(A.w() < 0) A.setW(-A.w());
    if(B.w() < 0) B.setW(-B.w());

    // Trivially accept the line if it is within the {x,y,z}=w plane.
    // Trivially reject the line if it is completely outside of the viewing volume
    if((A.w() > 0 && (A.w() + P[i]) >= 0) && (B.w() > 0 && (B.w() + Q[i]) >= 0)) continue;
    else if((A.w() > 0 && (A.w() + P[i]) < 0) && (B.w() > 0 && (B.w() + Q[i]) < 0)) return true;

    // Use the parametric equation of a line intersecting with a plane to find parameter a
    // This is the distance from point A to the intersection of the plane
    float a = (A.w() + P[i]) / ((A.w() + P[i]) - (B.w() + Q[i]));

    // Use the parametric line equation to find the intersect point
    if(A.w() > 0 && (A.w() + P[i]) < 0) {
      A = A + a*(B-A);
    } else {
      B = A + a*(B-A);
    }
  }

  return false;
} 

void Viewer::viewportMap(QVector4D& A, QVector4D& B) {
  QMatrix4x4 M;

  // These equations find the corner points of viewport in terms of the widgets normalized window
  float x = (2.0*m_Viewport.x())/(float)width()-1;
  float y = (2.0*m_Viewport.y())/(float)height()-1;
  float w = (2.0*(m_Viewport.x()+m_Viewport.width()))/(float)width()-1;
  float h = (2.0*(m_Viewport.y()+m_Viewport.height()))/(float)height()-1;
  
  // Translate the world to center on viewports origin
  translate(M, QVector3D((x + w)/2.0, (y + h)/2.0, 0));

  // Scale the world to fit within the viewports boundary
  scale(M, QVector3D(fabs((float)m_Viewport.width()/(float)width()), fabs((float)m_Viewport.height()/(float)height()), 0)); 

  // Transform the points
  A = M*A;
  B = M*B;
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
