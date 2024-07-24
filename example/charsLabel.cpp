#include "basic_template.h"
#include "gate.h"
#include "globals.h"
#include "raylib.h"
#include "ui.h"
#include <cstddef>
#include <memory>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  TouchableCollection tc(UsedCameraS::noCamera);
  InputBar ib(&tc, Rectangle{30, 30, 100, 50}, 14);
  ib.textPositionS = TextPositionS::center;
  InitWindow(WIDTH, HEIGHT, "Input test.");
  while (!WindowShouldClose()) {
    ib.TextUpdate();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    ib.draw();
    EndDrawing();
  }
}
