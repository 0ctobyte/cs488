#include <QGLShaderProgram>
#include "scene.hpp"
#include "Viewer.hpp"

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(Viewer* viewer, bool picking) const
{
  // Loop through each child, set the transformation by premultiplying this (parent) 
  // transformation with that of the child. Then call walk_gl for that child. Reset
  // the childs matrix after. The original inverse matrix is not changed throughout of all of 
  // this since it may be needed to invert a transformation on the children and we
  // don't want an inverted projection matrix because that doesn't make sense.
  for(auto child : m_children) {
    QMatrix4x4 T = child->get_transform();
    QMatrix4x4 I = child->get_inverse();

    // If the child is a joint, then we must remove the scale factor from the matrix
    // but perserve the translations and rotations. To do this we simply take the
    // transpose of the inverse of the transformation matrix. It is known that the
    // inverse of any rotation matrix is its transpose. Thus inverting the matrix
    // will apply a scale factor that is the reciprocal of the original scale factor
    // and transposing will perserve the original rotation.
    if(child->is_joint()) {
      child->set_transform(this->get_transform() * this->get_inverse().transposed() * T, I);
    } else {
      child->set_transform(this->get_transform() * T, I);
    }

    child->walk_gl(viewer, picking);
    child->set_transform(T, I);
  }
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

void JointNode::walk_gl(Viewer* viewer, bool picking) const
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

void GeometryNode::walk_gl(Viewer* viewer, bool picking) const
{
  // Walk the children of this node
  SceneNode::walk_gl(viewer, picking);

  // Set the transform matrices
  QGLShaderProgram& program = viewer->getShaderProgram();
  program.setUniformValue("mvpMatrix", viewer->getProjectionMatrix() * viewer->getCameraMatrix() * get_transform());
  program.setUniformValue("modelViewMatrix", viewer->getCameraMatrix() * get_transform());
  program.setUniformValue("normalModelViewMatrix", (viewer->getCameraMatrix() * get_transform()).normalMatrix());

  // Draw the geometry
  m_material->apply_gl(viewer);
  m_primitive->walk_gl(viewer, picking);
}
 
