#ifndef CS488_A2_HPP
#define CS488_A2_HPP

#include <QMatrix4x4>

void rotate(QMatrix4x4& matrix, float angle, const QVector3D& vector);
void translate(QMatrix4x4& matrix, const QVector3D& vector);
void scale(QMatrix4x4& matrix, const QVector3D& vector); 

#endif // CS488_A2_HPP

