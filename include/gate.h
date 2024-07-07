#include "basic_template.h"
#include "object.h"
#include "ui.h"
#include <algorithm>
#include <raylib.h>
#include <unordered_set>
enum class GatePointState { in, out };
using GPs = GatePointState;
template <GPs STATE> class GatePoint;
using IGP = GatePoint<GPs::in>;
using IGPs = Vec<IGP>;
using OGP = GatePoint<GPs::out>;
using OGPs = Vec<OGP>;
class Spline {
  static std::unordered_set<Spline *> SPLINES;

private:
  GatePoint<GPs::in> const *_in_ptr;
  GatePoint<GPs::out> const *_out_ptr;

public:
  static Spline *CURRENT_SPLINE;
  Spline(IGP *in_ptr, OGP *out_ptr) : _in_ptr(in_ptr), _out_ptr(out_ptr) {
    SPLINES.insert(this);
  }
  ~Spline() {
    auto it = SPLINES.find(this);
    if (it == SPLINES.end()) {
      Debugger::push_message("SPLINES isn't in spline");
      return;
    }
    SPLINES.erase(it);
    if(_in_ptr!=nullptr){
	_in_ptr->clear();
    }
  }

  void draw() {}
  friend IGP;
  friend OGP;
};
class Gate : public Draggable {
  constexpr static float MIN_POINT_DISTANCE = 10;
  constexpr static float WIDTH = 20;
  constexpr static float MIN_HEIGHT = 40;
  constexpr static float CIRCLE_RADIUS = 3;
  void addSpline(Spline *spline);
  void clearInPoints();

public:
  void draw();

private:
  const int _inPointnrMin = 2;
  const int _outPointnrMin = 1;
  Chars text;
  IGPs inPoints;
  OGPs outPoints;

  int _inPointnr();
  int _outPointnr();
  float _rectHeight();
  RectSize _rectsize();
  void _drawPoint(Vector2 point);

  friend IGP;
  friend OGP;
};
template <GatePointState STATE> class GatePoint : Touchable {
  constexpr static float RADIUS = 3;
  const Gate &gate;
  Vector2 relativePos;
  Vec<Spline *> splines;

  Vector2 _world_pos() { return relativePos + gate.NodePos; }
  Circle _cir() {
    auto orginalPos = _world_pos();
    return Circle{orginalPos, RADIUS};
  }

public:
  GatePoint(Gate &gate) : gate(gate), Touchable(gate.get_tc()) {}
  void draw() {
    DrawCircleCir(_cir(), WHITE);
    DrawCircleLinesCir(_cir(), BLACK, is_touching() ? 2 : 1);
  }

  void addSpline(Spline *const spline) {
    if constexpr (STATE == GPs::in) {
      splines.clear();
      Spline::CURRENT_SPLINE->_in_ptr = this;
    }
    if constexpr (STATE == GPs::out) {
      Spline::CURRENT_SPLINE->_out_ptr = this;
    }
    splines.push_back(spline);
  }
  void removeSpline(Spline *const spline)requires(STATE==GPs::out){
	
  }

  Vector2 setRelativePos(Vector2 pos) { relativePos = pos; }

  void onClick() {
    if constexpr (STATE == GPs::out) {
      if (Spline::CURRENT_SPLINE->_out_ptr == nullptr) {
        addSpline(Spline::CURRENT_SPLINE);
      }
    }
    if constexpr (STATE == GPs::in) {
      if (Spline::CURRENT_SPLINE->_in_ptr == nullptr) {
        addSpline(Spline::CURRENT_SPLINE);
      }
    }
  }
};
