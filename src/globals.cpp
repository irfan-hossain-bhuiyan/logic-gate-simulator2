#include "globals.h"
#include "gate.h"
#include "ui.h"
#include <memory>
#include <raylib.h>
#include <raymath.h>
namespace GameManager::GateWindow {
TouchableCollection tc(UsedCamera::gateCamera);
Vector2 arrowDirection;
MouseState mouseState = MouseState::editing;
[[nodiscard]] bool isMouseState(MouseState ms) { return ms == mouseState; }
Gates gates;
Camera2D camera2d{};
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
  if (roll != 0)
    Debugger::push_message(std::to_string(camera2d.zoom));
  camera2d.zoom += (0.1 * roll);
  camera2d.target += Vector2Normalize(arrowDirection);
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
  if (gateName == AND) {
    gate = make_unique<AndGate>(&tc, GetMousePosition(), "AND");
  } else if (gateName == OR) {
    gate = make_unique<OrGate>(&tc, GetMousePosition(), "OR");
  } else if (gateName == NOT) {
    gate = make_unique<NotGate>(&tc, GetMousePosition(), "NOT");
  } else if (gateName == LIGHT) {
    gate = make_unique<Light>(&tc, GetMousePosition(), "LIGHT");
  } else if (gateName == SWITCH) {
    gate = make_unique<Switch>(&tc, GetMousePosition(), "SWITCH");
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
    float y_offset = float(textspace + fontSize) * i;
    auto pos = positon + Vector2{0, y_offset};
    DrawText(bq.at(i), pos);
  }
}
void push_message(const Chars &&text) { bq.push(text); }
void push_message(const Chars &text) { bq.push(text); }
} // namespace GameManager::Debugger
  //
namespace GameManager::UI {

UIState currentState = UIState::NOTIHING;
TouchableCollection tc(UsedCamera::noCamera);
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
SelectBar _selectBar(&tc, Vector2{0, 0}, _menuBar(), BAR_SIZE, 14);
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
          GateWindow::_toggleMouseState();
          return UIState::NOTIHING;
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
  }
}
} // namespace GameManager::UI

void GameManager::tcUpdate() {
  UI::tc.click_update();
  GateWindow::tc.click_update();
}
Vector2 GameManager::getGlobalMousePosition(const UsedCamera camera) {
  switch (camera) {
  case UsedCamera::noCamera:
    return GetMousePosition();
  case UsedCamera::gateCamera:
    return GetScreenToWorld2D(GetMousePosition(), GateWindow::camera2d);
  }
}
