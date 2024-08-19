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
  Vector2 _world_pos() const;
  Circle _cir() const;
  bool _is_disconnected() const;
  const Touchable *checkPointCollision(Vector2 pos) const override;
  void _setRelativePos(Vector2 pos) { relativePos = pos; }
  void _update(const GGS &tc);
  void _onClick();

  // bool _checkPointCollision(Vector2 pos) override;
public:
  bool booleanState = false; // It needed to be public.As friend doesn't work on
                             // inherited class.
  m_GatePoint(const m_Gate &gate);
  m_GatePoint(const m_Gate &gate, const Chars &text);
  void _draw(const GGS &tc) const;
  m_Spline *get_spline() const
    requires(STATE == GPs::in);
  // void addSpline(Spline *const spline);
  // void removeSpline(Spline const *spline)
  //  requires(STATE == GPs::out);

  // void removeSpline()
  //  requires(STATE == GPs::in);
  void toggleState();
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

  void draw() const;
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
  const Chars gateName;
  virtual void draw(const GGS &tc) const;
  ~m_Gate();
  void update(const GGS &tc);
  const Touchable *checkPointCollision(Vector2 pos) const override;

protected:
  virtual void _circuitUpdate() = 0;
  virtual void _eventUpdate(const GGS &tc) {};

  m_Gate(Vector2 pos, const Chars &text, usize inPointnrMin,
         usize outPointnrMin, bool dynamicInput);
  m_Gate(Vector2 pos, float width, float minHeight, const Chars &text,
         usize inPointnrMin, usize outPointnrMin, bool dynamicInput);
  m_Gate(Vector2 pos, float width, const Chars &text, usize inPointnrMin,
         usize outPointnrMin, bool dynamicInput);
  m_Gate(Vector2 pos, float width, float minHeight, const Chars &text,
         bool dynamicInput, std::initializer_list<const Chars> inputText,
         std::initializer_list<const Chars> outputText);

  // m_Gate(const m_Gate &mg) = delete;
  // m_Gate(const m_Gate &&mg) = delete;

protected:
  IGPs _inPoints;
  OGPs _outPoints;
  void _pointDraw(const GGS &tc) const;
  void _boxDraw(const GGS &tc, Color color = RED) const;

private:
  const usize _inPointnrMin = 2;
  const usize _outPointnr = 1;
  const float _width = 30;
  const float _minHeight = 40;
  const bool _dynamicInput = false;
  void _init();
  void _init(std::span<const Chars> inputText,
             std::span<const Chars> outputText);
  void _refresh();
  void _resizePoint();
  void _clearPoint();
  float _inPointDistance() const;
  float _outPointDistance() const;
  usize _inPointnr() const;
  float _rectHeight() const;
  RectSize _rectsize() const;
  void _onClick();
  IGP &_addGatePoint();

  friend IGP;
  friend OGP;
  template <GPs S> friend void attach(m_GatePoint<S> &gp, m_Spline &sp);
  template <GPs S> friend void detach(m_GatePoint<S> &gp, m_Spline &sp);

protected:
  Rect _rect() const;
};
using namespace GameManager;
class AndGate : public m_Gate {
public:
  AndGate(Vector2 pos);
  void _circuitUpdate() override final;
};
class OrGate : public m_Gate {
public: // Constructor
  OrGate(Vector2 pos);

public: // Functions
  void _circuitUpdate() override final;
};

class NotGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  NotGate(Vector2 pos);
};

class NorGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  NorGate(Vector2 pos);
};
class NAndGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  NAndGate(Vector2 pos);
};
class XorGate : public m_Gate {
public:
  void _circuitUpdate() override final;

public:
  XorGate(Vector2 pos);
};

// Here are the functions
class Light : public m_Gate {
public: // Functions
  void draw(const GGS &tc) const override final;
  void _circuitUpdate() override final;

public: // Constructors
  Light(Vector2 pos);

private:
  bool _isOn() const; // Check if the the light is turns on or not.
};

class Switch : public m_Gate {
private:
  bool _isOn;
  void _eventUpdate(const GGS &tc) override final;

public:
  void draw(const GGS &tc) const override final;
  void _circuitUpdate() override final;

public: // Constructor
  Switch(Vector2 pos);
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
  RSff(Vector2 pos);
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
  JKff(Vector2 pos);
};
class ClkPulse : public m_Gate {
private:
  //  void _eventUpdate() override final;
  void draw(const GGS &tc) const override final;
  void _updateOutput();
  double _lastTime;
  bool _isOn;

public:
  //  void draw() override final;
  float halfPulseTime = 0.5;
  void _circuitUpdate() override final;

public: // Constructor
  ClkPulse(Vector2 pos);
};
