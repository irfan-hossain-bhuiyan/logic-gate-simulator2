#pragma once
#include "basic_template.h"
#include "globals.h"
#include "object.h"
#include "ui.h"
#include <raylib.h>
#include <span>
#include <unordered_set>
enum class GatePointState { in, out };
class m_Spline;
class m_Gate;
using Gate = Box<m_Gate>;
using Gates = Vec<Gate>;
using GPs = GatePointState;
// GatePoint is a helper class.
// It is clickable.
// It follows Gate.So gatepoint is a child.
// It takes a spline.
template <GPs STATE> class m_GatePoint;
template <GPs STATE> using GatePoint = m_GatePoint<STATE>;
// Because both of them are mutable reference of the function
template <GPs STATE> void attach(m_GatePoint<STATE> &gp, m_Spline &sp);
template <GPs STATE> void detach(m_GatePoint<STATE> &gp, m_Spline &sp);
// It just detach pointer.Don't delete anything.
//
//
template <GatePointState STATE> class m_GatePoint : Touchable {
private:
  constexpr static float RADIUS = 3.0;
  constexpr static float TEXT_OFFSET = 2.0;
  constexpr static float OUTLINE = 2.0;
  const m_Gate &_gate;
  const Chars _text = "";
  Vector2 relativePos;
  Vec<m_Spline *> splines;
  Vector2 _world_pos();
  Circle _cir();
  bool _is_disconnected();
  // bool _checkPointCollision(Vector2 pos) override;
  m_GatePoint(const m_GatePoint &gp) = delete;
  m_GatePoint(const m_GatePoint &&gp) = delete;

public:
  bool booleanState = false; // It needed to be public.As friend doesn't work on
                             // inherited class.
  m_GatePoint(const m_Gate &gate);
  m_GatePoint(const m_Gate &gate, const Chars &text);
  void _draw();
  m_Spline *get_spline()
    requires(STATE == GPs::in);
  // void addSpline(Spline *const spline);
  // void removeSpline(Spline const *spline)
  //  requires(STATE == GPs::out);

  // void removeSpline()
  //  requires(STATE == GPs::in);
  bool _checkPointCollision(Vector2 pos) override;
  void _setRelativePos(Vector2 pos) { relativePos = pos; }
  void _update();
  void _onClick();
  template <GPs S> friend void attach(m_GatePoint<S> &gp, m_Spline &sp);
  template <GPs S> friend void detach(m_GatePoint<S> &gp, m_Spline &sp);
  friend m_Spline;
  friend m_Gate;
};
using m_IGP = m_GatePoint<GPs::in>;
using IGP = Box<m_IGP>;
using IGPs = Vec<IGP>;
using m_OGP = m_GatePoint<GPs::out>;
using OGP = Box<m_OGP>;
using OGPs = Vec<OGP>;

// Spline connected tow gatePoint.
// It is like wire.
class m_Spline {
  static std::unordered_set<m_Spline *> SPLINES;
  constexpr static float SPLINE_THICKNESS = 5;
  constexpr static float BORDER = 3;
  constexpr static float BEZIER_POINT = 50;

private:
  m_IGP *_in_ptr;
  m_OGP *_out_ptr;
  m_Spline(const m_Spline &s) = delete;
  m_Spline(const m_Spline &&s) = delete;

private:
  void _signalPass();

public:
  static m_Spline *CURRENT_SPLINE;
  static bool mouse_empty();
  static void SplinesDraw();
  static void removeCurrentSpline();
  static void signalPasses();

  m_Spline(m_IGP *in_ptr, m_OGP *out_ptr);
  ~m_Spline();

  void draw();
  friend m_Gate;
  friend m_IGP;
  friend m_OGP;
  template <GPs STATE> friend void attach(m_GatePoint<STATE> &gp, m_Spline &sp);
  template <GPs STATE> friend void detach(m_GatePoint<STATE> &gp, m_Spline &sp);
};

// Gate is movable.
// It has gatepoint.That can be connected to spline.
// It is the main class
// GatePoint is a helper class.
class m_Gate : public Draggable {
  constexpr static float MIN_POINT_DISTANCE = 10;

public:
  virtual void draw();
  ~m_Gate();
  void update();

protected:
  virtual void _circuitUpdate() = 0;
  virtual void _eventUpdate() {};

  m_Gate(TouchableCollection *tc, Vector2 pos, const Chars &text,
         usize inPointnrMin, usize outPointnrMin, bool dynamicInput)
      : Draggable(tc, pos), _inPointnrMin(inPointnrMin),
        _outPointnr(outPointnrMin), _dynamicInput(dynamicInput), _text(text) {
    _init();
  }
  m_Gate(TouchableCollection *tc, Vector2 pos, float width, float minHeight,
         const Chars &text, usize inPointnrMin, usize outPointnrMin,
         bool dynamicInput)
      : Draggable(tc, pos), _inPointnrMin(inPointnrMin),
        _outPointnr(outPointnrMin), _width(width), _minHeight(minHeight),
        _dynamicInput(dynamicInput), _text(text) {
    _init();
  }
  m_Gate(TouchableCollection *tc, Vector2 pos, float width, float minHeight,
         const Chars &text, bool dynamicInput,
         std::initializer_list<const Chars> inputText,
         std::initializer_list<const Chars> outputText)
      : Draggable(tc, pos), _inPointnrMin(inputText.size()),
        _outPointnr(outputText.size()), _width(width), _minHeight(minHeight),
        _dynamicInput(dynamicInput), _text(text) {
    _init(inputText, outputText);
  }

