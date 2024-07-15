#pragma once
#include "basic_template.h"
#include <raylib.h>
#include <raymath.h>
namespace GameManager {
void init(); 
void tcUpdate();
enum class UsedCamera{
	noCamera,
	gateCamera,
};
Vector2 getGlobalMousePosition(const UsedCamera camera);
namespace Debugger {
void push_message(const Chars &&text);
void push_message(const Chars &text);
void draw();
} // namespace Debugger
namespace UI::Menu_Options {
const Chars MOUSE_TOGGLE = "RUN/EDIT";
const Chars DELETE = "DELETE";
const Chars CREATE = "CREATE";
//const Vec<Chars> MENU = {MOUSE_TOGGLE, DELETE, CREATE};
} // namespace UI::Menu_Options
namespace GateName {
const Chars AND = "AND ";
const Chars OR = "OR";
const Chars NOT = "NOT";
const Chars LIGHT = "LIGHT";
const Chars SWITCH = "SWITCH";
const Vec<Chars> GATES_NAME = {AND, OR, NOT, LIGHT, SWITCH};
} // namespace GateName

namespace UI {
enum class UIState {
  NOTIHING,
  MAIN_MENU,
  CREATE_MENU,
};
// calculated first,As they handle event.
const RectSize BAR_SIZE{50, 25};
// namespace Options
[[nodiscard]] UIState _nextState(UIState); // What will be the next state
                                           // of the ui given the current state
void _stateInit(UIState); // Initilize all the variabe if the state changes.
void _stateUpdate();
void draw();
void update();
} // namespace UI
namespace GateWindow {
enum class MouseState {
  editing,
  running,
};
Camera2D getCamera();
bool isMouseState(MouseState mouseState);
void draw();
void update();
void create_gate(const Chars &gate);
} // namespace GateWindow
} // namespace GameManager
