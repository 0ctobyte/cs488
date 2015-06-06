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
  QGLShaderProgram& program = viewer->getShaderProgram();

  program.setUniformValue("material.diffuse", m_kd.redF(), m_kd.greenF(), m_kd.blueF());
  program.setUniformValue("material.specular", m_ks.redF(), m_ks.greenF(), m_ks.blueF());
  program.setUniformValue("material.shininess", (float)m_shininess);
  program.setUniformValue("lighting_enabled", 1);
}
