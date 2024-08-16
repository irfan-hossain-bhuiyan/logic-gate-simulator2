#include "basic_template.h"
#include <span>
class GlobalState;
class GateGlobalState;
class UIGlobalState;
using GS = GlobalState;
using GGS = GateGlobalState;
using UGS = UIGlobalState;
class Touchable {
public:
  virtual const Touchable *_checkPointCollision(Vector2 pos) const = 0;
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
  virtual bool hasSelected() const = 0;
  virtual bool isSelected(Touchable const &obj) const = 0;
  virtual bool isTouching(Touchable const &obj) const = 0;
  virtual Vector2 screenToWorldPos(Vector2) const = 0;
};
class UIGlobalState : public GlobalState {
  // If a object is selected or not is checked by touchable collection.
private:
  Touchable const *_touching = nullptr;
  Touchable const *_lastSelected = nullptr;

public:
  UIGlobalState();
  template<typename Iterator>
  bool clickUpdate(Iterator begin,Iterator end);
  //bool clickUpdate(const std::initializer_list<Touchable const *> l);
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
  Vec<Touchable const *> _lastSelected = {};

public:
  GateGlobalState();
  template<typename Iterator>
  bool clickUpdate(Iterator begin,Iterator end);
  bool hasSelected() const override final;
  bool isSelected(Touchable const &obj) const override final;
  bool isTouching(Touchable const &obj) const override final;
  Vector2 screenToWorldPos(Vector2) const override final;
  friend class Touchable;
};

template<typename Iterator>
bool UIGlobalState::clickUpdate(Iterator begin,Iterator end) {
  this->_touching = nullptr;
  for (const auto &x=begin;x!=end;x++) {
    const Touchable *touched = x->_checkPointCollision(GetMousePosition());
    if (touched != nullptr) {
      this->_touching = touched;
      break;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    this->_lastSelected = this->_touching;
  }
  return _touching!=nullptr;
}

template<typename Iterator>
bool GateGlobalState::clickUpdate(Iterator begin,Iterator end) {
  this->_touching = nullptr;
  for (const auto &x=begin;x!=end;x++) {
    const Touchable *touched = x->_checkPointCollision(GetMousePosition());
    if (touched != nullptr) {
      this->_touching = touched;
      break;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
      this->_lastSelected.push_back(
          this->_touching); // It can be null.As I am using it as value,not to
                            // dereference it.
      return _touching!=nullptr;
    }
    if (_touching == nullptr) {
      this->_lastSelected.clear();
    }
  }
  return _touching!=nullptr;
}
