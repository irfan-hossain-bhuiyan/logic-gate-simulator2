#include "basic_template.h"
#include "raylib.h"
#include "ui.h"
#include "gate.h"
#include <cstddef>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  
  InitWindow(WIDTH, HEIGHT, "Input test.");
  TouchableCollection tc = TouchableCollection();
  Gate andGate=Gate(&tc,{12,12});
  Gate notGate=Gate(&tc,{10,10},1);
  while (!WindowShouldClose()) {
    tc.click_update();
    andGate.mouseMoveUpdate();
    notGate.mouseMoveUpdate();
    Spline::SplinesDraw();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    andGate.draw();
    notGate.draw();
    Debugger::draw();
    EndDrawing();
  }
}
