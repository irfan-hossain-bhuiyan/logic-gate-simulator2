#include "globals.hpp"
#include "raylib.h"
#include <cstddef>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);

  InitWindow(WIDTH, HEIGHT, "Input test.");
  using namespace GameManager;
  init();
  while (!WindowShouldClose()) {
    GameManager::update();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(GateWindow::getCamera());
    GateWindow::draw();
    EndMode2D();
    //Debugger::messages<<"M:"<<GetMousePosition()<<end;
    UI::draw();
    Debugger::draw();
    EndDrawing();
  }
}
