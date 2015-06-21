#include "a4.hpp"
#include "image.hpp"

#include <cmath>

Matrix4x4 a4_get_unproject_matrix(int width, int height, double fov, double d, Point3D eye, Vector3D view, Vector3D up)
{
  double h = 2.0*d*tan(fov / 2.0); // height of projection plane based field of view and distance to the plane
  
  // First translate the pixel so that it is centered at the origin in the projection plane (origin is in the middle of the screen)
  Matrix4x4 viewport_translate = Matrix4x4().translate(-(double)width / 2.0, -(double)height / 2.0, d);

  // Then scale it to the projection plane such that aspect ratio is maintained and we have a right handed coordinate system
  Matrix4x4 viewport_scale = Matrix4x4().scale(-h / (double)height, h / (double)height, 1.0);

  // Calculate the basis for the view coordinate system
  view.normalize();
  up.normalize();
  Vector3D u = up.cross(view);
  u.normalize();
  Vector3D v = view.cross(u);
  v.normalize();

  // Create the view rotation and translation matrix
  Matrix4x4 view_rotate = Matrix4x4(Vector4D(u, 0.0), Vector4D(v, 0.0), Vector4D(view, 0.0), Vector4D(0.0, 0.0, 0.0, 1.0)).transpose();
  Matrix4x4 view_translate = Matrix4x4().translate(Vector3D(eye));

  // Now multiply these together to form the pixel to 3D point transformation matrix
  Matrix4x4 unproject = view_translate * view_rotate * viewport_scale * viewport_translate;

  return unproject;
}

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
  // Fill in raytracing code here.

  std::cerr << "Stub: a4_render(" << root << ",\n     "
            << filename << ", " << width << ", " << height << ",\n     "
            << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
            << ambient << ",\n     {";

  for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  std::cerr << "});" << std::endl;

  // Get pixel unprojection matrix
  Matrix4x4 unproject = a4_get_unproject_matrix(width, height, fov, 1.0, eye, view, up);
    
  // For now, just make a sample image.

  Image img(width, height, 3);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Unproject the pixel to the projection plane
      Point3D pixel (x, y, 0.0);
      Point3D p = unproject * pixel;

      // Create the ray with origin at the eye point
      Ray ray(eye, p-eye);

      // Test intersection of ray with scene
      PhongMaterial material(Colour(0, 0, 0), Colour(0, 0, 0), 0);
      Intersection i;
      i.material = &material;

      root->intersect(ray, i);

      img(x, y, 0) = i.material->diffuse().R();
      img(x, y, 1) = i.material->diffuse().G();
      img(x, y, 2) = i.material->diffuse().B();

      std::cout << "Pixel: " << pixel << " | " << "Point3D: " << p << std::endl;
    }
  }
  img.savePng(filename);
  
}
