#include "./basic_template.cpp"
#include "raylib.h"
#include <algorithm>
#include <tuple>
class InputBar {
private:
  Rectangle rect;
  bool is_active=true;
  Chars input_text="";
  Chars ref_text="";
  Color ref_text_color = GRAY;
  int cursor_position=0;
  void cursor_move_right() {
    cursor_position = std::min(cursor_position + 1, (int)input_text.length());
  }
  void cursor_move_left() {
    cursor_position = std::max(0, cursor_position - 1);
  }
  void erase_front_char() { input_text.erase(cursor_position); }
  void move_input() {
    if (IsKeyPressed(KEY_RIGHT))
      cursor_move_right();
    if (IsKeyPressed(KEY_LEFT))
      cursor_move_left();
    if (IsKeyPressed(KEY_BACKSPACE)) {
      cursor_move_left();
      erase_front_char();
    }
  }
  void char_input() {
    char input = GetCharPressed();
    if (input != '\0') {
      input_text.insert(input, cursor_position);
      cursor_position += 1;
    }
  }
  std::tuple<Chars, Color> rendered_text() {
    if (input_text.empty()) {
      return std::tuple(ref_text, ref_text_color);
    }
    auto text=Chars(input_text);
    text.insert('|', cursor_position);
    return std::tuple(text, BLACK);
  }

public:
  InputBar(Rectangle rect) : rect(rect) {}
  InputBar(Rectangle rect, Chars ref_text) : rect(rect), ref_text(ref_text) {}
  InputBar(float x,float y,float width,float height):rect(Rectangle{x,y,width,height}){}
  void take_input() {
    if (is_active) {
      move_input();
      char_input();
    }
  }
  void draw() {
    auto [text, color] = rendered_text();
    DrawRectangleWithLines(rect,WHITE,BLACK,6);
    Vector2 pos = rect_pos(rect);
    pos += Vector2{5, 5};
    DrawText(text.c_str(), pos);
  }
};

int main() {
	const int WIDTH=800;
	const int HEIGHT=600;
	InitWindow(WIDTH,HEIGHT,"Input test.");
	InputBar inputbar=InputBar(10,10,60,30);
	while(!WindowShouldClose()){
		inputbar.take_input();
		BeginDrawing();
		ClearBackground(RAYWHITE);
		inputbar.draw();
		EndDrawing();
	}
}
