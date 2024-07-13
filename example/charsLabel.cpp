#include "basic_template.h"
#include "gate.h"
#include "raylib.h"
#include "ui.h"
#include <cstddef>
#include <memory>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  TouchableCollection tc;
  InitWindow(WIDTH, HEIGHT, "Input test.");
  while (!WindowShouldClose()) {
    Chars ch0;
    Chars ch1="NANo";
    Chars ch2 = "NO NO";
    Vec<Chars> cv{ch0,ch1,ch2};
    RectSize rs={50,20};
    Label l0({20,20},rs,ch0);
    Label l1({40,50},rs,ch1);
    BeginDrawing();
    ClearBackground(RAYWHITE);
    l0.draw(3);
    l1.draw(2);
    EndDrawing();
  }
}
