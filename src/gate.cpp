#include "gate.hpp"
#include "basic_template.hpp"
#include "globals.hpp"
#include "ui.hpp"
#include <algorithm>
#include <memory>
#include <raylib.h>
#include <span>
#include <unordered_set>
m_Spline *m_Spline::CURRENT_SPLINE = nullptr;
std::unordered_set<m_Spline *> m_Spline::SPLINES{};
usize m_Gate::_inPointnr() const { return _inPoints.size(); }
float m_Gate::_rectHeight() const {
  auto temp = std::max(_inPointnr(), _outPointnr);
  auto pointHeight = MIN_POINT_DISTANCE * float(temp + 1);
  return std::max(pointHeight, _minHeight);
}
float m_Gate::_inPointDistance() const {
  return std::max(_minHeight / float(_inPointnr() + 1), MIN_POINT_DISTANCE);
}
m_Gate::~m_Gate() {
  for (auto &x : _inPoints)
    std::for_each(x->splines.begin(), x->splines.end(),
                  [](auto ptr) { delete ptr; });

  for (auto &x : _outPoints)
    std::for_each(x->splines.begin(), x->splines.end(),
                  [](auto ptr) { delete ptr; });
}
RectSize m_Gate::_rectsize() const { return {_width, _rectHeight()}; }
void m_Gate::_pointDraw(const GGS &tc) const {
  for (auto &x : this->_inPoints) {
    x->_draw(tc);
  }
  for (auto &x : this->_outPoints) {
    x->_draw(tc);
  }
}
void m_Gate::draw(const GGS &tc) const {
  // Drawing in point
  _boxDraw(tc);
  _pointDraw(tc);
}
void m_Gate::_boxDraw(const GGS &tc, Color color) const {

  float lineWidth = 1.0f;
  if (is_selected(tc))
    lineWidth = 3.0f;
  else if (is_touching(tc))
    lineWidth = 2.0f;
  Label(_rect(), gateName, color).draw(lineWidth);
}
template <GPs T> Vector2 GatePoint<T>::_world_pos() const {
  return relativePos + _gate.NodePos;
}
template <GPs T> Circle GatePoint<T>::_cir() const {
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
m_GatePoint<T>::m_GatePoint(const m_Gate &gate) : _gate(gate) {}

template <GPs T> void GatePoint<T>::_draw(const GGS &tc) const {
  DrawCircleCir(_cir(), WHITE);
  DrawCircleLinesCir(_cir(), BLACK, is_touching(tc) ? OUTLINE : 1);
  if (_text.empty())
    return;
  auto size = measureText(_text);
  auto pos = _world_pos();
  float textPosX;
  if constexpr (T == GPs::in) {
    textPosX = pos.x + RADIUS + TEXT_OFFSET;
  }
  if constexpr (T == GPs::out) {
    textPosX = pos.x - RADIUS - TEXT_OFFSET - size.width;
  }
  auto textPosY = pos.y - size.height / 2;
  Vector2 textPos = {textPosX, textPosY};
  drawText(_text, textPos);
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
    _refresh();
  }
}
IGP &m_Gate::_addGatePoint() {
  auto newGatePoint = std::make_unique<m_IGP>(*this);
  _inPoints.push_back(std::move(newGatePoint));
  return _inPoints.back();
}

template <GPs STATE> void GatePoint<STATE>::_update(const GGS &tc) {
  if (is_clicked(tc)) {
    this->_onClick();
  }
}

