#include "a4.hpp"
#include "image.hpp"

#include <cmath>

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

  // Get unprojection matrix
  double d = 1.0; // distance to near plane (projection plane)
  double h = 2.0*d*tan(fov / 2.0); // height of projection plane based field of view and distance to the plane
  
  // First translate the pixel so that it is centered at the origin in the projection plane (origin is in the middle of the screen)
  Matrix4x4 m = Matrix4x4().translate(-(double)width / 2.0, -(double)height / 2.0, d);

  // Then scale it to the projection plane such that aspect ratio is maintained and we have a right handed coordinate system
  m = m.scale(-h / (double)height, h / (double)height, 1.0);

  
  // For now, just make a sample image.

  Image img(width, height, 3);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < height; x++) {
      // Red: increasing from top to bottom
      img(x, y, 0) = (double)y / height;
      // Green: increasing from left to right
      img(x, y, 1) = (double)x / width;
      // Blue: in lower-left and upper-right corners
      img(x, y, 2) = ((y < height/2 && x < height/2)
                      || (y >= height/2 && x >= height/2)) ? 1.0 : 0.0;
    }
  }
  img.savePng(filename);
  
}
