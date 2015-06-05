#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

class Viewer;

class Primitive {
public:
  virtual ~Primitive();
  virtual void walk_gl(Viewer* viewer, bool picking) const = 0;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual void walk_gl(Viewer* viewer, bool picking) const;
};

#endif
