#include "ui.h"
#include "basic_template.h"
#include "globals.h"
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
Touchable::Touchable(TouchableCollection *const tc) : child_to(tc) {
  tc->push_back(this);
}
TouchableCollection::TouchableCollection(UsedCameraS camera) : _camera(camera) {}
bool Touchable::is_touching() {
  return child_to == nullptr ? false : this == child_to->_touching;
}
bool Touchable::is_selected() {
  return child_to == nullptr ? false : this == child_to->_lastSelected;
}
const Touchable *TouchableCollection::getSelected() { return _lastSelected; }
// void Touchable::add_to(TouchableCollection *tc) {
//   if (child_to != nullptr) {
//     child_to->erase(this);
//   }
//   child_to = tc;
//   tc->push_back(this);
// }
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

bool TouchableCollection::click_update() {
  using namespace GameManager;
  _touching = nullptr;
  Vector2 mouseWorldPos = getGlobalMousePosition(_camera);
  for (auto x : touchables) {
    if (x->_checkPointCollision(mouseWorldPos)) {
      _touching = x;
      break;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    _lastSelected = _touching;
  }
  return _touching != nullptr;
}
void InputBar::_cursor_move_right() {
  cursor_position = std::min(cursor_position + 1, (int)input_text.length());
}
void InputBar::_cursor_move_left() {
  cursor_position = std::max(0, cursor_position - 1);
}
void InputBar::_erase_front_char() { input_text.erase(cursor_position); }
void InputBar::_move_input() {
  if (IsKeyPressed(KEY_RIGHT))
    _cursor_move_right();
  if (IsKeyPressed(KEY_LEFT))
    _cursor_move_left();
}
bool InputBar::_char_input() {
  char input = GetCharPressed();
  if (input != '\0') {
    input_text.insert(input, cursor_position);
    cursor_position += 1;
    return true;
  }
  if (IsKeyPressed(KEY_BACKSPACE)) {
    _cursor_move_left();
    _erase_front_char();
    return true;
  }

  return false;
}
std::tuple<Chars, Color> InputBar::_rendered_text() {
  if (input_text.empty()) {
    return std::tuple(ref_text, ref_text_color);
  }
  auto text = Chars(input_text);
  text.insert('|', cursor_position);
  return std::tuple(text, BLACK);
}
bool InputBar::_checkPointCollision(Vector2 pos) {
  return CheckCollisionPointRec(pos, rect);
}

InputBar::InputBar(TouchableCollection *tc, Rectangle rect, float fontSize)
    : Touchable(tc), rect(rect), _fontSize(fontSize) {}
InputBar::InputBar(TouchableCollection *tc, Rectangle rect,
                   const Chars &ref_text, float fontSize)
    : Touchable(tc), rect(rect), ref_text(ref_text), _fontSize(fontSize) {}
InputBar::InputBar(TouchableCollection *tc, float x, float y, float width,
                   float height)
    : Touchable(tc), rect(Rectangle{x, y, width, height}) {}
bool InputBar::TextUpdate() {
  if (is_selected()) {    // is_clicked is a Touchable funciton.It checks
                          // if any element has been clicked or not.
    _move_input();        // move_input is to move cursor using arrow.
    return _char_input(); // char_input is for typing.It returns true
                          // when the input has been updated.
  }
  return false;
}
void InputBar::draw() {
  auto [text, color] = _rendered_text();
  drawRectangleWithLines(rect, WHITE, BLACK, is_selected() ? 2.0 : 1.0);
  auto textPos=textPosition(rect, text, textPositionS,_fontSize);
  RectSize rs=MeasureTextEx(Resource::Fonts::LUMITIVE_FONT,text.c_str(),_fontSize,0);
  // DrawTextEx(text, pos, _fontSize);
  using namespace Resource::Fonts;
  drawText(text, textPos, _fontSize);
  DrawRectangleLinesEx(rectFromPos(textPos,rs), 2, RED);
}
Chars InputBar::get_text() { return input_text; }
void Label::draw(float linewidth) {
  DrawRectangleGradientHRec(rect, colorL, colorR);
  DrawRectangleLinesEx(rect, linewidth, BLACK);
  Vector2 _textPos = textPosition(rect, text, textPos);
  drawText(text, _textPos, fontSize, BLACK);
}
bool Button::_checkPointCollision(Vector2 pos) {
  return CheckCollisionPointRec(pos, label.rect);
}
void Button::draw() { label.draw(is_touching() ? 2.0 : 1.0); }

bool SelectBar::_checkPointCollision(Vector2 pos) {
  Rectangle colrec = Rectangle{_position.x, _position.y, _rectSize.width,
                               _rectSize.height * float(options.size())};
  return CheckCollisionPointRec(pos, colrec);
}
void SelectBar::draw() {
  auto highlighted = options.size();
  if (is_touching()) {
    highlighted = (GetMouseY() - _position.y) / _rectSize.height;
  }
  for (usize i = 0; i < options.size(); i++) {
    Rectangle button_rect =
        Rectangle{_position.x, _position.y + float(i) * _rectSize.height,
                  _rectSize.width, _rectSize.height};
    auto btn = Label(button_rect, options[i],_textPosS);
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
void InputBar::setPos(Vector2 pos) {
  rect.x = pos.x;
  rect.y = pos.y;
}

Chars SearchBar::getClick() { return sb.getClick(); }
void SearchBar::draw() {
  ib.draw();
  sb.draw();
}
void SelectBar::setPos(Vector2 pos) { this->_position = pos; }
void SearchBar::setPos(Vector2 pos) {
  this->ib.setPos(pos);
  this->sb.setPos(pos + Vector2{0, this->ib.rect.height});
}
void Touchable::toSelected() { child_to->_lastSelected = this; }
void SearchBar::toSelected() { this->ib.toSelected(); }
bool TouchableCollection::isSelected() { return _lastSelected != nullptr; }
