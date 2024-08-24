#pragma once
#include "basic_template.hpp"
#include <raylib.h>
#include "touch.hpp"
#include <tuple>
#include <utility>

enum class TextPositionS { left, right, center };
class InputBar : Touchable {
private:
  Rectangle rect;
  Chars input_text = "";
  Chars ref_text = "";
  Color ref_text_color = GRAY;
  int cursor_position = 0;
  float _fontSize = DEFAULT_FONT_SIZE;
  void _cursor_move_right();
  void _cursor_move_left();
  void _erase_front_char();
  void _move_input();
  bool _char_input();
  std::tuple<Chars, Color> _rendered_text();
  void setPos(Vector2 pos);

public:
  TextPositionS textPositionS = TextPositionS::left;
  InputBar(Rectangle rect, float fontSize = DEFAULT_FONT_SIZE);
  InputBar(Rectangle rect, const Chars &ref_text,
           float fontSize = DEFAULT_FONT_SIZE);
  InputBar(float x, float y, float width, float height);
  bool TextUpdate(const UGS &tc);
  void draw(const UGS &tc);
  void reset();
  Chars get_text();
  const Id checkPointCollision(Vector2 pos) const override;
  friend class SearchBar;
};
class Label {
public:
  Rectangle rect;
  Chars text;
  TextPositionS textPos;
  Color colorL = WHITE;
  Color colorR = GRAY;
  float fontSize = DEFAULT_FONT_SIZE;
  float border = 3;
  void draw(float linewidth) const;
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

  const Id checkPointCollision(Vector2 pos) const override;
private:
  void draw(const UGS &);

public:
  Button(Label label); 
  Button(Rectangle rect, const Chars &text);
};
class SelectBar : public Touchable {
public:
  Vec<Chars> options;
  SelectBar(Vector2 pos, Vec<Chars> &&options, float width, float height,
            float fontSize = DEFAULT_FONT_SIZE,
            TextPositionS textPosS = TextPositionS::center)
      : options(options), _textPosS(textPosS), _fontSize(fontSize),
        _position(pos), _rectSize(width, height) {}
  SelectBar(Vector2 pos, Vec<Chars> options, RectSize rectSize,
            float fontSize = DEFAULT_FONT_SIZE,
            TextPositionS textPosS = TextPositionS::center)
      : options(std::move(options)), _textPosS(textPosS), _fontSize(fontSize),
        _position(pos), _rectSize(rectSize) {}

public:
  const Id checkPointCollision(Vector2 pos) const override;

private:
  const TextPositionS _textPosS;
  float _fontSize;
  Vector2 _position;
  RectSize _rectSize;

public:
  void draw(const UGS &) const;
  Chars getClick(const UGS &) const; // Returns an empty Chars
                                     // if nothing is clicked or selected.
  void setPos(Vector2 pos);
};
class SearchBar : public Touchable {
private:
  InputBar ib;
  SelectBar sb;
  Vec<Chars> options;
  Vec<Chars> filtered_options;

public:
  SearchBar(Vector2 position, const Vec<Chars> &options,
            float fontSize = DEFAULT_FONT_SIZE, float width = 50,
            float height = 30)
      : ib(rectFromPos(position, width, height), fontSize),
        sb(position + Vector2{0, height}, Vec<Chars>(options), width, height),
        options(options), filtered_options(options) {
    ib.textPositionS = TextPositionS::center;
  }
  void CharUpdate(const UGS &);
  Chars getClick(const UGS &);
  void draw(const UGS &);
  void setPos(Vector2 pos);
  void toSelected(UGS &);
  const Id checkPointCollision(Vector2 pos) const override;
};
