#include <QGLShaderProgram>
#include "Viewer.hpp"
#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const QColor& kd, const QColor& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl(Viewer *viewer) const
{
  viewer->setLighting(m_kd, m_ks, m_shininess, true);
}
