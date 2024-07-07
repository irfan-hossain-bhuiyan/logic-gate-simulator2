
#include "gate.h"
#include "basic_template.h"
#include "ui.h"
#include <algorithm>
#include <raylib.h>
Spline* Spline::CURRENT_SPLINE=nullptr;
float Gate::_rectHeight() {
  auto temp = std::max(_inPointnr(), _outPointnr());
  auto pointHeight = (temp + 1) * MIN_POINT_DISTANCE;
  return std::max(pointHeight, MIN_HEIGHT);
}
RectSize Gate::_rectsize() { return RectSize(WIDTH, _rectHeight()); }
void Gate::draw() {
  auto lineWidth = is_clicked() ? 3 : 1;
  Label(rectFromPos(NodePos,_rectsize()), text, RED).draw(lineWidth);
  // Drawing in point
}

void Gate::addSpline(Spline const* spline){
	if()
}
