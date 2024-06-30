#include "./basic_template.cpp"
#include "./ui.cpp"
#include "raylib.h"
#include "ui.h"
#include <cstddef>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  InitWindow(WIDTH, HEIGHT, "Input test.");
  TouchableCollection tc = TouchableCollection();
  Vec<Chars> options{"And", "Or", "Not", "Xor", "Nor", "Nand"};
  SelectBar sb = SelectBar(&tc, options, Vector2{50, 30});
  while (!WindowShouldClose()) {
    tc.click_update();
    if (auto a = sb.getClick(); !a.empty()) {
      Debugger::push_message(a);
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    sb.draw();
    Debugger::draw();
    EndDrawing();
  }
}
