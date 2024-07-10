#pragma once
#include "basic_template.h"
#include "object.h"
#include "ui.h"
#include <raylib.h>
#include <unordered_set>
enum class GatePointState { in, out };
class Spline;
class Gate;
using GPs = GatePointState;

// GatePoint is a helper class.
// It is clickable.
// It follows Gate.So gatepoint is a child.
// It takes a spline.
template <GPs STATE> class GatePoint;

// Because both of them are mutable reference of the function
template <GPs STATE> void attach(GatePoint<STATE> &gp, Spline &sp);
template <GPs STATE> void detach(GatePoint<STATE> &gp, Spline &sp);
// It just detach pointer.Don't delete anything.
//
//
template <GatePointState STATE> class GatePoint : Touchable {
  constexpr static float RADIUS = 3;
  const Gate &gate;
  Vector2 relativePos;
  Vec<Spline *> splines;

  Vector2 _world_pos();
  Circle _cir();
  bool checkPointCollision(Vector2 pos) override {
    return CheckCollisionPointCircle(pos, _cir());
  }

public:
  GatePoint(Gate &gate);
  void draw();
  // void addSpline(Spline *const spline);
  // void removeSpline(Spline const *spline)
  //  requires(STATE == GPs::out);

  // void removeSpline()
  //  requires(STATE == GPs::in);

  void setRelativePos(Vector2 pos) { relativePos = pos; }

  void onClick();
  template <GPs S> friend void attach(GatePoint<S> &gp, Spline &sp);
  template <GPs S> friend void detach(GatePoint<S> &gp, Spline &sp);
  friend Spline;
};
using IGP = GatePoint<GPs::in>;
using IGPs = Vec<IGP>;
using OGP = GatePoint<GPs::out>;
using OGPs = Vec<OGP>;

// Spline connected tow gatePoint.
// It is like wire.
class Spline {
  static std::unordered_set<Spline *> SPLINES;
  constexpr static float SPLINE_THICKNESS = 5;
  constexpr static float BORDER = 1;

private:
  IGP *_in_ptr;
  OGP *_out_ptr;

public:
  static Spline *CURRENT_SPLINE;
  static bool mouse_empty() { return CURRENT_SPLINE == nullptr; }
  static void SplinesDraw() {
    for (auto x : SPLINES) {
      x->draw();
    }
  }
  Spline(IGP *in_ptr, OGP *out_ptr);
  ~Spline();

  void draw() {
    DrawLineBezier(_in_ptr->_world_pos(), _out_ptr->_world_pos(),
                   SPLINE_THICKNESS - BORDER, WHITE);
    DrawLineBezier(_in_ptr->_world_pos(), _out_ptr->_world_pos(),
                   SPLINE_THICKNESS, BLACK);
  }
  friend Gate;
  friend IGP;
  friend OGP;
  template <GPs STATE> friend void attach(GatePoint<STATE> &gp, Spline &sp);
  template <GPs STATE> friend void detach(GatePoint<STATE> &gp, Spline &sp);
};

// Gate is movable.
// It has gatepoint.That can be connected to spline.
// It is the main class
// GatePoint is a helper class.
class Gate : public Draggable {
  constexpr static float MIN_POINT_DISTANCE = 10;
  constexpr static float WIDTH = 20;
  constexpr static float MIN_HEIGHT = 40;
  constexpr static float CIRCLE_RADIUS = 3;
  void onClick();
  IGP &_addGatePoint();
  bool checkPointCollision(Vector2 pos) override {
    return CheckCollisionPointRec(pos, _rect());
  }

public:
  void draw();
  Gate(TouchableCollection *tc, Vector2 pos) : Draggable(tc, pos) {}
  Gate(TouchableCollection *tc, Vector2 pos, int inPointnrMin)
      : Draggable(tc, pos), _inPointnrMin(inPointnrMin) {}

  Gate(TouchableCollection *tc, Vector2 pos, const Chars &&text,
       int inPointnrMin, int outPointnrMin)
      : Draggable(tc, pos), _inPointnrMin(inPointnrMin),
        _outPointnrMin(outPointnrMin), _text(text) {}
  Gate(TouchableCollection *tc, Vector2 pos, const Chars &&text)
      : Draggable(tc, pos), _text(text) {}

private:
  const int _inPointnrMin = 2;
  const int _outPointnrMin = 1;
  Chars _text;
  IGPs _inPoints;
  OGPs _outPoints;

  usize _inPointnr();
  usize _outPointnr();
  float _rectHeight();
  RectSize _rectsize();
  Rect _rect();
  void _drawPoint(Vector2 point);
  friend IGP;
  friend OGP;
  template <GPs S> friend void attach(GatePoint<S> &gp, Spline &sp);
  template <GPs S> friend void detach(GatePoint<S> &gp, Spline &sp);
};

// Here are the functions


