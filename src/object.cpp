#include "object.hpp"
#include "basic_template.hpp"
#include "globals.hpp"
#include <raylib.h>

bool Draggable::isDraggable() {
  using GameManager::GateWindow::isMouseState;
  return isMouseState(GameManager::GateWindow::MouseState::editing);
}
void Draggable::mouseMoveUpdate(const GS & gs) {
  using GameManager::UsedCameraS;
  { // Updating is_dragging and mouse_relative field feild
    if (is_clicked(gs) && isDraggable()) {
      if (!is_dragging) {
        is_dragging = true;
        mouseRelative = NodePos - gs.getGlobalMousePosition();
      }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      is_dragging = false;
    }
  }
  if (is_dragging) {
    NodePos = gs.getGlobalMousePosition() + mouseRelative;
  }
}
const Touchable *DraggableBox::checkPointCollision(Vector2 position)const {
  return CheckCollisionPointRec(position, rectFromPos(NodePos, rectSize))
             ? (Touchable *)this
             : nullptr;
}
void DraggableBox::draw(const GS& gs)const {
  Label(rectFromPos(NodePos, rectSize), label, color)
      .draw(is_clicking(gs) ? 3 : 1);
}
