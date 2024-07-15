#include "globals.h"
#include "raylib.h"
#include <cstddef>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;

  InitWindow(WIDTH, HEIGHT, "Input test.");
  using namespace GameManager;
  while (!WindowShouldClose()) {
    GameManager::tcUpdate();
    UI::update();
    GateWindow::update();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(GateWindow::getCamera());
    GateWindow::draw();
    EndMode2D();
    UI::draw();
    Debugger::draw();
    EndDrawing();
  }
}
