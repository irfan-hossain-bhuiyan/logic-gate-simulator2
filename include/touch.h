#include "basic_template.h"
#include <set>
class GlobalState;
class GateGlobalState;
class UIGlobalState;
using GS = GlobalState;
using GGS = GateGlobalState;
using UGS = UIGlobalState;
class Touchable {
public:
  virtual const Touchable *checkPointCollision(Vector2 pos) const = 0;
  virtual ~Touchable() {};
  // Touchable(TouchableCollection *const tc);
  // Touchable(Touchable &) = delete;
  // Touchable(Touchable &&) = delete;
  bool is_touching(const GS &) const;
  bool is_selected(const GS &) const;
  bool is_clicking(const GS &) const;
  bool is_clicked(const GS &) const;
  //  void toSelected(const TC &) const;

private:
  friend class GlobalState;
};

class GlobalState {

public:
  virtual void frameInit() = 0;
  virtual bool hasSelected() const = 0;
  virtual bool isSelected(Touchable const &obj) const = 0;
  virtual bool isTouching(Touchable const &obj) const = 0;
  virtual Vector2 screenToWorldPos(Vector2) const = 0;
  Vector2 getGlobalMousePosition() const;
};
class UIGlobalState : public GlobalState {
  // If a object is selected or not is checked by touchable collection.
private:
  Touchable const *_touching = nullptr;
  Touchable const *_lastSelected =
      nullptr; // To check if input bar is active or not.

public:
  void frameInit() override final;
  const Touchable *
  touchUpdate(const Touchable &); // Calculate if object is being touched
                                  // by the mosue. You need to call
                                  // afterTouchUpdate() function after it.
  void afterTouchUpdate(); // This one check if you clicked anything or not.
                           // and update as selectd.
  bool hasSelected() const override final;
  bool isSelected(Touchable const &obj) const override final;
  bool isTouching(Touchable const &obj) const override final;
  Vector2 screenToWorldPos(Vector2) const override final;
  void toSelect(Touchable const &obj);
  friend class Touchable;
};
class GateGlobalState : public GlobalState {
  // If a object is selected or not is checked by touchable collection.
private:
  Touchable const *_touching = nullptr;
  std::set<Touchable const *> _lastSelected = {};

private:
  void _toggleSelection();
  void _clearSelection();
  void _toSelected(const Touchable &);

public:
  void frameInit() override final;
  const Touchable *
  touchUpdate(const Touchable &); // Calculate if object is being touched by the
                                  // mosue. by the mosue. You need to call
                                  // afterTouchUpdate() function after it.
  void afterTouchUpdate(); // This one check if you clicked anything or not.
                           // and update as selectd.It also supports multiple
                           // selection.

  bool hasSelected() const override final;
  void removeSelection(const Touchable &);
  bool isSelected(Touchable const &obj) const override final;
  bool isTouching(Touchable const &obj) const override final;
  Vector2 screenToWorldPos(Vector2) const override final;
  friend class Touchable;
};
