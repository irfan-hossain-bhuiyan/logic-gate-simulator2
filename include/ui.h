#pragma once
#include "basic_template.h"
#include "raylib.h"
#include <algorithm>
#include <tuple>
#include <unordered_set>
#include <vector>
class TouchableCollection;

class Touchable {
public:
  virtual bool checkPointCollision(Vector2 pos) = 0;
  Touchable(TouchableCollection *tc);
  virtual ~Touchable();

  bool is_touching();
  bool is_selected();
  bool is_clicking();
  bool is_clicked();
  void add_to(TouchableCollection *tc);
  TouchableCollection* get_tc(){return child_to;}
private:
  TouchableCollection *child_to = nullptr;
  friend class TouchableCollection;
};

class TouchableCollection {
private:
  std::unordered_set<Touchable *> touchables;
  Touchable *touching = nullptr;
  Touchable *last_click = nullptr;

  void push_back(Touchable *touchable);
  void erase(Touchable *touchable);
  bool no_selected(){return last_click==nullptr;}
public:
  void click_update();
  friend class Touchable;
};
class InputBar : Touchable {
private:
  Rectangle rect;
  Chars input_text = "";
  Chars ref_text = "";
  Color ref_text_color = GRAY;
  int cursor_position = 0;
  void cursor_move_right();
  void cursor_move_left();
  void erase_front_char();
  void move_input();
  bool char_input();
  std::tuple<Chars, Color> rendered_text();
  bool checkPointCollision(Vector2 pos) override;

public:
  InputBar(TouchableCollection *tc, Rectangle rect);
  InputBar(TouchableCollection *tc, Rectangle rect, Chars ref_text);
  InputBar(TouchableCollection *tc, float x, float y, float width,
           float height);
  bool TextUpdate();
  void draw();
  Chars get_text();
};
class Label {
public:
  Rectangle rect;
  Chars text;
  Color colorL = WHITE;
  Color colorR = GRAY;
  int font_size = 11;
  void draw(int linewidth);
  Label(Rectangle rect, Chars text) : rect(rect), text(text) {}
  Label(Rectangle rect, Chars text, Color color)
      : rect(rect), text(text), colorL(color), colorR(color) {}
  Label(Rectangle rect, Chars text, Color colorL, Color colorR)
      : rect(rect), text(text), colorL(colorL), colorR(colorR) {}
};
class Button : public Touchable {
public:
  Label label;

private:
  bool checkPointCollision(Vector2 pos) override;
  void draw();

public:
  Button(TouchableCollection *tc, Label label) : Touchable(tc), label(label) {}
  Button(TouchableCollection *tc, Rectangle rect, Chars text)
      : Touchable(tc), label(rect, text) {}
};
class SelectBar : public Touchable {
public:
  Vec<Chars> options;
  Vector2 position;
  RectSize rectSize;
  bool active;

  SelectBar(TouchableCollection *tc, Vector2 pos, Vec<Chars> options,
            float width, float height)
      : Touchable(tc), options(options), position(pos),
        rectSize(width, height) {}
  SelectBar(TouchableCollection *tc, Vector2 pos, Vec<Chars> options,
            RectSize rectSize)
      : Touchable(tc), options(options), position(pos), rectSize(rectSize) {}

private:
  bool checkPointCollision(Vector2 pos) override;

public:
  void draw();
  Chars getClick();
};
class SearchBar {
private:
  InputBar ib;
  SelectBar sb;
  Vec<Chars> options;
  Vec<Chars> filtered_options;

public:
  SearchBar(TouchableCollection *tc, Vector2 position, const Vec<Chars>& options,
            int fontSize = 11, float width = 50, float height = 30)
      : ib(tc, rectFromPos(position, width, height)),
        sb(tc, position + Vector2{0, height}, options, width, height),
        options(options), filtered_options(options) {}
  void CharUpdate();
  Chars getClick();
  void draw();
};
namespace Debugger {
	extern BoundedQueue<Chars> bq;
extern Vector2 positon;
extern int textspace;
extern int fontSize;

void push_message(Chars text);
void draw();
} // namespace Debugger
