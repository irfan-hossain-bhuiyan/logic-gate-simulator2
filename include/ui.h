#pragma once
#include "basic_template.h"
#include "raylib.h"
#include <tuple>
#include <unordered_set>
#include <utility>
class TouchableCollection;

class Touchable {
public:
  virtual bool _checkPointCollision(Vector2 pos) = 0;
  Touchable(TouchableCollection *tc);
  Touchable(Touchable&)=delete;
  Touchable(Touchable&&)=delete;
  virtual ~Touchable();

  bool is_touching();
  bool is_selected();
  bool is_clicking();
  bool is_clicked();
  void add_to(TouchableCollection *tc);
  void toSelected();
  TouchableCollection *get_tc() const { return child_to; }

private:
  TouchableCollection *child_to = nullptr;
  friend class TouchableCollection;
};

class TouchableCollection {
	//If a object is selected or not is checked by touchable collection.
private:
  std::unordered_set<Touchable *> touchables;
  Touchable *touching = nullptr;
  Touchable *lastSelected = nullptr;

  void push_back(Touchable *touchable);
  void erase(Touchable *touchable);
public:
  bool isSelected();
  bool click_update();
  friend class Touchable;
};
class InputBar : Touchable {
private:
  Rectangle rect;
  Chars input_text = "";
  Chars ref_text = "";
  Color ref_text_color = GRAY;
  int cursor_position = 0;
  int _fontSize = 11;
  void _cursor_move_right();
  void _cursor_move_left();
  void _erase_front_char();
  void _move_input();
  bool _char_input();
  std::tuple<Chars, Color> _rendered_text();
  bool _checkPointCollision(Vector2 pos) override;
  void setPos(Vector2 pos);
public:
  InputBar(TouchableCollection *tc, Rectangle rect, int fontSize = 11);
  InputBar(TouchableCollection *tc, Rectangle rect, const Chars &ref_text,
           int fontSize = 11);
  InputBar(TouchableCollection *tc, float x, float y, float width,
           float height);
  bool TextUpdate();
  void draw();
  Chars get_text();
  friend class SearchBar;
};
class Label {
public:
  Rectangle rect;
  Chars text;
  Color colorL = WHITE;
  Color colorR = GRAY;
  int font_size = 11;
  void draw(float linewidth);
  Label(Rectangle rect, const Chars &text) : rect(rect), text(text) {}
  Label(Rectangle rect, const Chars &text, Color color)
      : rect(rect), text(text), colorL(color), colorR(color) {}
  Label(Rectangle rect, const Chars &text, Color colorL, Color colorR)
      : rect(rect), text(text), colorL(colorL), colorR(colorR) {}
  Label(Vector2 pos, RectSize rectSize, const Chars &text)
      : rect(rectFromPos(pos, rectSize)), text(text) {}
};
class Button : public Touchable {
public:
  Label label;

private:
  bool _checkPointCollision(Vector2 pos) override;
  void draw();

public:
  Button(TouchableCollection *tc, Label label)
      : Touchable(tc), label(std::move(label)) {}
  Button(TouchableCollection *tc, Rectangle rect, const Chars &text)
      : Touchable(tc), label(rect, text) {}
};
class SelectBar : public Touchable {
public:
  Vec<Chars> options;
  SelectBar(TouchableCollection *tc, Vector2 pos, Vec<Chars> &&options,
            float width, float height, int fontSize = 11)
      : Touchable(tc), options(options), _fontSize(fontSize), _position(pos),
        _rectSize(width, height) {}
  SelectBar(TouchableCollection *tc, Vector2 pos, Vec<Chars> options,
            RectSize rectSize, int fontSize = 11)
      : Touchable(tc), options(std::move(options)), _fontSize(fontSize),
        _position(pos), _rectSize(rectSize) {}

private:
  int _fontSize;
  Vector2 _position;
  RectSize _rectSize;
  bool _checkPointCollision(Vector2 pos) override;

public:
  void draw();
  Chars getClick(); // Returns an empty Chars if nothing is clicked or selected.
  void setPos(Vector2 pos);
};
class SearchBar {
private:
  InputBar ib;
  SelectBar sb;
  Vec<Chars> options;
  Vec<Chars> filtered_options;

public:
  SearchBar(TouchableCollection *tc, Vector2 position,
            const Vec<Chars> &options, int fontSize = 11, float width = 50,
            float height = 30)
      : ib(tc, rectFromPos(position, width, height), fontSize),
        sb(tc, position + Vector2{0, height}, Vec<Chars>(options), width,
           height),
        options(options), filtered_options(options) {}
  void CharUpdate();
  Chars getClick();
  void draw();
  void setPos(Vector2 pos);
  void toSelected();
};
