#include "object.h"
#include "basic_template.h"
#include <raylib.h>
void Draggable::mouseMoveUpdate() {
  {//Updating is_dragging and mouse_relative field feild
    if (is_clickedone()) {
      if (!is_dragging) {
        is_dragging = true;
        mouseRelative = NodePos - GetMousePosition();
      }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      is_dragging = false;
    }
  }
  if (is_dragging) {
    NodePos = GetMousePosition() + mouseRelative;
  }
}
bool DraggableBox::CollisionPoint(Vector2 position) {
  return CheckCollisionPointRec(position, rectFromPos(NodePos, rectSize));
}
void DraggableBox::draw() {
  Label(rectFromPos(NodePos, rectSize), label, color)
      .draw(is_clicking() ? 3 : 1);
}
