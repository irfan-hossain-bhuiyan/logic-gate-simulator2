#pragma once
#include "basic_template.hpp"
#include "ui.hpp"
#include <raylib.h>
class Draggable : public Touchable {
private:
  bool is_dragging = false;
  Vector2 mouseRelative; // Mouse relative position with the object

public:
  Vector2 NodePos;
  void mouseMoveUpdate(const GS&);
  virtual bool isDraggable();
  Draggable(Vector2 nodePos) : NodePos(nodePos) {}
};

class DraggableBox : public Draggable {
private:
  RectSize rectSize;
  Color color = RED;
  Chars label = "";

public:
  DraggableBox(Vector2 position, RectSize rectSize = {30, 40},
               const Chars &label = "")
      : Draggable(position), rectSize(rectSize), label(label) {}
  const Id checkPointCollision(Vector2 position) const override;
  void draw(const GS& gs)const;
};
