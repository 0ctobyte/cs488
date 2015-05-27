#ifndef CS488_A2_HPP
#define CS488_A2_HPP

#include <QMatrix4x4>

// Sets the matrix to the identity matrix
void identity(QMatrix4x4& matrix);

// Multiplies matrix with a rotation matrix about the given axis
void rotate(QMatrix4x4& matrix, float angle, const QVector3D& vector);

// Multiplies matrix with a translation matrix specified by the vector
void translate(QMatrix4x4& matrix, const QVector3D& vector);

// Multiplies matrix with a scale matrix
void scale(QMatrix4x4& matrix, const QVector3D& vector); 

// Multiplies matrix with a perspective projection matrix
void perspective(QMatrix4x4& matrix, double fovy, double aspect, double near, double far);

#endif // CS488_A2_HPP

