#include "basic_template.h"
#include "object.h"
#include "raylib.h"
#include "ui.h"
#include <cstddef>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  InitWindow(WIDTH, HEIGHT, "Input test.");
  TouchableCollection tc = TouchableCollection();
  auto box=DraggableBox(&tc,{20,20},{30,30},"A");
  while (!WindowShouldClose()) {
    tc.click_update();
    box.mouseMoveUpdate();
       BeginDrawing();
    ClearBackground(RAYWHITE);
    box.draw();
    Debugger::draw();
    EndDrawing();
  }
}
