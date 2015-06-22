#include "mesh.hpp"
#include <iostream>
#include <cmath>
#include <limits>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
}

bool Mesh::intersect(const Ray& ray, Intersection& j) const
{
  // Intersection tests are done per face which are convex and planar polygons
  // If the ray intersects with at least one of the faces then the ray intersects with the mesh
  // We have to check intersection with every single face to find the closest intersection
  bool intersected = false;
  double prev_t = std::numeric_limits<double>::infinity();
  for(auto face : m_faces)
  {
    // We need three points in order to calculate the normal for the plane that contains the face
    // We assume that there is some sort of winding or order that the vertices are in such that 
    // an indexed vertex are adjacent to the previous indexed vertex
    Point3D P = m_verts[face[1]];
    Vector3D normal = (m_verts[face[0]] - P).cross(m_verts[face[2]] - P).normalized();

    // Check if the ray intersects the plane at all
    double d = normal.dot(ray.direction());
    if(fabs(d) < std::numeric_limits<double>::epsilon()) continue;

    // Calculate t, the parameter t in the ray parametric equation of the line
    // Essentially the distance from the ray's origin to the intersecting point along the ray's direction vector
    double t = normal.dot(P - ray.origin()) / d;

    // Check if t is behind the eye
    if(t < 0) continue;

    // Alright now check if a previous intersection with another face is closer than this one
    if(prev_t < t) continue;

    // So we have found the intersection point for the ray intersecting the *plane* that contains the face
    // Now we have to check if the intersection point is within the bounds of the actual face
    bool in_bounds = false;
    Point3D Q = ray.origin() + t*ray.direction();
    
    // Make sure to check last edge!
    Point3D P0 = m_verts[face.back()];
    Point3D P1 = m_verts[face[0]];
    if(((P0 - P1).cross(Q - P1)).dot(normal) >= 0.0) in_bounds = true;

    // We are essentially check if the cross product of the vector Q (intersection point) and P0 (point in face)
    // and P1 (another point on face) and P1 gives us the same normal vector as the plane
    // This makes sense since if the point was outside the face, then the normal vector would be in the opposite direction
    for(size_t i = 1; in_bounds && i < face.size(); i++)
    {
      Point3D P0 = m_verts[face[i-1]];
      Point3D P1 = m_verts[face[i]];
      if(((P0 - P1).cross(Q - P1)).dot(normal) >= 0.0) in_bounds = true;
      else in_bounds = false;
    }

    // Alright, so the intersection point is in bounds, update the Intersection object
    if(in_bounds)
    {
      prev_t = t;
      j.q = Q; 
      j.n = normal;
      intersected = true;
    }
  }

  return intersected;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
