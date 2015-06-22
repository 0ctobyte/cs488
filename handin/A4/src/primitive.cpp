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

Cube::~Cube()
{
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
    double t = (num_roots == 1) ? roots[0] : ((roots[0] < roots[1]) ? roots[0] : roots[1]);
    if(t < 0) return false;
    j.t = t;
    j.normal = ((ray.origin() + j.t*ray.direction()) - m_pos).normalized();
    return true;
  }
  
  return false;
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::intersect(const Ray& ray, Intersection& j) const
{
  Vector3D r_dir(1.0 / ray.direction()[0], 1.0 / ray.direction()[1], 1.0 / ray.direction()[2]); // Reciprocal of the ray's direction vector
  Point3D r_origin(ray.origin());
  Point3D b_min = m_pos;
  Point3D b_max(b_min[0] + m_size, b_min[1] + m_size, b_min[2] + m_size);

  // Calculate intersection point using slabs method
  // Algorithm adapted from here: http://tavianator.com/fast-branchless-raybounding-box-intersections/
  double tx1 = (b_min[0] - r_origin[0]) * r_dir[0];
  double tx2 = (b_max[0] - r_origin[0]) * r_dir[0];

  double ty1 = (b_min[1] - r_origin[1]) * r_dir[1];
  double ty2 = (b_max[1] - r_origin[1]) * r_dir[1];

  double tz1 = (b_min[2] - r_origin[2]) * r_dir[2];
  double tz2 = (b_max[2] - r_origin[2]) * r_dir[2];

  double tmin = std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)), std::min(tz1, tz2));
  double tmax = std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)), std::max(tz1, tz2));

  // If tmax is greater than tmin than an intersection has occured and the closest intersection point to the eye is at tmin
  // tmax must also be greater than 0. If it isn't then the box is behind the eye point
  if(tmax >= 0 && tmax >= tmin)
  {
    j.t = tmin;

    // Now we need to find the normal of the intersection point
    // For each face of the box, calculate the normal and check if the vector from the intersection point to a point on the face
    // is perpendicular to the normal. If it is then it is contained within that face and the normal can be used to shade the intersection point
    double x = m_pos[0], y = m_pos[1], z = m_pos[2], r = m_size;
    Point3D Q = r_origin + j.t*ray.direction();

    // Calculate a face point and face normal for each of the 6 faces
    Point3D fp[6] = {Point3D(x+r, y, z), Point3D(x+r, y+r, z), Point3D(x+r, y, z+r), Point3D(x, y+r, z), Point3D(x, y+r, z+r), Point3D(x, y, z+r)};
    Vector3D fn[6] = {(Point3D(x, y, z) - fp[0]).cross(Point3D(x+r, y+r, z) - fp[0]).normalized(),
      (Point3D(x, y+r, z) - fp[1]).cross(Point3D(x+r, y+r, z+r) - fp[1]).normalized(),
      (Point3D(x+r, y, z) - fp[2]).cross(Point3D(x+r, y+r, z+r) - fp[2]).normalized(),
      (Point3D(x, y, z) - fp[3]).cross(Point3D(x, y+r, z+r) - fp[3]).normalized(),
      (Point3D(x, y, z+r) - fp[4]).cross(Point3D(x+r, y+r, z+r) - fp[4]).normalized(),
      (Point3D(x, y, z) - fp[5]).cross(Point3D(x+r, y, z+r) - fp[5]).normalized()
    };

    for(int i = 0; i < 6; i++)
    {
      double d = (Q - fp[i]).dot(fn[i]);
      if(fabs(d) < std::numeric_limits<double>::epsilon())
      {
        // If d is close to zero than the intersection point lies on this face
        j.normal = fn[i];
        break;
      }
    }

    return true;
  }
  
  return false;
}

