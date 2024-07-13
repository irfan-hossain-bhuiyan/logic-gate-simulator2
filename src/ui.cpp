#include "ui.h"
#include "basic_template.h"
#include "gate.h"
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
bool Touchable::is_selected() {
  return child_to == nullptr ? false : this == child_to->last_click;
}
void Touchable::add_to(TouchableCollection *tc) {
  if (child_to != nullptr) {
    child_to->erase(this);
  }
  child_to = tc;
  tc->push_back(this);
}
bool Touchable::is_clicked() {
  return is_selected() &&
         IsMouseButtonPressed(MOUSE_BUTTON_LEFT); // This works because for
                                                  // each frame if MousePressed
                                                  // function returns true,for
                                                  // every frame it called.
}
bool Touchable::is_clicking() {
  return is_touching() && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
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
    if (x->checkPointCollision(mouse_pos)) {
      touching = x;
      break;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    last_click = touching;
  }
}
void InputBar::cursor_move_right() {
  cursor_position = std::min(cursor_position + 1, (int)input_text.length());
}
void InputBar::cursor_move_left() {
  cursor_position = std::max(0, cursor_position - 1);
}
void InputBar::erase_front_char() { input_text.erase(cursor_position); }
void InputBar::move_input() {
  if (IsKeyPressed(KEY_RIGHT))
    cursor_move_right();
  if (IsKeyPressed(KEY_LEFT))
    cursor_move_left();
}
bool InputBar::char_input() {
  char input = GetCharPressed();
  if (input != '\0') {
    input_text.insert(input, cursor_position);
    cursor_position += 1;
    return true;
  }
  if (IsKeyPressed(KEY_BACKSPACE)) {
    cursor_move_left();
    erase_front_char();
    return true;
  }

  return false;
}
std::tuple<Chars, Color> InputBar::rendered_text() {
  if (input_text.empty()) {
    return std::tuple(ref_text, ref_text_color);
  }
  auto text = Chars(input_text);
  text.insert('|', cursor_position);
  return std::tuple(text, BLACK);
}
bool InputBar::checkPointCollision(Vector2 pos) {
  return CheckCollisionPointRec(pos, rect);
}

InputBar::InputBar(TouchableCollection *tc, Rectangle rect, int fontSize)
    : Touchable(tc), rect(rect), _fontSize(fontSize) {}
InputBar::InputBar(TouchableCollection *tc, Rectangle rect,
                   const Chars &ref_text, int fontSize)
    : Touchable(tc), rect(rect), ref_text(ref_text), _fontSize(fontSize) {}
InputBar::InputBar(TouchableCollection *tc, float x, float y, float width,
                   float height)
    : Touchable(tc), rect(Rectangle{x, y, width, height}) {}
bool InputBar::TextUpdate() {
  if (is_selected()) {   // is_clicked is a Touchable funciton.It checks if any
                         // element has been clicked or not.
    move_input();        // move_input is to move cursor using arrow.
    return char_input(); // char_input is for typing.It returns true when the
                         // input has been updated.
  }
  return false;
}
void InputBar::draw() {
  auto [text, color] = rendered_text();
  drawRectangleWithLines(rect, WHITE, BLACK, is_selected() ? 2.0 : 1.0);
  Vector2 pos = rect_pos(rect);
  pos += Vector2{5, 5};
  DrawText(text, pos, _fontSize);
}
Chars InputBar::get_text() { return input_text; }
void Label::draw(float linewidth) {
  DrawRectangleGradientHRec(rect, colorL, colorR);
  DrawRectangleLinesEx(rect, linewidth, BLACK);
  Vector2 center = middle(rect);
  float text_width = float(MeasureText(text.c_str(), font_size));
  float text_height = float(font_size);
  Vector2 text_pos = center - Vector2{text_width, text_height} / 2;
  DrawText(text, text_pos, font_size, BLACK);
}
bool Button::checkPointCollision(Vector2 pos) {
  return CheckCollisionPointRec(pos, label.rect);
}
void Button::draw() { label.draw(is_touching() ? 2.0 : 1.0); }

bool SelectBar::checkPointCollision(Vector2 pos) {
  Rectangle colrec = Rectangle{_position.x, _position.y, _rectSize.width,
                               _rectSize.height * float(options.size())};
  return CheckCollisionPointRec(pos, colrec);
}
void SelectBar::draw() {
  auto highlighted = options.size();
  if (is_touching()) {
    highlighted = ((float)GetMouseY() - _position.y) / _rectSize.height;
  }
  for (usize i = 0; i < options.size(); i++) {
    Rectangle button_rect =
        Rectangle{_position.x, _position.y + float(i) * _rectSize.height,
                  _rectSize.width, _rectSize.height};
    auto btn = Label(button_rect, options[i]);
    btn.draw(i == highlighted ? 3.0 : 1.0);
  }
}
Chars SelectBar::getClick() {
  if (is_clicked()) {
    int highlighted =
        int((float(GetMouseY()) - _position.y) / _rectSize.height);
    return options[highlighted];
  }
  return Chars();
}

void SearchBar::CharUpdate() {
  auto changed = ib.TextUpdate();
  if (changed) {
    filtered_options = fuzzySearch(ib.get_text(), options);
    sb.options = filtered_options;
  }
}
Chars SearchBar::getClick() { return sb.getClick(); }
void SearchBar::draw() {
  ib.draw();
  sb.draw();
}
// namespace Debugger
//
namespace GameManager {
void _removeCurrentSpline() {

  delete m_Spline::CURRENT_SPLINE;
  m_Spline::CURRENT_SPLINE = nullptr;
}

void _onRightClick() {
  if (m_Spline::CURRENT_SPLINE != nullptr) {
    _removeCurrentSpline();
    return;
  }
}

void mainUpdate() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    _onRightClick();
  }
}
} // namespace GameManager
  //
namespace GameManager::Debugger {
BoundedQueue<Chars> bq(10);
Vector2 positon = Vector2{10, 10};
int textspace = 3;
int fontSize = 11;
void draw() {
  for (usize i = 0; i < bq.size(); i++) {
    float y_offset = float(textspace + fontSize) * i;
    auto pos = positon + Vector2{0, y_offset};
    DrawText(bq.at(i), pos);
  }
}
void push_message(const Chars &&text) { bq.push(text); }
void push_message(const Chars &text) { bq.push(text); }
} // namespace GameManager::Debugger
  //
namespace GameManager::UI {
TouchableCollection tc;
// SelectBar sb(&tc,{0,0});
void draw(){
	
}
void update(){

}
} // namespace GameManager::UI
namespace GameManager::GateWindow {
TouchableCollection tc;
Gates gates;
Camera2D camera2d{
    .offset = {0, 0},
    .target = {0, 0},
    .rotation = 0.0,
    .zoom = 1.0,
};
void draw() {
  for (auto &x : gates)
    x->draw();
}
void update() {
  for (auto &x : gates)
    x->update();
}
// Usage:
} // namespace GameManager::GateWindow
