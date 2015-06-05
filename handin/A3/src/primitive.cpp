#include "primitive.hpp"
#include "Viewer.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

void Sphere::walk_gl(Viewer* viewer, bool picking) const
{
  viewer->drawSphere();
}
