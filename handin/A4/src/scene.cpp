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
  // Transform the ray from WCS->MCS for this node
  Ray r(get_inverse() * ray.origin(), get_inverse() * ray.direction());

  bool intersects = false;
  for(auto child : m_children)
  {
    intersects = intersects || child->intersect(r, i);
  }

  // If intersection occurs than transform the intersection point and the normal
  // from MCS->WCS
  if(intersects)
  {
    i.q = get_transform() * i.q;
    i.n = transNorm(get_inverse(), i.n);
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

  // But first transform ray to geometry's model coordinates (inverse transform from WCS->MCS)
  Ray r(get_inverse() * ray.origin(), get_inverse() * ray.direction());

  Intersection j;
  bool intersects = m_primitive->intersect(r, j);
  if(intersects)
  {
    if(j.t < i.t)
    {
      // We have to convert the intersection point from MCS->WCS and the normal from MCS->WCS
      // Normals must be multiplied by the transpose of the inverse to throw away scaling (no translations either, but the normal is 
      // a vector and vectors can't be translated) but preserve rotation
      i.t = j.t;
      i.q = get_transform() * j.q;
      i.n = transNorm(get_inverse(), j.n);
      i.m = get_material();
    }
  }

  return (intersects || SceneNode::intersect(ray, i));
}

GeometryNode::~GeometryNode()
{
}
 
