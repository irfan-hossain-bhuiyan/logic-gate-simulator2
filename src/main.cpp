#include "basic_template.h"
#include "raylib.h"
#include "ui.h"
#include <cstddef>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  InitWindow(WIDTH, HEIGHT, "Input test.");
  TouchableCollection tc = TouchableCollection();
  Vec<Chars> options{"And", "Or", "Not", "Xor", "Nor", "Nand"};
  auto sb = SearchBar(&tc, Vector2{30, 40}, options);
  while (!WindowShouldClose()) {
    tc.click_update();
    sb.CharUpdate();
    if (auto a = sb.getClick()) {
      Debugger::push_message(a);
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    sb.draw();
    Debugger::draw();
    EndDrawing();
  }
}
