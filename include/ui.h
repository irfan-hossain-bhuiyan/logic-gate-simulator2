#pragma once
#include "basic_template.h"
#include "globals.h"
#include "raylib.h"
#include <tuple>
#include <unordered_set>
#include <utility>
class TouchableCollection;

using namespace GameManager;
class Touchable {
public:
  virtual bool _checkPointCollision(Vector2 pos) = 0;
  Touchable(TouchableCollection *const tc);
  Touchable(Touchable &) = delete;
  Touchable(Touchable &&) = delete;
  virtual ~Touchable();

  bool is_touching();
  bool is_selected();
  bool is_clicking();
  bool is_clicked();
  void toSelected();
  TouchableCollection *get_tc() const { return child_to; }

private:
  TouchableCollection *const child_to;
  friend class TouchableCollection;
};

class TouchableCollection {
  // If a object is selected or not is checked by touchable collection.
private:
  std::unordered_set<Touchable *> touchables;
  Touchable *_touching = nullptr;
  Touchable *_lastSelected = nullptr;
  const UsedCameraS _camera;

  void push_back(Touchable *touchable);
  void erase(Touchable *touchable);

public:
  TouchableCollection(UsedCameraS camera);
  bool isSelected();
  bool click_update();
  const Touchable *getSelected();
  friend class Touchable;
  friend class Draggable;
};

enum class TextPositionS { left, right, center };
class InputBar : Touchable {
private:
  Rectangle rect;
  Chars input_text = "";
  Chars ref_text = "";
  Color ref_text_color = GRAY;
  int cursor_position = 0;
  float _fontSize = 11.0f;
  void _cursor_move_right();
  void _cursor_move_left();
  void _erase_front_char();
  void _move_input();
  bool _char_input();
  std::tuple<Chars, Color> _rendered_text();
  bool _checkPointCollision(Vector2 pos) override;
  void setPos(Vector2 pos);

public:
  TextPositionS textPositionS=TextPositionS::left;
  InputBar(TouchableCollection *tc, Rectangle rect, float fontSize = 11);
  InputBar(TouchableCollection *tc, Rectangle rect, const Chars &ref_text,
           float fontSize = 11);
  InputBar(TouchableCollection *tc, float x, float y, float width,
           float height);
  bool TextUpdate();
  void draw();
  void reset();
  Chars get_text();
  friend class SearchBar;
};
class Label {
public:
  Rectangle rect;
  Chars text;
  TextPositionS textPos;
  Color colorL = WHITE;
  Color colorR = GRAY;
  float fontSize = 11.0f;
  float border = 3;
  void draw(float linewidth);
  Label(Rectangle rect, const Chars &text,
        TextPositionS textpos = TextPositionS::center)
      : rect(rect), text(text), textPos(textpos) {}
  Label(Rectangle rect, const Chars &text, Color color,
        TextPositionS textPos = TextPositionS::center)
      : rect(rect), text(text), textPos(textPos), colorL(color), colorR(color) {
  }
  Label(Rectangle rect, const Chars &text, Color colorL, Color colorR,
        TextPositionS textPos = TextPositionS::center)
      : rect(rect), text(text), textPos(textPos), colorL(colorL),
        colorR(colorR) {}
  Label(Vector2 pos, RectSize rectSize, const Chars &text,
        TextPositionS textPos = TextPositionS::center)
      : rect(rectFromPos(pos, rectSize)), text(text), textPos(textPos) {}
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
            float width, float height, float fontSize = 11,TextPositionS textPosS=TextPositionS::center)
      : Touchable(tc), options(options), _textPosS(textPosS), _fontSize(fontSize),
        _position(pos),_rectSize(width, height) {}
  SelectBar(TouchableCollection *tc, Vector2 pos, Vec<Chars> options,
            RectSize rectSize, float fontSize = 11,TextPositionS textPosS=TextPositionS::center)
      : Touchable(tc), options(std::move(options)), _textPosS(textPosS),
        _fontSize(fontSize), _position(pos),_rectSize(rectSize) {}

private:
  const TextPositionS _textPosS;
  float _fontSize;
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
            const Vec<Chars> &options, float fontSize = 11, float width = 50,
            float height = 30)
      : ib(tc, rectFromPos(position, width, height), fontSize),
        sb(tc, position + Vector2{0, height}, Vec<Chars>(options), width,
           height),
        options(options), filtered_options(options) {
		ib.textPositionS=TextPositionS::center;
	}
  void CharUpdate();
  Chars getClick();
  void draw();
  void setPos(Vector2 pos);
  void toSelected();
};
