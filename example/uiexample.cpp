#include "ui.h"
#include "basic_template.h"
#include "globals.h"
#include "raylib.h"
#include <cstddef>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  InitWindow(WIDTH, HEIGHT, "Input test.");
  TouchableCollection tc = TouchableCollection();
  auto choices = Vec<Chars>{"Ok", "Not ok", "It's good", "Abacus"};
  SearchBar sb(&tc, Vector2{30, 30}, choices);
  using namespace GameManager;
  while (!WindowShouldClose()) {
    tc.click_update();
    sb.CharUpdate();
    if (auto x = sb.getClick()) {
      Debugger::push_message(x);
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    sb.draw();
    Debugger::draw();
    EndDrawing();
  }
}
