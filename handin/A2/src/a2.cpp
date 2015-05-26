#include <cmath>

#include "a2.hpp"

void rotate(QMatrix4x4& matrix, float angle, const QVector3D& vector) {
  QMatrix4x4 r;
  r.setToIdentity();
  QVector3D n = vector.normalized();
  float* data = r.data();

  // Calculate angle in radians
  angle = angle * M_PI/180.0f;

  // Calculate constant values
  float c = cos(angle);
  float s = sin(angle);
  float oc = 1 - c;

  float x = n.x();
  float y = n.y();
  float z = n.z();

  // Apply the rotation to the arbitrary axis
  data[0] = (x*x)*oc+c;
  data[1] = y*x*oc+z*s;
  data[2] = x*z*oc-y*s;
  data[4] = x*y*oc-z*s;
  data[5] = (y*y)*oc+c;
  data[6] = y*z*oc+x*s;
  data[8] = x*z*oc+y*s;
  data[9] = y*z*oc-x*s;
  data[10] = (z*z)*oc+c;

  // Multiply the rotation matrix with the given matrix
  matrix = matrix * r;
}

void translate(QMatrix4x4& matrix, const QVector3D& vector) {
  QMatrix4x4 t;
  t.setToIdentity();
  float* data = t.data();

  data[12] = vector.x();
  data[13] = vector.y();
  data[14] = vector.z();

  matrix = matrix * t;
}

void scale(QMatrix4x4& matrix, const QVector3D& vector) {
  QMatrix4x4 s;
  s.setToIdentity();
  float* data = s.data();

  data[0] = vector.x();
  data[5] = vector.y();
  data[10] = vector.z();

  matrix = matrix * s;
}

