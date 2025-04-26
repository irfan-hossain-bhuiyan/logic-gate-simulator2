#include "touch.hpp"
#include "globals.hpp"
#include <raylib.h>
Touchable::Touchable() {}
Touchable::Id Touchable::id() const { return _id; }
bool Touchable::is_touching(const GS &tc) const { return tc.isTouching(*this); }
bool Touchable::is_selected(const GS &tc) const { return tc.isSelected(*this); }
bool Touchable::is_clicked(const GS &tc) const {
  return is_selected(tc) &&
         IsMouseButtonPressed(MOUSE_BUTTON_LEFT); // This works because for
                                                  // each frame if MousePressed
                                                  // function returns true,for
                                                  // every frame it called.
}

Touchable::Touchable(Id id) : _id(id) {}
bool Touchable::is_clicking(const GS &tc) const {
  return is_touching(tc) && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

bool UIGlobalState::hasSelected() const { return _lastSelected != IdT::Null; }
bool UIGlobalState::isSelected(const Touchable &t) const {
  return _lastSelected == t.id();
}
bool UIGlobalState::isTouching(const Touchable &t) const {
  return _touching == t.id();
}

Vector2 UIGlobalState::screenToWorldPos(Vector2 pos) const { return pos; }
void UIGlobalState::toSelect(Touchable const &obj) { _lastSelected = obj.id(); }

Vector2 GS::getGlobalMousePosition() const {
  return screenToWorldPos(GetMousePosition());
}
void UIGlobalState::frameInit() { _touching = IdT::Null; }
void GateGlobalState::frameInit() { _touching = IdT::Null; }
const UIGlobalState::IdT UIGlobalState::touchUpdate(const Touchable &tc) {
  if (_touching.isNull() == false)
    return _touching; // The mouse is already touching other object.
  _touching = tc.checkPointCollision(GetMousePosition());
  return _touching;
}
void UIGlobalState::afterTouchUpdate() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    _lastSelected = _touching;
}
const GateGlobalState::IdT GateGlobalState::touchUpdate(const Touchable &tc) {
  if (_touching.isNull() == false)
    return _touching;
  _touching = tc.checkPointCollision(getGlobalMousePosition());
  return _touching;
}
Vector2 GateGlobalState::screenToWorldPos(Vector2 pos) const {
  return GameManager::getScreenToWorld(pos,
                                       GameManager::UsedCameraS::gateCamera);
}

bool GateGlobalState::hasSelected() const { return !_lastSelected.empty(); }
bool GateGlobalState::isSelected(const Touchable &t) const {
  return _lastSelected.find(t.id()) != _lastSelected.end();
  // std::find(_lastSelected.begin(), _lastSelected.end(), &t)
  // !=_lastSelected.end();
}
bool GateGlobalState::isTouching(const Touchable &t) const {
  return _touching == t.id();
}
bool GateGlobalState::_isSelected(const IdT idt) {
  return this->_lastSelected.count(idt) != 0;
}
void GateGlobalState::_removeSelection(const IdT idt) {
  _lastSelected.erase(idt);
}
void GateGlobalState::_toggleSelection() {
  if (_isSelected(_touching)) {
    _removeSelection(_touching);
  } else {
    _toSelected(_touching);
  }
}
void GateGlobalState::removeSelection(const Touchable &tc) {
  _lastSelected.erase(tc.id());
}
void GateGlobalState::_toSelected(const IdT tc) { _lastSelected.insert(tc); }
void GateGlobalState::toSelected(const Touchable& to) { _lastSelected.insert(to.id()); }
void GateGlobalState::_clearAllSelection() { _lastSelected.clear(); }
void GateGlobalState::afterTouchUpdate() {
  if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    return;
  bool shiftPressed = IsKeyDown(KEY_LEFT_SHIFT);
  bool touching = _touching.isNull() == false;
  if (shiftPressed == false && touching == false)
    _clearAllSelection();
  else if (shiftPressed == false && touching == true) {
    if (_isSelected(_touching))
      return;
    _clearAllSelection();
    _toSelected(_touching);
  } else if (shiftPressed == true && touching == true)
    _toggleSelection();

  return;
}
