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
  // Push the transform on the matrix stack and walk the children
  viewer->pushMatrix();
  viewer->multMatrix(get_transform());
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

void SceneNode::clear_joint_rotation()
{
  for(auto child : m_children) {
    child->clear_joint_rotation();
  }
}

void SceneNode::push_joint_rotation()
{
  for(auto child : m_children) {
    child->push_joint_rotation();
  }
}

void SceneNode::undo_joint_rotation()
{
  for(auto child : m_children) {
    child->undo_joint_rotation();
  }
}

void SceneNode::redo_joint_rotation()
{
  for(auto child : m_children) {
    child->redo_joint_rotation();
  }
}

void SceneNode::apply_joint_rotation(float x_rot, float y_rot)
{
  for(auto child : m_children) {
    child->apply_joint_rotation(x_rot, y_rot);
  }
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

void JointNode::clear_joint_rotation()
{
  SceneNode::clear_joint_rotation();
  m_redo_stack.clear();
  m_undo_stack.clear();
}

void JointNode::push_joint_rotation()
{
  SceneNode::push_joint_rotation();
  if(m_selected) {
    m_redo_stack.clear();
    m_undo_stack.push_back((m_undo_stack.size() > 0) ? m_undo_stack.back() : JointAngles());
  }
}

void JointNode::undo_joint_rotation()
{
  SceneNode::undo_joint_rotation();
  if(m_selected && m_undo_stack.size() > 0) {
    m_redo_stack.push_back(m_undo_stack.back());
    m_undo_stack.pop_back();
  }
}

void JointNode::redo_joint_rotation()
{
  SceneNode::redo_joint_rotation();
  if(m_selected && m_redo_stack.size() > 0) {
    m_undo_stack.push_back(m_redo_stack.back());
    m_redo_stack.pop_back();
  }
}

void JointNode::apply_joint_rotation(float x_rot, float y_rot)
{
  SceneNode::apply_joint_rotation(x_rot, y_rot);
  if(m_selected) {
    // Must ensure angles are within the constraints
    JointAngles& angles = m_undo_stack.back();
    angles.x_rot = (x_rot + angles.x_rot <= m_joint_x.max) ? ((x_rot + angles.x_rot >= m_joint_x.min) ? angles.x_rot + x_rot : m_joint_x.min) : m_joint_x.max;
    angles.y_rot = (y_rot + angles.y_rot <= m_joint_y.max) ? ((y_rot + angles.y_rot >= m_joint_y.min) ? angles.y_rot + y_rot : m_joint_y.min) : m_joint_y.max;
  }
}

void JointNode::walk_gl(Viewer* viewer, bool picking)
{
  // Walk the children of this node
  // If picking is enabled and a child of the joint is selected then
  // set the joint as selected and return
  viewer->pushMatrix();
  viewer->multMatrix(get_transform());

  // Transform joint children to initial x and y angles 
  QMatrix4x4 m;
  m.rotate(m_joint_x.init, 1, 0, 0);
  m.rotate(m_joint_y.init, 0, 1, 0);
  viewer->multMatrix(m);

  // Multiply by joint rotation angles
  QMatrix4x4 joint;
  joint.rotate(m_undo_stack.size() > 0 ? m_undo_stack.back().x_rot : 0.0, 1, 0, 0);
  joint.rotate(m_undo_stack.size() > 0 ? m_undo_stack.back().y_rot : 0.0, 0, 1, 0);
  viewer->multMatrix(joint);

  for(auto child: m_children) {
    child->walk_gl(viewer, picking);
    if(picking && (m_selected = child->is_picked(viewer))) break;
  }

  viewer->popMatrix();
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

bool GeometryNode::is_picked(Viewer *viewer)
{
  viewer->pushMatrix();
  viewer->multMatrix(get_transform());
  toggle_selected(viewer->picker());
  viewer->popMatrix();

  return m_selected;
}

void GeometryNode::walk_gl(Viewer* viewer, bool picking)
{
  // Walk the children of this node
  viewer->pushMatrix();
  viewer->multMatrix(get_transform());
  for(auto child : m_children) {
    child->walk_gl(viewer, picking);
  }

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

  viewer->popMatrix();
}
 
