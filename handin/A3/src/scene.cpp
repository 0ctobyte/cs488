#include <QGLShaderProgram>
#include "scene.hpp"
#include "Viewer.hpp"

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
  , m_selected(false)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(Viewer* viewer, bool picking)
{
  viewer->pushMatrix();
  viewer->multMatrix(get_transform());
  if(is_joint()) viewer->multJointMatrix();
  for(auto child : m_children) {
    child->walk_gl(viewer, picking);
  }
  viewer->popMatrix();
}

void SceneNode::rotate(char axis, double angle)
{
  QMatrix4x4 m;

  if(axis == 'x') m.rotate(angle, 1.0, 0.0, 0.0);
  else if(axis == 'y') m.rotate(angle, 0.0, 1.0, 0.0); 
  else if(axis == 'z') m.rotate(angle, 0.0, 0.0, 1.0);

  set_transform(get_transform() * m);
}

void SceneNode::scale(const QVector3D& amount)
{
  QMatrix4x4 m;
  m.scale(amount);
  set_transform(get_transform() * m);
}

void SceneNode::translate(const QVector3D& amount)
{
  QMatrix4x4 m;
  m.translate(amount);
  set_transform(get_transform() * m);
}

void SceneNode::rotate(double angle, const QVector3D& axis) {
  QMatrix4x4 m;
  m.rotate(angle, axis);
  set_transform(get_transform() * m);
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(Viewer* viewer, bool picking)
{
  // Walk the children of this node
  SceneNode::walk_gl(viewer, picking);
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::walk_gl(Viewer* viewer, bool picking)
{
  // Walk the children of this node
  viewer->pushMatrix();
  viewer->multMatrix(get_transform());
  for(auto child : m_children) {
    child->walk_gl(viewer, picking);
  }

  if(!picking) {
    // Apply the material and draw the geometry
    m_material->apply_gl(viewer);
    m_primitive->walk_gl(viewer, picking);

    // Draw the mesh on top if selected
    if(m_selected) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      viewer->setLighting(QColor(0, 0, 0), QColor(0, 0, 0), 0, false);
      m_primitive->walk_gl(viewer, picking);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  } else {
    toggle_selected(viewer->picker());
  }
  viewer->popMatrix();
}
 
