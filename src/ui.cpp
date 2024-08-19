#include "ui.h"
#include "basic_template.h"
#include "globals.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <tuple>
// Touchable::~Touchable() {
//   if (child_to != nullptr) {
//     child_to->erase(this);
//   }
// }
// Touchable::Touchable(TouchableCollection *const tc) : child_to(tc) {
//   tc->push_back(this);
// }
// TouchableCollection::TouchableCollection(UsedCameraS camera) :
// _camera(camera) {}
// void Touchable::add_to(TouchableCollection *tc) {
//   if (child_to != nullptr) {
//     child_to->erase(this);
//   }
//   child_to = tc;
//   tc->push_back(this);
// }
// void TouchableCollection::push_back(Touchable *touchable) {
//   touchables.insert(touchable);
// }
// void TouchableCollection::erase(Touchable *touchable) {
//   touchables.erase(touchable);
// }

// bool TouchableCollection::click_update() {
//   using namespace GameManager;
//   _touching = nullptr;
//   Vector2 mouseWorldPos = getGlobalMousePosition(_camera);
//   for (auto x : touchables) {
//     if (x->_checkPointCollision(mouseWorldPos)) {
//       _touching = x;
//       break;
//     }
//   }
//   if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//     _lastSelected = _touching;
//   }
//   return _touching != nullptr;
// }
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
const Touchable *InputBar::checkPointCollision(Vector2 pos) const {
  return CheckCollisionPointRec(pos, rect) ? (Touchable *)this : nullptr;
}

InputBar::InputBar(Rectangle rect, float fontSize)
    : rect(rect), _fontSize(fontSize) {}
InputBar::InputBar(Rectangle rect, const Chars &ref_text, float fontSize)
    : rect(rect), ref_text(ref_text), _fontSize(fontSize) {}
InputBar::InputBar(float x, float y, float width, float height)
    : rect(Rectangle{x, y, width, height}) {}
bool InputBar::TextUpdate(const UGS &tc) {
  if (is_selected(tc)) {  // is_clicked is a Touchable funciton.It checks
                          // if any element has been clicked or not.
    _move_input();        // move_input is to move cursor using arrow.
    return _char_input(); // char_input is for typing.It returns true
                          // when the input has been updated.
  }
  return false;
}
void InputBar::draw(const UGS &tc) {
  auto [text, color] = _rendered_text();
  drawRectangleWithLines(rect, WHITE, BLACK, is_selected(tc) ? 2.0 : 1.0);
  auto textPos = textPosition(rect, text, textPositionS, _fontSize);
  RectSize rs =
      MeasureTextEx(Resource::Fonts::LUMITIVE_FONT, text.c_str(), _fontSize, 0);
  // DrawTextEx(text, pos, _fontSize);
  using namespace Resource::Fonts;
  drawText(text, textPos, _fontSize);
  DrawRectangleLinesEx(rectFromPos(textPos, rs), 2, RED);
}
Chars InputBar::get_text() { return input_text; }
void Label::draw(float linewidth) const {
  DrawRectangleGradientHRec(rect, colorL, colorR);
  DrawRectangleLinesEx(rect, linewidth, BLACK);
  Vector2 _textPos = textPosition(rect, text, textPos);
  drawText(text, _textPos, fontSize, BLACK);
}
const Touchable *Button::checkPointCollision(Vector2 pos) const {
  return CheckCollisionPointRec(pos, label.rect) ? (Touchable *)this : nullptr;
}
void Button::draw(const UGS &tc) { label.draw(is_touching(tc) ? 2.0 : 1.0); }

const Touchable *SelectBar::checkPointCollision(Vector2 pos) const {
  Rectangle colrec = Rectangle{_position.x, _position.y, _rectSize.width,
                               _rectSize.height * float(options.size())};
  return CheckCollisionPointRec(pos, colrec) ? (Touchable *)this : nullptr;
}
void SelectBar::draw(const UGS &tc) const {
  auto highlighted = options.size();
  if (is_touching(tc)) {
    highlighted = (float(GetMouseY()) - _position.y) / _rectSize.height;
  }
  for (usize i = 0; i < options.size(); i++) {
    Rectangle button_rect =
        Rectangle{_position.x, _position.y + float(i) * _rectSize.height,
                  _rectSize.width, _rectSize.height};
    auto btn = Label(button_rect, options[i], _textPosS);
    btn.draw(i == highlighted ? 3.0 : 1.0);
  }
}
Chars SelectBar::getClick(const UGS &tc) const {
  if (is_clicked(tc)) {
    int highlighted =
        (float(GetMouseY()) - _position.y) / _rectSize.height;

    return options[highlighted];
  }
  return Chars();
}

void SearchBar::CharUpdate(const UGS &tc) {
  auto changed = ib.TextUpdate(tc);
  if (changed) {
    filtered_options = fuzzySearch(ib.get_text(), options);
    sb.options = filtered_options;
  }
}
void InputBar::setPos(Vector2 pos) {
  rect.x = pos.x;
  rect.y = pos.y;
}

Chars SearchBar::getClick(const UGS &tc) { return sb.getClick(tc); }
void SearchBar::draw(const UGS &tc) {
  ib.draw(tc);
  sb.draw(tc);
}
void SelectBar::setPos(Vector2 pos) { this->_position = pos; }
void SearchBar::setPos(Vector2 pos) {
  this->ib.setPos(pos);
  this->sb.setPos(pos + Vector2{0, this->ib.rect.height});
}
// void Touchable::toSelected() { child_to->_lastSelected = this; }
void SearchBar::toSelected(UGS &tc) { tc.toSelect(this->ib); }
const Touchable *SearchBar::checkPointCollision(Vector2 pos) const {
  if (const Touchable *collided = sb.checkPointCollision(pos))
    return collided;
  if (const Touchable *collided = ib.checkPointCollision(pos))
    return collided;
  return nullptr;
}
