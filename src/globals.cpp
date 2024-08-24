#include "globals.hpp"
#include "basic_template.hpp"
#include "gate.hpp"
#include "ui.hpp"
#include <memory>
#include <raylib.h>
#include <raymath.h>
using IdT = Touchable::Id;
namespace GameManager::GateWindow {
GateGlobalState ggs;
Vector2 arrowDirection;
MouseState mouseState = MouseState::editing;
[[nodiscard]] bool isMouseState(MouseState ms) { return ms == mouseState; }
Gates gates;
Camera2D camera2d;
void _initGateCamera() {
  Vector2 middle =
      Vector2{float(GetScreenWidth()) / 2, float(GetScreenHeight()) / 2};
  GateWindow::camera2d = Camera2D{
      .offset = middle, .target = {0, 0}, .rotation = 0.0f, .zoom = 1.0f};
}
void _deleteGate() {
  for (auto it = gates.begin(); it != gates.end();) {
    auto &x = *it;
    if (GateWindow::ggs.isSelected(*x.get())) {
      ggs.removeSelection(*x.get());
      it = gates.erase(it);
    } else {
      it++;
    }
    if (GateWindow::ggs.hasSelected() == false)
      return;
  }
  Debugger::messages << "Not all elements can be deleted." << end;
}

Camera2D getCamera() { return camera2d; }
void _arrowUpdate() {
  arrowDirection = {0, 0};
  if (IsKeyDown(KEY_RIGHT))
    arrowDirection.x += 1;
  if (IsKeyDown(KEY_LEFT))
    arrowDirection.x -= 1;
  if (IsKeyDown(KEY_UP))
    arrowDirection.y -= 1;
  if (IsKeyDown(KEY_DOWN))
    arrowDirection.y += 1;
}
void _toggleMouseState() {
  const auto swap = []() {
    switch (mouseState) {
    case MouseState::editing:
      SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
      return MouseState::running;
    case MouseState::running:
      SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      return MouseState::editing;
    };
  };
  mouseState = swap();
}
void draw() {
  m_Spline::SplinesDraw(); // Drawing splines.
  for (auto &x : gates)
    x->draw(GateWindow::ggs); // Drawing all gates.
}
void _cameraMove() {
  float roll = GetMouseWheelMove();
  if (roll != 0) {
    float zoomIncrease = 0.1f * roll;
    Vector2 distance = camera2d.offset - GetMousePosition();
    camera2d.offset += distance * zoomIncrease / camera2d.zoom;
    camera2d.zoom += (0.1f * roll);
  }
  camera2d.zoom = Clamp(camera2d.zoom, 0.1, 4);
  camera2d.target += Vector2Normalize(arrowDirection) / camera2d.zoom;
}
void objectUpdate() {
  _arrowUpdate(); // Arrow used to move around in the gate Window.
  _cameraMove();  // Moving camera using the arrow.
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    m_Spline::removeCurrentSpline();
  for (auto &x : gates) {
    x->update(GateWindow::ggs); // Updating all gates.
    // Debugger::messages << x->NodePos << end;
  }
  m_Spline::signalPasses();
}
void create_gate(const Chars &gateName) {
  using namespace GateName;
  using std::make_unique;
  Gate gate;
  auto mouseGlobalPos = getGlobalMousePosition(UsedCameraS::gateCamera);
  if (gateName == AND) {
    gate = make_unique<AndGate>(mouseGlobalPos);
  } else if (gateName == OR) {
    gate = make_unique<OrGate>(mouseGlobalPos);
  } else if (gateName == NOT) {
    gate = make_unique<NotGate>(mouseGlobalPos);
  } else if (gateName == NOR) {
    gate = make_unique<NorGate>(mouseGlobalPos);
  } else if (gateName == NAND) {
    gate = make_unique<NAndGate>(mouseGlobalPos);
  } else if (gateName == XOR) {
    gate = make_unique<XorGate>(mouseGlobalPos);
  } else if (gateName == LIGHT) {
    gate = make_unique<Light>(mouseGlobalPos);
  } else if (gateName == SWITCH) {
    gate = make_unique<Switch>(mouseGlobalPos);
  } else if (gateName == RS_FF) {
    gate = make_unique<RSff>(mouseGlobalPos);
  } else if (gateName == JK_FF) {
    gate = make_unique<JKff>(mouseGlobalPos);
  } else if (gateName == CLK_PULSE) {
    gate = make_unique<ClkPulse>(mouseGlobalPos);
  } else {
    // Debugger::messages << "Gate name is not addressed." <<end;
    return;
  }
  gates.push_back(std::move(gate));
}
const Touchable::Id _clickUpdate() {
  for (auto x = gates.rbegin(); x != gates.rend(); x++) {
    const Touchable::Id clicked = ggs.touchUpdate(**x);
    if (clicked.isNull() == false) {
      return clicked;
    }
  }
  return Touchable::Id::Null;
}

} // namespace GameManager::GateWindow
  //
namespace GameManager::Debugger {
Vector2 positon = Vector2{10, 10};
float textspace = 5;
float fontSize = 19;
void draw() {
  for (usize i = 0; i < messages.size(); i++) {
    float y_offset = float(textspace + fontSize) * float(i);
    auto pos = positon + Vector2{0, y_offset};
    drawText(messages.at(i), pos, fontSize, RED);
  }
}
} // namespace GameManager::Debugger
  //
