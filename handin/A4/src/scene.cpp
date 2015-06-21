#include "scene.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
  set_transform(m_trans.rotate(angle, (axis == 'x') ? 1.0 : 0.0, (axis == 'y') ? 1.0 : 0.0, (axis == 'z') ? 1.0 : 0.0));
}

void SceneNode::scale(const Vector3D& amount)
{
  set_transform(m_trans.scale(amount));
}

void SceneNode::translate(const Vector3D& amount)
{
  set_transform(m_trans.translate(amount));
}

bool SceneNode::is_joint() const
{
  return false;
}

bool SceneNode::intersect(const Ray& ray, Intersection& i) const
{
  bool intersects = false;
  for(auto child : m_children)
  {
    intersects = intersects || child->intersect(ray, i);
  }
  return intersects;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
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

bool GeometryNode::intersect(const Ray& ray, Intersection& i) const
{
  // Test for intersection
  // If there is an intersection, check if the t parameter is less than
  // the previous intersection t parameter. If it is then this primitive
  // is closer to the eye point so replace the material with this primitive's
  // material.
  Intersection j;
  bool intersects = m_primitive->intersect(ray, j);
  if(intersects)
  {
    if(j.t < i.t)
    {
      i.t = j.t;
      i.normal = j.normal;
      i.material = get_material();
    }
  }

  return (intersects || SceneNode::intersect(ray, i));
}

GeometryNode::~GeometryNode()
{
}
 
