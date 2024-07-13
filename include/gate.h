#pragma once
#include "basic_template.h"
#include "object.h"
#include "ui.h"
#include <raylib.h>
#include <unordered_set>
enum class GatePointState { in, out };
class m_Spline;
class m_Gate;
using Gate = Box<m_Gate>;
using Gates=Vec<Gate>;
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
  constexpr static float RADIUS = 3.0;
  constexpr static float OUTLINE = 2.0;
  const m_Gate &_gate;
  Vector2 relativePos;
  Vec<m_Spline *> splines;
  Vector2 _world_pos();
  Circle _cir();
  bool _is_disconnected();
  bool checkPointCollision(Vector2 pos) override;
  m_GatePoint(const m_GatePoint &gp) = delete;
  m_GatePoint(const m_GatePoint &&gp) = delete;

public:
  bool booleanState = false;
  m_GatePoint(const m_Gate &gate);
  void _draw();
  m_Spline *get_spline()
    requires(STATE == GPs::in);
  // void addSpline(Spline *const spline);
  // void removeSpline(Spline const *spline)
  //  requires(STATE == GPs::out);

  // void removeSpline()
  //  requires(STATE == GPs::in);

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
  constexpr static float BEZIER_POINT=50;
private:
  m_IGP *_in_ptr;
  m_OGP *_out_ptr;
  m_Spline(const m_Spline &s) = delete;
  m_Spline(const m_Spline &&s) = delete;

public:
  static m_Spline *CURRENT_SPLINE;
  static bool mouse_empty();
  static void SplinesDraw();
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
  void draw();
  virtual void circuitUpdate() = 0;
  void update();

  m_Gate(TouchableCollection *tc, Vector2 pos, const Chars &text,
         usize inPointnrMin, usize outPointnrMin, bool dynamicInput)
      : Draggable(tc, pos), _inPointnrMin(inPointnrMin),
        _outPointnr(outPointnrMin), _dynamicInput(dynamicInput), _text(text) {
    _init();
  }
  m_Gate(const m_Gate &mg) = delete;
  m_Gate(const m_Gate &&mg) = delete;

protected:
  IGPs _inPoints;
  OGPs _outPoints;

private:
  const usize _inPointnrMin = 2;
  const usize _outPointnr = 1;
  const float WIDTH = 30;
  const float MIN_HEIGHT = 40;
  const bool _dynamicInput = true;
  Chars _text = "";
  void _init();
  void _refresh();
  void _resizePoint();
  void _clearPoint();
  float _inPointDistance();
  float _outPointDistance();
  usize _inPointnr();
  float _rectHeight();
  RectSize _rectsize();
  Rect _rect();
  void _onClick();
  IGP &_addGatePoint();
  bool checkPointCollision(Vector2 pos) override;

  friend IGP;
  friend OGP;
  template <GPs S> friend void attach(m_GatePoint<S> &gp, m_Spline &sp);
  template <GPs S> friend void detach(m_GatePoint<S> &gp, m_Spline &sp);
};
class AndGate : public m_Gate {
public:
  AndGate(TouchableCollection *tc, Vector2 pos, Chars &&text = "")
      : m_Gate(tc, pos, text, 2, 1, true) {}
  void circuitUpdate() override final {
    bool output = true;
    for (auto &x : _inPoints) {
      output ^= x->booleanState;
    }
    _outPoints.front()->booleanState = output;
  }
};
class OrGate : public m_Gate {
public:
  OrGate(TouchableCollection *tc, Vector2 pos, Chars &&text = "")
      : m_Gate(tc, pos, text, 2, 1, true) {}
  void circuitUpdate() override final {
    bool output = false;
    for (auto &x : _inPoints) {
      output |= x->booleanState;
    }
    _outPoints.front()->booleanState = output;
  }
};

class NotGate : public m_Gate {
public:
  void circuitUpdate() override final {
    bool output = true;
    for (auto &x : _inPoints) {
      output ^= x->booleanState;
    }
    _outPoints.front()->booleanState = output;
  }

public:
  NotGate(TouchableCollection *tc, Vector2 pos, const Chars &text = "")
      : m_Gate(tc, pos, text, 1, 1, false) {}
};
// Here are the functions