StringQueue GameManager::Debugger::messages(10);
namespace GameManager::UI {

UIState currentState = UIState::NOTIHING;
UIGlobalState ugs;
Vec<Chars> _menuBar() {
  using namespace GateWindow;
  using namespace Menu_Options;
  Vec<Chars> ans;
  ans.reserve(10);
  if (isMouseState(MouseState::editing)) {
    ans.push_back(CREATE);
    if (GateWindow::ggs.hasSelected()) {
      ans.push_back(DELETE);
    }
    ans.push_back(MOUSE_TOGGLE);
  } else if (isMouseState(MouseState::running)) {
    ans.push_back(MOUSE_TOGGLE);
  }
  return ans;
}
using namespace GameManager::GateName;
using namespace Menu_Options;
SelectBar _selectBar(Vector2{0, 0}, _menuBar(), BAR_SIZE, 14,
                     TextPositionS::center);
SearchBar _searchBar(Vector2{0, 0}, GATES_NAME);
std::array<Touchable *, 2> ui = {&_selectBar, &_searchBar};
UIState _nextState(UIState current_state) {
  switch (current_state) {
  case UIState::NOTIHING:
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      return UIState::MAIN_MENU;
    }
    break;
  case UIState::MAIN_MENU:
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (auto selectedOption = _selectBar.getClick(ugs)) {
        if (selectedOption == CREATE)
          return UIState::CREATE_MENU;
        else if (selectedOption == MOUSE_TOGGLE) {
          // GateWindow::_toggleMouseState();
          return UIState::MOUSE_CHANGE;
        } else if (selectedOption == DELETE) {
          return UIState::DELETE;
        }
      } else {
        return UIState::NOTIHING;
      }
    }
    break;
  case UIState::CREATE_MENU:
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (auto selectedOption = _searchBar.getClick(ugs)) {
        GateWindow::create_gate(selectedOption);
        return UIState::NOTIHING; // To turn off the searchbar.
      } else {
        return UIState::NOTIHING; // Turn off if select elsewhere.
      }
    }
    break;
  case UIState::MOUSE_CHANGE:
    return UIState::NOTIHING;
  case UIState::DELETE:
    return UIState::NOTIHING;
  }
  return current_state;
}
void _stateUpdate() {
  auto newState = _nextState(currentState);
  if (newState != currentState) {
    _stateInit(newState);
  }
  currentState = newState;
}
void _stateInit(UIState currentState) {
  switch (currentState) {
  case UIState::NOTIHING:
    break;
  case UIState::CREATE_MENU:
    _searchBar.setPos(GetMousePosition());
    _searchBar.toSelected(ugs);
    break;
  case UIState::MAIN_MENU:
    _selectBar.options = _menuBar();
    _selectBar.setPos(GetMousePosition());
    ugs.toSelect(_selectBar);
    break;
  case UIState::MOUSE_CHANGE:
    GateWindow::_toggleMouseState();
    break;
  case UIState::DELETE:
    GateWindow::_deleteGate();
    break;
  }
}
const Touchable::Id _clickUpdate() {
  switch (currentState) {
  case UIState::NOTIHING:
    break;
  case UIState::MAIN_MENU:
    return ugs.touchUpdate(_selectBar);
    break;
  case UIState::CREATE_MENU:
    return ugs.touchUpdate(_searchBar);
  case UIState::MOUSE_CHANGE:
    break;
  case UIState::DELETE:
    break;
  }
  return IdT::Null;
}

void draw() {
  switch (currentState) {
  case UIState::NOTIHING:
    break;
  case UIState::MAIN_MENU:
    _selectBar.draw(ugs);
    break;
  case UIState::CREATE_MENU:
    _searchBar.draw(ugs);
  case UIState::MOUSE_CHANGE:
    break;
  case UIState::DELETE:
    break;
  }
}

void _objectUpdate() {
  switch (currentState) {
  case UIState::NOTIHING: // Created this for exhaustic match.Like in rust.
    break;
  case UIState::MAIN_MENU:
    break;
  case UIState::CREATE_MENU:
    _searchBar.CharUpdate(ugs); // SelectBar doesn't have update
    break;
  case UIState::MOUSE_CHANGE:
    break;
  case UIState::DELETE:
    break;
  }
}
void objectUpdate() {
  _stateUpdate();
  _objectUpdate();
}
} // namespace GameManager::UI
namespace GameManager {
using GateWindow::gates;
using UI::ui;
void clickUpdate() {
#pragma region uiClickUpdate
  {
    bool uiTouched =! UI::_clickUpdate().isNull();
    UI::ugs.afterTouchUpdate();
    if (uiTouched)
      return;
  }
#pragma endregion

#pragma region gateClickUpdate
  {
    bool gateTouched =! GateWindow::_clickUpdate().isNull();
    GateWindow::ggs.afterTouchUpdate();
    if (gateTouched)
      return;
  }
#pragma endregion
}
Vector2 getScreenToWorld(Vector2 pos, const UsedCameraS camera) {
  switch (camera) {
  case UsedCameraS::noCamera:
    return pos;
  case UsedCameraS::gateCamera:
    return GetScreenToWorld2D(pos, GateWindow::camera2d);
  }
}
void _gsInit() {
  UI::ugs.frameInit();
  GateWindow::ggs.frameInit();
}
Vector2 getGlobalMousePosition(const UsedCameraS camera) {
  return getScreenToWorld(GetMousePosition(), camera);
}

void init() {
  GateWindow::_initGateCamera();
  Resource::_init();
}
void update() {
  GameManager::_gsInit();
  GameManager::clickUpdate();
  UI::objectUpdate();
  GateWindow::objectUpdate();
}

} // namespace GameManager
void Resource::_init() {
  Fonts::LUMITIVE_FONT = LoadFont("./Resources/fonts/luminova_bold.otf");
  // Fonts::LUMITIVE_FONT=GetFontDefault();
}

// extern initilize
Font Resource::Fonts::LUMITIVE_FONT{};