  // m_Gate(const m_Gate &mg) = delete;
  // m_Gate(const m_Gate &&mg) = delete;

protected:
  IGPs _inPoints;
  OGPs _outPoints;
  void _pointDraw();
  void _boxDraw(Color color = RED);

private:
  const usize _inPointnrMin = 2;
  const usize _outPointnr = 1;
  const float _width = 30;
  const float _minHeight = 40;
  const bool _dynamicInput = false;
  Chars _text = "";
  void _init();
  void _init(std::span<const Chars> inputText,
             std::span<const Chars> outputText);
  void _refresh();
  void _resizePoint();
  void _clearPoint();
  float _inPointDistance();
  float _outPointDistance();
  usize _inPointnr();
  float _rectHeight();
  RectSize _rectsize();
  void _onClick();
  IGP &_addGatePoint();
  bool _checkPointCollision(Vector2 pos) override;

  friend IGP;
  friend OGP;
  template <GPs S> friend void attach(m_GatePoint<S> &gp, m_Spline &sp);
  template <GPs S> friend void detach(m_GatePoint<S> &gp, m_Spline &sp);

protected:
  Rect _rect();
};
using namespace GameManager;
class AndGate : public m_Gate {
public:
  AndGate(TouchableCollection *tc, Vector2 pos,
          const Chars &text = GateName::AND)
      : m_Gate(tc, pos, text, 2, 1, true) {}
  void _circuitUpdate() override final;
};
class OrGate : public m_Gate {
public: // Constructor
  OrGate(TouchableCollection *tc, Vector2 pos, const Chars &text = GateName::OR)
      : m_Gate(tc, pos, text, 2, 1, true) {}

public: // Functions
  void _circuitUpdate() override final;
};

class NotGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  NotGate(TouchableCollection *tc, Vector2 pos,
          const Chars &text = GateName::NOT)
      : m_Gate(tc, pos, text, 1, 1, false) {}
};

class NorGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  NorGate(TouchableCollection *tc, Vector2 pos,
          const Chars &text = GateName::NOR)
      : m_Gate(tc, pos, text, 2, 1, true) {}
};
class NAndGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  NAndGate(TouchableCollection *tc, Vector2 pos,
           const Chars &text = GateName::NAND)
      : m_Gate(tc, pos, text, 2, 1, true) {}
};
class XorGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  XorGate(TouchableCollection *tc, Vector2 pos,
          const Chars &text = GateName::NAND)
      : m_Gate(tc, pos, text, 2, 1, false) {}
};

// Here are the functions
class Light : public m_Gate {
public: // Functions
  void draw() override final;
  void _circuitUpdate() override final;

public: // Constructors
  Light(TouchableCollection *tc, Vector2 pos,
        const Chars &text = GateName::LIGHT)
      : m_Gate(tc, pos, text, 1, 0, false) {}

private:
  bool _isOn(); // Check if the the light is turns on or not.
};

class Switch : public m_Gate {
private:
  bool _isOn;
  void _eventUpdate() override final;

public:
  void draw() override final;
  void _circuitUpdate() override final;

public: // Constructor
  Switch(TouchableCollection *tc, Vector2 pos,
         const Chars &text = GateName::SWITCH)
      : m_Gate(tc, pos, text, 0, 1, false) {}
};
enum class ClkTriggerS {
  up,
  down,
};
struct ClkTrigger {
  ClkTriggerS clkTS = ClkTriggerS::down;
  bool lastClkUp = false;
  bool isTriggered(bool isClkUP);
};
enum class LatchS {
  q,
  nQ,
  invalid,
};
class RSff : public m_Gate {
private:
  ClkTrigger clkTrigger;
  LatchS savedState = LatchS::nQ;
  //  void _eventUpdate() override final;
  void _updateOutput();

public:
  ClkTriggerS clkTriggerState;
//  void draw() override final;
  void _circuitUpdate() override final;
  void setClkTriggerState(ClkTriggerS state);

public: // Constructor
  RSff(TouchableCollection *tc, Vector2 pos);
};

class JKff : public m_Gate {
private:
  ClkTrigger clkTrigger;
  //  void _eventUpdate() override final;
  bool savedState = false; // As it doesn't have invalid state.
  void _updateOutput();

public:
//  void draw() override final;
  void _circuitUpdate() override final;
  void setClkTriggerState(ClkTriggerS state);

public: // Constructor
  JKff(TouchableCollection *tc, Vector2 pos);
};
