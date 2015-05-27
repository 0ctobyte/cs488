#include <cmath>

#include "a2.hpp"

void identity(QMatrix4x4& matrix) {
  float *data = matrix.data();
  for(int i = 0; i < 16; i++) {
    data[i] = ((i % 5) == 0) ? 1.0 : 0.0;
  }
}

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

void perspective(QMatrix4x4& matrix, double fovy, double aspect, double near, double far) {
  QMatrix4x4 p;
  float *data = p.data();

  float one_over_tan = 1.0f/tan((fovy*(M_PI/180.0f))/2.0f);
  float nz_fz = near-far;

  data[0] = one_over_tan/aspect;
  data[5] = one_over_tan;
  data[10] = (near+far)/(nz_fz);
  data[11] = -1.0f;
  data[14] = (2.0f*far*near)/nz_fz;
  data[15] = 0.0f;

  matrix = matrix * p;
}

