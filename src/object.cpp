#include "object.h"
#include "basic_template.h"
#include "globals.h"
#include <raylib.h>

bool Draggable::isDraggable() {
  using GameManager::GateWindow::isMouseState;
  return isMouseState(GameManager::GateWindow::MouseState::editing);
}
void Draggable::mouseMoveUpdate() {
  UsedCameraS camera = get_tc()->_camera;
  { // Updating is_dragging and mouse_relative field feild
    if (is_clicked() && isDraggable()) {
      if (!is_dragging) {
        is_dragging = true;
        mouseRelative = NodePos - getGlobalMousePosition(camera);
      }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      is_dragging = false;
    }
  }
  if (is_dragging) {
    NodePos = getGlobalMousePosition(camera) + mouseRelative;
  }
}
Touchable* DraggableBox::_checkPointCollision(Vector2 position) {
  return CheckCollisionPointRec(position, rectFromPos(NodePos, rectSize))?(Touchable*)this:nullptr;
}
void DraggableBox::draw() {
  Label(rectFromPos(NodePos, rectSize), label, color)
      .draw(is_clicking() ? 3 : 1);
}
