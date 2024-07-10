#include "gate.h"
#include "basic_template.h"
#include "ui.h"
#include <algorithm>
#include <raylib.h>
#include <unordered_set>

Spline *Spline::CURRENT_SPLINE = nullptr;
std::unordered_set<Spline *> Spline::SPLINES{};
usize Gate::_inPointnr() { return _inPoints.size(); }
usize Gate::_outPointnr() { return _outPoints.size(); }
float Gate::_rectHeight() {
  auto temp = std::max(_inPointnr(), _outPointnr());
  auto pointHeight = MIN_POINT_DISTANCE * float(temp + 1);
  return std::max(pointHeight, MIN_HEIGHT);
}
RectSize Gate::_rectsize() { return {WIDTH, _rectHeight()}; }
void Gate::draw() {
  auto lineWidth = is_clicked() ? 3 : 1;
  Label(rectFromPos(NodePos, _rectsize()), _text, RED).draw(lineWidth);
  // Drawing in point
}
template <GPs T> Vector2 GatePoint<T>::_world_pos() {
  return relativePos + gate.NodePos;
}
template <GPs T> Circle GatePoint<T>::_cir() {
  auto orginalPos = _world_pos();
  return Circle{orginalPos, RADIUS};
}
// template <GPs STATE>
// void GatePoint<STATE>::removeSpline()
//   requires(STATE == GPs::in)
//{
//   splines.clear();
// }
// template <GPs STATE>
// void GatePoint<STATE>::removeSpline(const Spline* spline)
//   requires(STATE == GPs::out)
//{
//	Error err=eraseItem<Spline*>(splines,spline);
//	if(err!=Error::OK){Debugger::push_message("Spline isn't avaliable in the
// out pointer.");}
// }
template <GPs T>
GatePoint<T>::GatePoint(Gate &gate) : Touchable(gate.get_tc()), gate(gate) {}

template <GPs T> void GatePoint<T>::draw() {
  DrawCircleCir(_cir(), WHITE);
  DrawCircleLinesCir(_cir(), BLACK, is_touching() ? 2 : 1);
}
// template <GPs STATE> void GatePoint<STATE>::addSpline(Spline *const spline) {
//   if constexpr (STATE == GPs::in) {
//     splines.clear();
//     Spline::CURRENT_SPLINE->_in_ptr = this;
//   }
//   if constexpr (STATE == GPs::out) {
//     Spline::CURRENT_SPLINE->_out_ptr = this;
//   }
//   splines.push_back(spline);
// }

void Gate::onClick() {
  const auto cs = Spline::CURRENT_SPLINE;
  if (cs == nullptr) {
    return;
  }
  if (cs->_in_ptr == nullptr) {
    IGP &gp = _addGatePoint();
    gp.onClick();
  }
}
IGP &Gate::_addGatePoint() {
  IGP n = IGP(*this);
  _inPoints.push_back(IGP(*this));
  return _inPoints.back();
}

template <GPs STATE> void GatePoint<STATE>::onClick() {

  if constexpr (STATE == GPs::out) {
    if (Spline::CURRENT_SPLINE == nullptr) {
      Spline::CURRENT_SPLINE =
          new Spline(nullptr, nullptr); // Spline has new in here,because it
                                        // doesn't reference by anyone.
    }
    if (Spline::CURRENT_SPLINE->_out_ptr == nullptr) {
      attach(*this, *Spline::CURRENT_SPLINE);
    }
  } 
  if constexpr (STATE == GPs::in) {
    if (Spline::CURRENT_SPLINE == nullptr) {
      if (this->splines.empty()) {
        Spline::CURRENT_SPLINE = new Spline(nullptr, nullptr);
        attach(*this, *Spline::CURRENT_SPLINE);
      } else {
        Spline::CURRENT_SPLINE = splines.front();
        detach(*this, *splines.front());
      }
    }
    if (Spline::CURRENT_SPLINE->_in_ptr == nullptr) {

      attach(*this, *Spline::CURRENT_SPLINE);
    }
  }
}
Spline::Spline(IGP *in_ptr, OGP *out_ptr) : _in_ptr(in_ptr), _out_ptr(out_ptr) {
  SPLINES.insert(this);
}
Spline::~Spline() {
  auto it = SPLINES.find(this);
  if (it == SPLINES.end()) {
    Debugger::push_message("SPLINES isn't in spline");
    return;
  }
  SPLINES.erase(it);
  if (_in_ptr != nullptr) {
    detach(*_in_ptr, *this);
  }
  if (_out_ptr != nullptr) {
    detach(*_out_ptr, *this);
  }
}

Rect Gate::_rect(){
	return rectFromPos(NodePos,_rectsize());
}

template <GPs STATE> void attach(GatePoint<STATE> &gp, Spline &spline) {
  if constexpr (STATE == GPs::in) {
    if (!gp.splines.empty()) {
      delete gp.splines.front();
    }
    gp.splines.push_back(&spline);
    spline._in_ptr = &gp;
  } else if constexpr (STATE == GPs::out) {
    gp.splines.push_back(&spline);
    spline._out_ptr = &gp;
  }
}

template void attach<GPs::in>(IGP &gp, Spline &spline);
template void attach<GPs::out>(OGP &gp, Spline &spline);
template <GPs STATE> void detach(GatePoint<STATE> &gp, Spline &spline) {
  if constexpr (STATE == GPs::in) {
    if (gp.splines.empty()) {
      Debugger::push_message("The inpoint vector is empty.");
      return;
    }
    if (gp.splines.front() != &spline) {
      Debugger::push_message("In point is not contain the spline.");
      return;
    }
    if (spline._in_ptr != &gp) {
      Debugger::push_message("Spline not contain the gp");
      return;
    }
    gp.splines.clear();
    spline._in_ptr = nullptr;
  } else if constexpr (STATE == GPs::out) {
    Error err = eraseItem(gp.splines, &spline);
    if (err != Error::OK) {
      Debugger::push_message("The Gatepoint don't contain spline");
    }
    if (spline._out_ptr != &gp) {
      Debugger::push_message("The Spline don't contain Gatepoint");
    }
    spline._out_ptr = &gp;
  }
}

template void detach<GPs::in>(IGP &gp, Spline &spline); 
template void detach<GPs::out>(OGP &gp, Spline &spline); 

template class GatePoint<GPs::in>;
template class GatePoint<GPs::out>;
