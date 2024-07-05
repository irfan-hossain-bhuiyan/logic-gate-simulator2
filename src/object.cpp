#include "object.h"
#include "basic_template.h"
#include <raylib.h>
void Draggable::mouseMoveUpdate() {
  if (is_clicking()) {
    if (is_dragging) {
      NodePos = GetMousePosition() + mouseRelative;
    } else {
      is_dragging = true;
      mouseRelative = NodePos - GetMousePosition();
    }
  } else {
    is_dragging = false;
  }
}
bool DraggableBox::CollisionPoint(Vector2 position) {
  return CheckCollisionPointRec(position, rectFromPos(NodePos, rectSize));
}
void DraggableBox::draw() {
  Label(rectFromPos(NodePos, rectSize), label, color).draw(is_clicking()?3:1);
}

