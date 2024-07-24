#include "globals.h"
#include "raylib.h"
#include <cstddef>
#include <iostream>
int main() {
  const int WIDTH = 800;
  const int HEIGHT = 600;
  InitWindow(WIDTH, HEIGHT, "Input test.");
  GameManager::init();
  Vector2 x=MeasureTextEx(Resource::Fonts::LUMITIVE_FONT,"It's working",11, 2);
  std::cout<<x.x <<x.y;

}