using namespace GameManager;
template <GPs STATE> void GatePoint<STATE>::_onClick() {
  if (!GateWindow::isMouseState(GateWindow::MouseState::editing))
    return;
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

   // this->_gate.refresh();
  }
}
m_Spline::m_Spline(m_IGP *in_ptr, m_OGP *out_ptr)
    : _in_ptr(in_ptr), _out_ptr(out_ptr) {
  SPLINES.insert(this);
}
m_Spline::~m_Spline() {
  auto it = SPLINES.find(this);
  if (it == SPLINES.end()) {
    GameManager::Debugger::messages << "SPLINES isn't in spline" << "\n";
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

Rect m_Gate::_rect() const { return rectFromPos(NodePos, _rectsize()); }

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
      Debugger::messages << "The inpoint vector is empty." << "\n";
      return;
    }
    if (gp.splines.front() != &spline) {
      Debugger::messages << "In point is not contain the spline." << "\n";
      return;
    }
    if (spline._in_ptr != &gp) {
      Debugger::messages << "Spline not contain the gp" << "\n";
      return;
    }
    gp.splines.clear();
    spline._in_ptr = nullptr;
  } else if constexpr (STATE == GPs::out) {
    ErrorS err = eraseItem(gp.splines, &spline);
    if (err != ErrorS::OK) {
      Debugger::messages << "The Gatepoint don't contain spline" << "\n";
    }
    if (spline._out_ptr != &gp) {
      Debugger::messages << "The Spline don't contain Gatepoint" << "\n";
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
        {_width + 2, float(i + 1) * this->_outPointDistance()});
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
float m_Gate::_outPointDistance() const {
  return _rectHeight() / float(_outPointnr + 1);
}
void m_Gate::_init(std::span<const Chars> inputText,
                   std::span<const Chars> outputText) {
  _inPoints.reserve(_inPointnrMin);
  for (usize i = 0; i < _inPointnrMin; i++) {
    auto point = std::make_unique<m_IGP>(*this, inputText[i]);
    _inPoints.push_back(std::move(point));
  }
  for (usize i = 0; i < _outPointnr; i++) {
    auto point = std::make_unique<m_OGP>(*this, outputText[i]);
    _outPoints.push_back(std::move(point));
  }
  this->_resizePoint();
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
void m_Gate::update(const GGS &tc) {
  using namespace GameManager::GateWindow;
  if (this->is_clicked(tc) && isMouseState(MouseState::editing)) {
    _onClick();
  }
  this->_eventUpdate(tc);
  this->_circuitUpdate();
  this->mouseMoveUpdate(tc);
  for (auto &x : _inPoints) {
    x->_update(tc);
  }
  for (auto &x : _outPoints) {
    x->_update(tc);
  }
  _refresh();
}
bool m_Spline::mouse_empty() { return CURRENT_SPLINE == nullptr; }
void m_Spline::SplinesDraw() {
  for (auto x : SPLINES) {
    x->draw();
  }
}
void m_Spline::draw() const {
  using namespace GameManager;
  auto in_pos = _in_ptr != nullptr
                    ? _in_ptr->_world_pos()
                    : getGlobalMousePosition(UsedCameraS::gateCamera);
  auto out_pos = _out_ptr != nullptr
                     ? _out_ptr->_world_pos()
                     : getGlobalMousePosition(UsedCameraS::gateCamera);
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
const Touchable::Id m_GatePoint<STATE>::checkPointCollision(Vector2 pos) const {
  return CheckCollisionPointCircle(pos, _cir()) ? this->id() : Id::Null;
}
template <GPs STATE> void m_GatePoint<STATE>::toggleState() {
  booleanState = !booleanState;
}

template <GPs STATE> bool m_GatePoint<STATE>::_is_disconnected() const {
  return splines.empty();
}
const Touchable::Id m_Gate::checkPointCollision(Vector2 pos) const {
  for (auto &x : _inPoints)
    if (const Id t = x->checkPointCollision(pos))
      return t;
  for (auto &x : _outPoints)
    if (const Id t = x->checkPointCollision(pos))
      return t;
  if (CheckCollisionPointRec(pos, _rect()))
    return this->id();
  return Id::Null;
}
template <GPs STATE>
m_Spline *m_GatePoint<STATE>::get_spline() const
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
bool Light::_isOn() const { return _inPoints.front()->booleanState; }
void Light::draw(const GGS &tc) const {
  _boxDraw(tc, _isOn() ? GREEN : RED);
  _pointDraw(tc);
}
void Switch::draw(const GGS &tc) const {
  _boxDraw(tc, _isOn ? GREEN : RED);
  _pointDraw(tc);
}
void Switch::_circuitUpdate() { _outPoints.front()->booleanState = _isOn; }
void Switch::_eventUpdate(const GGS &tc) {
  using namespace GateWindow;
  if (is_clicked(tc) && isMouseState(MouseState::running)) {
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
void NorGate::_circuitUpdate() {
  bool output = false;
  for (const auto &x : _inPoints) {
    output |= x->booleanState;
  }
  _outPoints.front()->booleanState = !output;
}
void XorGate::_circuitUpdate() {
  bool output = false;
  for (const auto &x : _inPoints) {
    output ^= x->booleanState;
  }
  _outPoints.front()->booleanState = output;
}
void NAndGate::_circuitUpdate() {
  bool output = true;
  for (const auto &x : _inPoints) {
    output &= x->booleanState;
  }
  _outPoints.front()->booleanState = !output;
}
template <GPs STATE>
m_GatePoint<STATE>::m_GatePoint(const m_Gate &gate, const Chars &text)
    : _gate(gate), _text(text) {}
void RSff::_updateOutput() {
  switch (savedState) {
  case LatchS::q:
    _outPoints[0]->booleanState = true;
    _outPoints[1]->booleanState = false;
    break;
  case LatchS::nQ:
    _outPoints[0]->booleanState = false;
    _outPoints[1]->booleanState = true;
    break;
  case LatchS::invalid:
    _outPoints[0]->booleanState = true;
    _outPoints[1]->booleanState = true;
    break;
  }
}

void RSff::_circuitUpdate() {
  auto R_S = _inPoints[0]->booleanState;
  auto clk_S = _inPoints[1]->booleanState;
  auto S_S = _inPoints[2]->booleanState;
  if (clkTrigger.isTriggered(clk_S)) {
    if (R_S == false && S_S == true)
      savedState = LatchS::q;
    if (R_S == true && S_S == false)
      savedState = LatchS::nQ;
    if (R_S == true && S_S == true)
      savedState = LatchS::invalid;
  }
  _updateOutput();
}
void JKff::_updateOutput() {
  _outPoints[0]->booleanState = savedState;
  _outPoints[1]->booleanState = !savedState;
}
void JKff::_circuitUpdate() {
  auto J_S = _inPoints[0]->booleanState;
  auto clk_S = _inPoints[1]->booleanState;
  auto K_S = _inPoints[2]->booleanState;
  if (clkTrigger.isTriggered(clk_S)) {
    if (J_S == false && K_S == true)
      savedState = false;
    if (J_S == true && K_S == false)
      savedState = true;
    if (J_S == true && K_S == true)
      savedState = !savedState;
  }
  _updateOutput();
}
bool ClkTrigger::isTriggered(bool isClkUP) {
  if (isClkUP == lastClkUp)
    return false;
  bool ans;
  if (lastClkUp == false && clkTS == ClkTriggerS::up)
    ans = true;
  else if (lastClkUp == true && clkTS == ClkTriggerS::down)
    ans = true;
  else
    ans = false;
  lastClkUp = isClkUP;
  return ans;
}
JKff::JKff(Vector2 pos)
    : m_Gate(pos, 50.0f, 40.0f, GateName::JK_FF, false, {"J", "Clk", "K"},
             {"Q", "Q`"}) {}

RSff::RSff(Vector2 pos)
    : m_Gate(pos, 50.0f, 40.0f, GateName::RS_FF, false, {"R", "Clk", "S"},
             {"Q", "Q`"}) {}
void RSff::setClkTriggerState(ClkTriggerS state) { clkTrigger.clkTS = state; }

void JKff::setClkTriggerState(ClkTriggerS state) { clkTrigger.clkTS = state; }
ClkPulse::ClkPulse(Vector2 pos)
    : m_Gate(pos, GateName::CLK_PULSE, 0, 1, false), _lastTime(GetTime()) {}
void ClkPulse::_circuitUpdate() {
  if (GetTime() >= _lastTime + halfPulseTime) {
    _isOn = !_isOn;
    _outPoints[0]->booleanState = _isOn;
    _lastTime = GetTime();
  }
}
void ClkPulse::draw(const GGS &tc) const {
  _boxDraw(tc, _isOn ? GREEN : RED);
  _pointDraw(tc);
}
m_Gate::m_Gate(Vector2 pos, const Chars &text, usize inPointnrMin,
               usize outPointnrMin, bool dynamicInput)
    : Draggable(pos), gateName(text), _inPointnrMin(inPointnrMin),
      _outPointnr(outPointnrMin), _dynamicInput(dynamicInput) {
  _init();
}
m_Gate::m_Gate(Vector2 pos, float width, float minHeight, const Chars &text,
               usize inPointnrMin, usize outPointnrMin, bool dynamicInput)
    : Draggable(pos), gateName(text), _inPointnrMin(inPointnrMin),
      _outPointnr(outPointnrMin), _width(width), _minHeight(minHeight),
      _dynamicInput(dynamicInput) {
  _init();
}
m_Gate::m_Gate(Vector2 pos, float width, const Chars &text, usize inPointnrMin,
               usize outPointnrMin, bool dynamicInput)
    : Draggable(pos), gateName(text), _inPointnrMin(inPointnrMin),
      _outPointnr(outPointnrMin), _width(width), _dynamicInput(dynamicInput) {
  _init();
}
m_Gate::m_Gate(Vector2 pos, float width, float minHeight, const Chars &text,
               bool dynamicInput, std::initializer_list<const Chars> inputText,
               std::initializer_list<const Chars> outputText)
    : Draggable(pos), gateName(text),
      _inPointnrMin(inputText.size()), _outPointnr(outputText.size()), _width(width),
      _minHeight(minHeight), _dynamicInput(dynamicInput) {
  _init(inputText, outputText);
}
AndGate::AndGate(Vector2 pos)
    : m_Gate(pos, GateName::AND, 2, 1, true) {}
OrGate::OrGate(Vector2 pos)
    : m_Gate(pos, GateName::OR, 2, 1, true) {}

NotGate::NotGate(Vector2 pos)
      : m_Gate(pos, GateName::NOT, 1, 1, false){}

NorGate::NorGate(Vector2 pos)
      : m_Gate(pos, GateName::NOR, 2, 1, true) {}


NAndGate::NAndGate(Vector2 pos)
      : m_Gate(pos, GateName::NAND, 2, 1, true) {}
XorGate::XorGate(Vector2 pos)
      : m_Gate(pos, GateName::XOR, 2, 1, false) {}


Light::Light(Vector2 pos)
      : m_Gate(pos, 50, GateName::LIGHT, 1, 0, false) {}


Switch::Switch(Vector2 pos)
      : m_Gate(pos, 60, GateName::SWITCH, 0, 1, false) {}
