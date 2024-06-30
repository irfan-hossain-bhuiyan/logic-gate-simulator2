#include "./ui.h"
#include "./basic_template.cpp"
#include "raylib.h"
#include <algorithm>
#include <cstddef>
#include <tuple>
#include <unordered_set>
Touchable::~Touchable() {
  if (child_to != nullptr) {
    child_to->erase(this);
  }
}
Touchable::Touchable(TouchableCollection *tc) {
  child_to = tc;
  tc->push_back(this);
}
bool Touchable::is_touching() {
  return child_to == nullptr ? false : this == child_to->touching;
}
bool Touchable::is_clicked() {
  return child_to == nullptr ? false : this == child_to->last_click;
}
void Touchable::add_to(TouchableCollection *tc) {
  if (child_to != nullptr) {
    child_to->erase(this);
  }
  child_to = tc;
  tc->push_back(this);
}
bool Touchable::is_clickedone() {
  return is_clicked() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
void TouchableCollection::push_back(Touchable *touchable) {
  touchables.insert(touchable);
}
void TouchableCollection::erase(Touchable *touchable) {
  touchables.erase(touchable);
}

void TouchableCollection::click_update() {
  touching = nullptr;
  auto mouse_pos = GetMousePosition();
  for (auto x : touchables) {
    if (x->CollisionPoint(mouse_pos)) {
      touching = x;
      break;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    last_click = touching;
  }
}
class InputBar : Touchable {
private:
  Rectangle rect;
  Chars input_text = "";
  Chars ref_text = "";
  Color ref_text_color = GRAY;
  int cursor_position = 0;
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
    auto text = Chars(input_text);
    text.insert('|', cursor_position);
    return std::tuple(text, BLACK);
  }
  bool CollisionPoint(Vector2 pos) override {
    return CheckCollisionPointRec(pos, rect);
  }

public:
  InputBar(TouchableCollection *tc, Rectangle rect)
      : Touchable(tc), rect(rect) {}
  InputBar(TouchableCollection *tc, Rectangle rect, Chars ref_text)
      : Touchable(tc), rect(rect), ref_text(ref_text) {}
  InputBar(TouchableCollection *tc, float x, float y, float width, float height)
      : Touchable(tc), rect(Rectangle{x, y, width, height}) {}
  void take_input() {
    if (is_clicked()) {
      move_input();
      char_input();
    }
  }
  void draw() {
    auto [text, color] = rendered_text();
    DrawRectangleWithLines(rect, WHITE, BLACK, is_clicked() ? 2 : 1);
    Vector2 pos = rect_pos(rect);
    pos += Vector2{5, 5};
    DrawText(text, pos);
  }
};
class Label {
public:
  Rectangle rect;
  Chars text;
  int font_size=11;
  void draw(int linewidth) {
    DrawRectangleGradientHRec(rect, WHITE, GRAY);
    DrawRectangleLinesEx(rect, linewidth, BLACK);
    Vector2 center = middle(rect);
    float text_width=MeasureText(text.c_str(), font_size);
    float text_height=font_size;
    Vector2 text_pos=center-Vector2{text_width,text_height}/2;
    DrawText(text, text_pos, font_size, BLACK);
  }
  Label(Rectangle rect, Chars text) : rect(rect), text(text) {}
};
class Button : public Touchable {
public:
  Label label;

private:
  bool CollisionPoint(Vector2 pos) override {
    return CheckCollisionPointRec(pos, label.rect);
  }
  void draw() { label.draw(is_touching() ? 2 : 1); }

public:
  Button(TouchableCollection *tc, Label label) : Touchable(tc), label(label) {}
  Button(TouchableCollection *tc, Rectangle rect, Chars text)
      : Touchable(tc), label(rect, text) {}
};
class SelectBar : public Touchable {
public:
  Vec<Chars> options;
  Vector2 position;
  float width;
  float height;
  bool active;

  SelectBar(TouchableCollection *tc, Vec<Chars> options, Vector2 pos,
            float width = 60, float height = 30)
      : Touchable(tc), options(options), position(pos), width(width),
        height(height) {}

private:
  bool CollisionPoint(Vector2 pos) override {
    Rectangle colrec =
        Rectangle{position.x, position.y, width, height * options.size()};
    return CheckCollisionPointRec(pos, colrec);
  }
public:
  void draw() {
    int highlighted = options.size();
    if (is_touching()) {
      highlighted = (GetMouseY() - position.y) / height;
    }
    range(i, 0, options.size()) {
      Rectangle button_rect =
          Rectangle{position.x, position.y + i * height, width, height};
      auto btn = Label(button_rect, options[i]);
      btn.draw(i == highlighted ? 2 : 1);
    }
  }
  Chars getClick() {
    if (is_clickedone()) {
      int highlighted = (GetMouseY() - position.y) / height;
      return options[highlighted];
    }
    return Chars();
  }
};

class SearchBar{
	private:
		InputBar ib;
		SelectBar sb;
};
namespace Debugger {
BoundedQueue<Chars> bq(10);
Vector2 positon = Vector2{10, 10};
int textspace = 3;
int fontSize = 11;
void push_message(Chars text) { bq.push(text); }
void draw() {
  for (int i = 0; i < bq.size(); i++) {
    float y_offset = (textspace + fontSize) * i;
    auto pos = positon + Vector2{0, y_offset};
    DrawText(bq.at(i), pos);
  }
}
} // namespace Debugger

