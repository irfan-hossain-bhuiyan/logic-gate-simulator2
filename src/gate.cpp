#include "gate.h"
#include "basic_template.h"
#include "globals.h"
#include "ui.h"
#include <algorithm>
#include <memory>
#include <raylib.h>
#include <unordered_set>

m_Spline *m_Spline::CURRENT_SPLINE = nullptr;
std::unordered_set<m_Spline *> m_Spline::SPLINES{};
usize m_Gate::_inPointnr() { return _inPoints.size(); }
float m_Gate::_rectHeight() {
  auto temp = std::max(_inPointnr(), _outPointnr);
  auto pointHeight = MIN_POINT_DISTANCE * float(temp + 1);
  return std::max(pointHeight, MIN_HEIGHT);
}
float m_Gate::_inPointDistance() {
  return std::max(MIN_HEIGHT / float(_inPointnr() + 1), MIN_POINT_DISTANCE);
}
RectSize m_Gate::_rectsize() { return {WIDTH, _rectHeight()}; }
void m_Gate::_pointDraw() {
  for (auto &x : this->_inPoints) {
    x->_draw();
  }
  for (auto &x : this->_outPoints) {
    x->_draw();
  }
}
void m_Gate::draw() {
  // Drawing in point
  _boxDraw();
  _pointDraw();
}
void m_Gate::_boxDraw(Color color) {

  float lineWidth = is_clicking() ? 3.0 : 1.0;
  Label(_rect(), _text, color).draw(lineWidth);
}
template <GPs T> Vector2 GatePoint<T>::_world_pos() {
  return relativePos + _gate.NodePos;
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
m_GatePoint<T>::m_GatePoint(const m_Gate &gate)
    : Touchable(gate.get_tc()), _gate(gate) {}

template <GPs T> void GatePoint<T>::_draw() {
  DrawCircleCir(_cir(), WHITE);
  DrawCircleLinesCir(_cir(), BLACK, is_touching() ? OUTLINE : 1);
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

void m_Gate::_onClick() {
  if (!_dynamicInput) {
    return;
  }
  const auto cs = m_Spline::CURRENT_SPLINE;
  if (cs == nullptr) {
    return;
  }
  if (cs->_in_ptr == nullptr) {
    IGP &gp = _addGatePoint();
    gp->_onClick();
  }
}
IGP &m_Gate::_addGatePoint() {
  auto newGatePoint = std::make_unique<m_IGP>(*this);
  _inPoints.push_back(std::move(newGatePoint));
  this->_resizePoint();
  return _inPoints.back();
}

template <GPs STATE> void GatePoint<STATE>::_update() {
  if (is_clicked()) {
    this->_onClick();
  }
}
template <GPs STATE> void GatePoint<STATE>::_onClick() {

  if constexpr (STATE == GPs::out) {
    if (m_Spline::CURRENT_SPLINE == nullptr) {
      m_Spline::CURRENT_SPLINE =
          new m_Spline(nullptr, nullptr); // Spline has new in here,because it
                                          // // doesn't reference by anyone.
      attach(*this, *m_Spline::CURRENT_SPLINE);

    } else if (m_Spline::CURRENT_SPLINE->_out_ptr == nullptr) {
      attach(*this, *m_Spline::CURRENT_SPLINE);
      m_Spline::CURRENT_SPLINE = nullptr;
    }
  }
  if constexpr (STATE == GPs::in) {
    if (m_Spline::CURRENT_SPLINE == nullptr) {
      if (this->splines.empty()) {
        m_Spline::CURRENT_SPLINE = new m_Spline(nullptr, nullptr);
        attach(*this, *m_Spline::CURRENT_SPLINE);
      } else {
        m_Spline::CURRENT_SPLINE = splines.front();
        detach(*this, *splines.front());
      }
    } else if (m_Spline::CURRENT_SPLINE->_in_ptr == nullptr) {

      attach(*this, *m_Spline::CURRENT_SPLINE);
      m_Spline::CURRENT_SPLINE = nullptr;
    }
  }
}
m_Spline::m_Spline(m_IGP *in_ptr, m_OGP *out_ptr)
    : _in_ptr(in_ptr), _out_ptr(out_ptr) {
  SPLINES.insert(this);
}
m_Spline::~m_Spline() {
  auto it = SPLINES.find(this);
  if (it == SPLINES.end()) {
    GameManager::Debugger::push_message("SPLINES isn't in spline");
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

Rect m_Gate::_rect() { return rectFromPos(NodePos, _rectsize()); }

template <GPs STATE> void attach(m_GatePoint<STATE> &gp, m_Spline &spline) {
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

template void attach<GPs::in>(m_IGP &gp, m_Spline &spline);
template void attach<GPs::out>(m_OGP &gp, m_Spline &spline);
template <GPs STATE> void detach(m_GatePoint<STATE> &gp, m_Spline &spline) {
  using namespace GameManager;
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
    spline._out_ptr = nullptr;
  }
}

template void detach<GPs::in>(m_IGP &gp, m_Spline &spline);
template void detach<GPs::out>(m_OGP &gp, m_Spline &spline);

template class m_GatePoint<GPs::in>;
template class m_GatePoint<GPs::out>;

void m_Gate::_refresh() {
  this->_clearPoint();
  this->_resizePoint();
}

void m_Gate::_resizePoint() {
  for (usize i = 0; i < this->_inPointnr(); i++) {
    _inPoints[i]->_setRelativePos(
        {-2, float(i + 1) * this->_inPointDistance()});
  }
  for (usize i = 0; i < this->_outPointnr; i++) {
    _outPoints[i]->_setRelativePos(
        {WIDTH + 2, float(i + 1) * this->_outPointDistance()});
  }
}
void m_Gate::_clearPoint() {
  for (auto it = _inPoints.begin(); it != _inPoints.end();) {
    if (_inPoints.size() == _inPointnrMin) {
      break;
    }
    if ((*it)->_is_disconnected()) {
      it = _inPoints.erase(it);
    } else {
      it++;
    }
  }
}
float m_Gate::_outPointDistance() {
  return _rectHeight() / float(_outPointnr + 1);
}
void m_Gate::_init() {
  _inPoints.reserve(_inPointnrMin);
  for (usize i = 0; i < _inPointnrMin; i++) {
    auto point = std::make_unique<m_IGP>(*this);
    _inPoints.push_back(std::move(point));
  }
  for (usize i = 0; i < _outPointnr; i++) {
    auto point = std::make_unique<m_OGP>(*this);
    _outPoints.push_back(std::move(point));
  }
  this->_resizePoint();
}

void m_Gate::update() {
  using namespace GateWindow;
  if (this->is_clicked() && isMouseState(MouseState::editing)) {
    _onClick();
  }
  this->_eventUpdate();
  this->_circuitUpdate();
  this->mouseMoveUpdate();
  for (auto &x : _inPoints) {
    x->_update();
  }
  for (auto &x : _outPoints) {
    x->_update();
  }
}
bool m_Spline::mouse_empty() { return CURRENT_SPLINE == nullptr; }
void m_Spline::SplinesDraw() {
  for (auto x : SPLINES) {
    x->draw();
  }
}
void m_Spline::draw() {
  using namespace GameManager;
  auto in_pos = _in_ptr != nullptr
                    ? _in_ptr->_world_pos()
                    : getGlobalMousePosition(UsedCamera::gateCamera);
  auto out_pos = _out_ptr != nullptr
                     ? _out_ptr->_world_pos()
                     : getGlobalMousePosition(UsedCamera::gateCamera);
  auto color =
      _out_ptr != nullptr ? (_out_ptr->booleanState ? RED : WHITE) : WHITE;
  DrawLineBezierCubic(in_pos, out_pos, in_pos + Vector2{-BEZIER_POINT, 0},
                      out_pos + Vector2{BEZIER_POINT, 0}, SPLINE_THICKNESS,
                      BLACK);
  DrawLineBezierCubic(in_pos, out_pos, in_pos + Vector2{-BEZIER_POINT, 0},
                      out_pos + Vector2{BEZIER_POINT, 0},
                      SPLINE_THICKNESS - BORDER, color);
}
template <GPs STATE>
bool m_GatePoint<STATE>::_checkPointCollision(Vector2 pos) {
  return CheckCollisionPointCircle(pos, _cir());
}
template <GPs STATE> bool m_GatePoint<STATE>::_is_disconnected() {
  return splines.empty();
}
bool m_Gate::_checkPointCollision(Vector2 pos) {
  return CheckCollisionPointRec(pos, _rect());
}
template <GPs STATE>
m_Spline *m_GatePoint<STATE>::get_spline()
  requires(STATE == GPs::in)
{
  return this->splines.front();
}
void m_Spline::removeCurrentSpline() {
  if (m_Spline::CURRENT_SPLINE == nullptr)
    return;
  delete m_Spline::CURRENT_SPLINE;
  m_Spline::CURRENT_SPLINE = nullptr;
}

void AndGate::_circuitUpdate() {
  bool output = true;
  for (auto &x : _inPoints) {
    output &= x->booleanState;
  }
  _outPoints.front()->booleanState = output;
}
void OrGate::_circuitUpdate() {
  bool output = false;
  for (auto &x : _inPoints) {
    output |= x->booleanState;
  }
  _outPoints.front()->booleanState = output;
}
void NotGate::_circuitUpdate() {
  bool output = true;
  for (auto &x : _inPoints) {
    output ^= x->booleanState;
  }
  _outPoints.front()->booleanState = output;
}
bool Light::_isOn() { return _inPoints.front()->booleanState; }
void Light::draw() {
  _boxDraw(_isOn() ? GREEN : RED);
  _pointDraw();
}
void Switch::draw() {
  _boxDraw(_isOn ? GREEN : RED);
  _pointDraw();
}
void Switch::_circuitUpdate() { _outPoints.front()->booleanState = _isOn; }
void Switch::_eventUpdate() {
  using namespace GateWindow;
  if (is_clicked() && isMouseState(MouseState::running)) {
    _isOn = !_isOn;
  }
}
void Light::_circuitUpdate() {}
void m_Spline::_signalPass() {
  if (_in_ptr == nullptr || _out_ptr == nullptr) {
    return;
  }
  _in_ptr->booleanState = _out_ptr->booleanState;
}
void m_Spline::signalPasses() {
  for (auto &x : SPLINES)
    x->_signalPass();
}
