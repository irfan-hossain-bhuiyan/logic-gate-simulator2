#include "basic_template.h"
#include "gate.h"
#include "raylib.h"
#include "ui.h"
#include <cstddef>
#include <memory>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;

  InitWindow(WIDTH, HEIGHT, "Input test.");
  TouchableCollection tc = TouchableCollection();
  Gate andGate = std::make_unique<AndGate>(&tc, Vector2{12, 12});
  Gate notGate = std::make_unique<NotGate>(&tc, Vector2{10, 10});
  Gate orGate = std::make_unique<OrGate>(&tc, Vector2{40, 40});
  Gates gates = make_vector<Gate>(std::move(andGate), std::move(notGate),
                                  std::move(orGate));
  while (!WindowShouldClose()) {
    tc.click_update();
    for(auto& x:gates)x->update();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    m_Spline::SplinesDraw();
    for(auto& x:gates)x->draw();
    Debugger::draw();
    EndDrawing();
  }
}
