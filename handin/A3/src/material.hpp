#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include <QColor>

class Viewer;

class Material {
public:
  virtual ~Material();
  virtual void apply_gl(Viewer* viewer) const = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const QColor& kd, const QColor& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl(Viewer* viewer) const;

private:
  QColor m_kd;
  QColor m_ks;

  double m_shininess;
};


#endif
