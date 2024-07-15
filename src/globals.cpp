#include "globals.h"
#include "gate.h"
#include "ui.h"
#include <memory>
#include <raylib.h>
namespace GameManager::GateWindow {
TouchableCollection tc;
MouseState mouseState = MouseState::editing;
bool isMouseState(MouseState ms) { return ms==mouseState; }
Gates gates;
Camera2D camera2d{
    .offset = {0, 0},
    .target = {0, 0},
    .rotation = 0.0,
    .zoom = 1.0,
};
void draw() {
  m_Spline::SplinesDraw(); // Drawing splines.
  for (auto &x : gates)
    x->draw(); // Drawing all gates.
}
void update() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    m_Spline::removeCurrentSpline();
  for (auto &x : gates)
    x->update(); // Updating all gates.
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
TouchableCollection tc;
using namespace GameManager::GateName;
using namespace Menu_Options;
SelectBar _selectBar(&tc, Vector2{0, 0}, MENU, BAR_SIZE, 14);
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
void GameManager::updateEvent() {
  UI::tc.click_update();
  GateWindow::tc.click_update();
}
