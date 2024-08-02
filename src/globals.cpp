#include "globals.h"
#include "basic_template.h"
#include "gate.h"
#include "ui.h"
#include <algorithm>
#include <memory>
#include <raylib.h>
#include <raymath.h>
namespace GameManager::GateWindow {
TouchableCollection tc(UsedCameraS::gateCamera);
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
  if (auto touchable = GateWindow::tc.getSelected()) {
    auto it =
        std::find_if(gates.begin(), gates.end(), [touchable](const auto &x) {
          return x.get() == touchable;
        });
    if (it == gates.end()) {
      Debugger::push_message("The gates vec doesn;t have the object.");
      return;
    }
    gates.erase(it);
  }
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
    x->draw(); // Drawing all gates.
}
void _cameraMove() {
  float roll = GetMouseWheelMove();
  if (roll != 0) {
    float zoomIncrease = 0.1f * roll;
    Vector2 distance = camera2d.offset - GetMousePosition();
    camera2d.offset += distance * zoomIncrease / camera2d.zoom;
    camera2d.zoom += (0.1f * roll);
    Debugger::push_message(std::to_string(camera2d.zoom));
  }
  camera2d.zoom = Clamp(camera2d.zoom, 0.1, 4);
  camera2d.target += Vector2Normalize(arrowDirection) / camera2d.zoom;
}
void update() {
  _arrowUpdate();
  _cameraMove();
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    m_Spline::removeCurrentSpline();
  for (auto &x : gates)
    x->update(); // Updating all gates.
  m_Spline::signalPasses();
}
void create_gate(const Chars &gateName) {
  using GateWindow::tc;
  using namespace GateName;
  using std::make_unique;
  Gate gate;
  auto mouseGlobalPos = getGlobalMousePosition(UsedCameraS::gateCamera);
  if (gateName == AND) {
    gate = make_unique<AndGate>(&tc, mouseGlobalPos);
  } else if (gateName == OR) {
    gate = make_unique<OrGate>(&tc, mouseGlobalPos);
  } else if (gateName == NOT) {
    gate = make_unique<NotGate>(&tc, mouseGlobalPos);
  } else if (gateName == NOR) {
    gate = make_unique<NorGate>(&tc, mouseGlobalPos);
  } else if (gateName == NAND) {
    gate = make_unique<NAndGate>(&tc, mouseGlobalPos);
  } else if (gateName == XOR) {
    gate = make_unique<XorGate>(&tc, mouseGlobalPos);
  } else if (gateName == LIGHT) {
    gate = make_unique<Light>(&tc, mouseGlobalPos);
  } else if (gateName == SWITCH) {
    gate = make_unique<Switch>(&tc, mouseGlobalPos);
  } else if (gateName == RS_FF) {
    gate = make_unique<RSff>(&tc, mouseGlobalPos);
  } else if (gateName == JK_FF) {
    gate = make_unique<JKff>(&tc, mouseGlobalPos);
  } else {
    Debugger::push_message("Gate name is not addressed.");
    return;
  }
  gates.push_back(std::move(gate));
}

// Usage:
} // namespace GameManager::GateWindow
  //
namespace GameManager::Debugger {
BoundedQueue<Chars> bq(10);
Vector2 positon = Vector2{10, 10};
int textspace = 3;
int fontSize = 11;
void draw() {
  for (usize i = 0; i < bq.size(); i++) {
    float y_offset = float(textspace + fontSize) * float(i);
    auto pos = positon + Vector2{0, y_offset};
    drawText(bq.at(i), pos);
  }
}
void push_message(const Chars &&text) { bq.push(text); }
void push_message(const Chars &text) { bq.push(text); }
} // namespace GameManager::Debugger
  //
namespace GameManager::UI {

UIState currentState = UIState::NOTIHING;
TouchableCollection tc(UsedCameraS::noCamera);
Vec<Chars> _menuBar() {
  using namespace GateWindow;
  using namespace Menu_Options;
  Vec<Chars> ans;
  ans.reserve(10);
  if (isMouseState(MouseState::editing)) {
    ans.push_back(CREATE);
    if (GateWindow::tc.isSelected()) {
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
SelectBar _selectBar(&tc, Vector2{0, 0}, _menuBar(), BAR_SIZE, 14,
                     TextPositionS::center);
SearchBar _searchBar(&tc, Vector2{0, 0}, GATES_NAME);
UIState _nextState(UIState current_state) {
  switch (current_state) {
  case UIState::NOTIHING:
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      return UIState::MAIN_MENU;
    }
    break;
  case UIState::MAIN_MENU:
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (auto selectedOption = _selectBar.getClick()) {
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
      if (auto selectedOption = _searchBar.getClick()) {
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
    _searchBar.toSelected();
    break;
  case UIState::MAIN_MENU:
    _selectBar.options = _menuBar();
    _selectBar.setPos(GetMousePosition());
    _selectBar.toSelected();
    break;
  case UIState::MOUSE_CHANGE:
    GateWindow::_toggleMouseState();
    break;
  case UIState::DELETE:
    GateWindow::_deleteGate();
    break;
  }
}
void draw() {
  switch (currentState) {
  case UIState::NOTIHING:
    break;
  case UIState::MAIN_MENU:
    _selectBar.draw();
    break;
  case UIState::CREATE_MENU:
    _searchBar.draw();
  case UIState::MOUSE_CHANGE:
    break;
  case UIState::DELETE:
    break;
  }
}
void update() {
  _stateUpdate();
  switch (currentState) {
  case UIState::NOTIHING: // Created this for exhaustic match.Like in rust.
    break;
  case UIState::MAIN_MENU:
    break;
  case UIState::CREATE_MENU:
    _searchBar.CharUpdate();
    break;
  case UIState::MOUSE_CHANGE:
    break;
  case UIState::DELETE:
    break;
  }
}
} // namespace GameManager::UI
namespace GameManager {

void tcUpdate() {
  if (UI::tc.click_update())
    return;
  if (GateWindow::tc.click_update())
    return;
}
Vector2 getGlobalMousePosition(const UsedCameraS camera) {
  switch (camera) {
  case UsedCameraS::noCamera:
    return GetMousePosition();
  case UsedCameraS::gateCamera:
    return GetScreenToWorld2D(GetMousePosition(), GateWindow::camera2d);
  }
}

void init() {
  GateWindow::_initGateCamera();
  Resource::_init();
}
} // namespace GameManager
void Resource::_init() {
  Fonts::LUMITIVE_FONT = LoadFont("./Resources/fonts/luminova_bold.otf");
}

// extern initilize
Font Resource::Fonts::LUMITIVE_FONT{};
