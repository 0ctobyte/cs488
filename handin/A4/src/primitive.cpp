#include "primitive.hpp"
#include "polyroots.hpp"

#include <cmath>
#include <algorithm>
#include <limits>

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Ray& ray, Intersection& j) const
{
  NonhierSphere sphere(Point3D(0.0, 0.0, 0.0), 1.0);
  return sphere.intersect(ray, j);
}

Cube::~Cube()
{
}

bool Cube::intersect(const Ray& ray, Intersection& j) const
{
  NonhierBox box(Point3D(0.0, 0.0, 0.0), 1.0);
  return box.intersect(ray, j);
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::intersect(const Ray& ray, Intersection& j) const
{
  // Ray/sphere intersection test
  // Equation for a sphere centered at p_c with radius r and arbitrary point on the sphere p:
  // (p - p_c) . (p - p_c) - r^2 = 0;
  // Equation for the line coincident with a ray at origin p_o and direction d:
  // t*d + p_o = p
  // Substituting the equation for the ray into the equation for the sphere and solving for parameter t
  // will give use the intersection point if one exists
  // (t*d + p_o - p_c) . (t*d + p_o - p_c) - r^2 = 0
  // (d . d)*t^2 + (2d . (p_o - p_c))*t - ((p_o - p_c) . (p_o - p_c)) - r^2 = 0
  // A = (d . d)
  // B = (2d . (p_o - p_c))
  // C = ((p_o - p_c) . (p_o - p_c)) - r^2
  
  double roots[2];

  Vector3D v = ray.origin() - m_pos;
  double A = ray.direction().dot(ray.direction());
  double B = (2*ray.direction()).dot(v);
  double C = v.dot(v) - m_radius*m_radius;

  size_t num_roots = quadraticRoots(A, B, C, roots);

  // If there are no roots==no intersections
  // If there is one root, then the ray intersect the sphere on a tangent
  // If there is two roots, then one is the intersection with the sphere from the front as the ray enters the sphere
  // and the other is the intersection through the back as it leaves the sphere
  // For this case we only take the smallest t value since that is the point where the ray enters the sphere which
  // will be the visible point of the sphere. Of course t must not be negative otherwise it is behind the eye point
  if(num_roots > 0)
  {
    double t = (num_roots == 1) ? roots[0] : std::min<double>(roots[0], roots[1]);
    if(t < 0) return false;
    j.q = ray.origin() + t*ray.direction();
    j.n = (j.q - m_pos).normalized();
    return true;
  }
  
  return false;
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::intersect(const Ray& ray, Intersection& j) const
{
  return false;
}

