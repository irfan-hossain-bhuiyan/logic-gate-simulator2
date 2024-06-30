#include <iostream>
#include <type_traits>

enum class DayOfWeek {
  Monday,
  Tuesday,
  Wednesday,
  Thursday,
  Friday,
  Saturday,
  Sunday
};

void printDay(DayOfWeek day) {
  switch (day) {
  case DayOfWeek::Monday:
    std::cout << "Monday" << std::endl;
    break;
  case DayOfWeek::Tuesday:
    std::cout << "Tuesday" << std::endl;
    break;
  case DayOfWeek::Wednesday:
    std::cout << "Wednesday" << std::endl;
    break;
  case DayOfWeek::Thursday:
    std::cout << "Thursday" << std::endl;
    break;
  case DayOfWeek::Friday:
    std::cout << "Friday" << std::endl;
    break;
  case DayOfWeek::Sunday:
    std::cout << "Sunday" << std::endl;
    break;
  }

  }

int main() {
  printDay(DayOfWeek::Wednesday);
  // Uncommenting the line below would trigger a compile-time error due to the
  // static assertion: printDay(static_cast<DayOfWeek>(42));

  return 0;
}
