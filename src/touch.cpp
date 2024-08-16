#include "touch.h"
#include "globals.h"
#include <algorithm>
#include <initializer_list>
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

bool GateGlobalState::hasSelected() const { return !_lastSelected.empty(); }
bool GateGlobalState::isSelected(const Touchable &t) const {
  return std::find(_lastSelected.begin(), _lastSelected.end(), &t) !=
         _lastSelected.end();
}
bool GateGlobalState::isTouching(const Touchable &t) const {
  return _touching == &t;
}


Vector2 UIGlobalState::screenToWorldPos(Vector2 pos) const { return pos; }
Vector2 GateGlobalState::screenToWorldPos(Vector2 pos) const {
  return GameManager::getScreenToWorld(pos,
                                       GameManager::UsedCameraS::gateCamera);
}
void UIGlobalState::toSelect(Touchable const &obj) { _lastSelected = &obj; }
