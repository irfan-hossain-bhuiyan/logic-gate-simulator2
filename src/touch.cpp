#include "touch.h"
#include "globals.h"
#include <raylib.h>
bool Touchable::is_touching(const GS &tc) const { return tc.isTouching(*this); }
bool Touchable::is_selected(const GS &tc) const { return tc.isSelected(*this); }
bool Touchable::is_clicked(const GS &tc) const {
  return is_selected(tc) &&
         IsMouseButtonPressed(MOUSE_BUTTON_LEFT); // This works because for
                                                  // each frame if MousePressed
                                                  // function returns true,for
                                                  // every frame it called.
}
bool Touchable::is_clicking(const GS &tc) const {
  return is_touching(tc) && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

bool UIGlobalState::hasSelected() const { return _lastSelected != nullptr; }
bool UIGlobalState::isSelected(const Touchable &t) const {
  return _lastSelected == &t;
}
bool UIGlobalState::isTouching(const Touchable &t) const {
  return _touching == &t;
}

Vector2 UIGlobalState::screenToWorldPos(Vector2 pos) const { return pos; }
void UIGlobalState::toSelect(Touchable const &obj) { _lastSelected = &obj; }

Vector2 GS::getGlobalMousePosition() const {
  return screenToWorldPos(GetMousePosition());
}
void UIGlobalState::frameInit() { _touching = nullptr; }
void GateGlobalState::frameInit() { _touching = nullptr; }
const Touchable *UIGlobalState::touchUpdate(const Touchable &tc) {
  if (_touching != nullptr)
    return _touching; // The mouse is already touching other object.
  _touching = tc.checkPointCollision(GetMousePosition());
  return _touching;
}
void UIGlobalState::afterTouchUpdate() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    _lastSelected = _touching;
}
const Touchable *GateGlobalState::touchUpdate(const Touchable &tc) {
  if (_touching != nullptr)
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
  return _lastSelected.find(&t) != _lastSelected.end();
  // std::find(_lastSelected.begin(), _lastSelected.end(), &t)
  // !=_lastSelected.end();
}
bool GateGlobalState::isTouching(const Touchable &t) const {
  return _touching == &t;
}

void GateGlobalState::_toggleSelection() {
  if (isSelected(*_touching)) {
    removeSelection(*_touching);
  } else {
    _toSelected(*_touching);
  }
}
void GateGlobalState::removeSelection(const Touchable &tc) {
  _lastSelected.erase(&tc);
}
void GateGlobalState::_toSelected(const Touchable &tc) {
  _lastSelected.insert(&tc);
}
void GateGlobalState::_clearSelection() { _lastSelected.clear(); }
void GateGlobalState::afterTouchUpdate() {
  if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    return;
  bool shiftPressed = IsKeyDown(KEY_LEFT_SHIFT);
  bool touching = _touching != nullptr;
  if (shiftPressed == false && touching == false)
    _clearSelection();
  else if (shiftPressed == false && touching == true) {
    if (isSelected(*_touching))
      return;
    _clearSelection();
    _toSelected(*_touching);
  } else if (shiftPressed == true && touching == true)
    _toggleSelection();

  return;
}
