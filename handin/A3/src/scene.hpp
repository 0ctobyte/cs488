#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <QMatrix4x4>
#include "primitive.hpp"
#include "material.hpp"

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(Viewer* viewer, bool picking = false);

  const QMatrix4x4& get_transform() const { return m_trans; }
  const QMatrix4x4& get_inverse() const { return m_invtrans; }

  void set_transform(const QMatrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.inverted();
  }

  void set_transform(const QMatrix4x4& m, const QMatrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const QVector3D& amount);
  void translate(const QVector3D& amount);

  void rotate(double angle, const QVector3D& axis);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;

  // Propagate joint rotations through the hierarchy
  virtual void clear_joint_rotation();
  virtual void push_joint_rotation();
  virtual void undo_joint_rotation();
  virtual void redo_joint_rotation();
  virtual void apply_joint_rotation(float x_rot, float y_rot);

  void toggle_selected(bool picked) { if(picked) m_selected = !m_selected; }
  bool is_selected() { return m_selected; }
  virtual bool is_picked(Viewer *viewer) { return false; }
  
protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  QMatrix4x4 m_trans;
  QMatrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;

  bool m_selected;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(Viewer* viewer, bool picking = false);

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  virtual void clear_joint_rotation();
  virtual void push_joint_rotation();
  virtual void undo_joint_rotation();
  virtual void redo_joint_rotation();
  virtual void apply_joint_rotation(float x_rot, float y_rot);

  struct JointRange {
    double min, init, max;
  };

  struct JointAngles {
    double x_rot, y_rot;
  };
  
protected:

  JointRange m_joint_x, m_joint_y;
  
  std::vector<JointAngles> m_undo_stack;
  std::vector<JointAngles> m_redo_stack;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name, Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(Viewer* viewer, bool picking = false);

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

  virtual bool is_picked(Viewer *viewer);

protected:
  Material* m_material;
  Primitive* m_primitive;
};

#endif
