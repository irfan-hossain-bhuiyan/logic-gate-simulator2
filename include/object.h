#pragma once
#include "basic_template.h"
#include "ui.h"
#include <raylib.h>
class Draggable :public Touchable {
private:
  bool is_dragging=false;
  Vector2 mouseRelative; // Mouse relative position with the object

public:
  Vector2 NodePos;
  void mouseMoveUpdate();
  virtual bool isDraggable();
  Draggable(TouchableCollection* const tc, Vector2 nodePos)
      : Touchable(tc), NodePos(nodePos) {}
};

class DraggableBox :public Draggable {
private:
  RectSize rectSize;
  Color color = RED;
  Chars label = "";

public:
  DraggableBox(TouchableCollection *tc, Vector2 position, RectSize rectSize={30,40}, const Chars& label = "")
      : Draggable(tc, position), rectSize(rectSize), label(label) {}
  bool _checkPointCollision(Vector2 position) override;
  void draw();
};
