#pragma once
#include "basic_template.hpp"
#include <set>
class GlobalState;
class GateGlobalState;
class UIGlobalState;
using GS = GlobalState;
using GGS = GateGlobalState;
using UGS = UIGlobalState;
class Touchable {
  // virtual const Touchable *checkPointCollision(Vector2 pos) const = 0;
public:
  using Id = Id<Touchable>;
  virtual const Id checkPointCollision(Vector2 pos) const = 0;
  Touchable();
  Touchable(Id id);
  virtual ~Touchable() {};
  // Touchable(TouchableCollection *const tc);
  // Touchable(Touchable &) = delete;
  // Touchable(Touchable &&) = delete;
  bool is_touching(const GS &) const;
  bool is_selected(const GS &) const;
  bool is_clicking(const GS &) const;
  bool is_clicked(const GS &) const;
  Id id()const;
  //  void toSelected(const TC &) const;

private:
  const Id _id = Id::generateId();
  friend class GlobalState;
};

class GlobalState {

public:
  using IdT = Id<Touchable>;
  virtual void frameInit() = 0;
  virtual bool hasSelected() const = 0;
  virtual bool isSelected(const Touchable &obj) const = 0;
  virtual bool isTouching(const Touchable &obj) const = 0;
  virtual Vector2 screenToWorldPos(Vector2) const = 0;
  Vector2 getGlobalMousePosition() const;
};
class UIGlobalState : public GlobalState {
  // If a object is selected or not is checked by touchable collection.

private:
  IdT _touching = IdT::Null;
  IdT _lastSelected = IdT::Null; // To check if input bar is active or not.

public:
  void frameInit() override final;
  const IdT
  touchUpdate(const Touchable &); // Calculate if object is being touched
                                  // by the mosue. You need to call
                                  // afterTouchUpdate() function after it.
  void afterTouchUpdate(); // This one check if you clicked anything or not.
                           // and update as selectd.
  bool hasSelected() const override final;
  bool isSelected(const Touchable &obj) const override final;
  bool isTouching(const Touchable &obj) const override final;
  Vector2 screenToWorldPos(Vector2) const override final;
  void toSelect(Touchable const &obj);
  friend class Touchable;
};
class GateGlobalState : public GlobalState {
  // If a object is selected or not is checked by touchable collection.
  using Idt = GlobalState::IdT;

private:
  IdT _touching = IdT::Null;
  std::set<IdT> _lastSelected;

private:
  void _toggleSelection();
  void _clearAllSelection();
  void _removeSelection(const IdT);
  bool _isSelected(const IdT);
  void _toSelected(const IdT);
public:
  void frameInit() override final;
  const IdT touchUpdate(
      const Touchable &);  // Calculate if object is being touched by the
                           // mosue. by the mosue. You need to call
                           // afterTouchUpdate() function after it.
  void afterTouchUpdate(); // This one check if you clicked anything or not.
                           // and update as selectd.It also supports multiple
                           // selection.

  bool hasSelected() const override final;
  void toSelected(const Touchable&);
  void removeSelection(const Touchable &);
  bool isSelected(const Touchable &obj) const override final;
  bool isTouching(const Touchable &obj) const override final;
  Vector2 screenToWorldPos(Vector2) const override final;
  friend class Touchable;
};
