#pragma once
#include <cstdint>
#include <deque>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <utility>
#include <type_traits>
using i32 = int;
using i64 = long long;
using u32 = unsigned int;
using u64 = unsigned long long;
using u8 = char;
using usize = uintptr_t;
using std::unique_ptr;
using std::make_unique;

template <typename T> using Vec = std::vector<T>;
template <typename T> using Box = std::unique_ptr<T>;
template <typename T> using Rc = std::shared_ptr<T>;
using Rect = Rectangle;
const u8 CHARS_MAX_SIZE = 64;
enum class Error {
  OVERFLOW_ERROR,
  OUTOFBOUND_ERROR,
  ITEMNOTFOUND_ERROR,
  OK,
};

class Chars {
  using CharArray = std::array<char, CHARS_MAX_SIZE>;
  // Chars are array of character,That are stack allocated,Each heap allocated
  // String is not worth it
  using Iterator = CharArray::iterator;
  using CIterator = CharArray::const_iterator;

public:
  Chars(const char *chars);
  Chars() : Chars("") {}
  Chars(const Chars &other) noexcept;
  Chars(const std::string &str) : Chars(str.c_str()) {}
  char *c_str();
  const char *c_str()const;
  char at(int a);
  explicit operator bool() const;
  char operator[](int a);
  auto operator<=>(const Chars& ch)const;
  Error push_back(char c);
  char pop_back();
  Error insert(char c, u8 position);
  Error erase(u8 position);
  [[nodiscard]] u8 length() const;
  [[nodiscard]] bool empty() const;
  Iterator begin();
  Iterator end();

  CIterator begin() const;
  CIterator end() const;

private:
  CharArray core;
  Iterator end_ptr;
};

Vector2 operator+(const Vector2 &v1, const Vector2 &v2);
void operator+=(Vector2 &v1, const Vector2 &v2);
void operator-=(Vector2 &v1, const Vector2 &v2);
Vector2 operator-(const Vector2 &v1, const Vector2 &v2);

Vector2 operator*(const Vector2 &v, float scalar);

Vector2 operator/(const Vector2 &v, float scalar);
Rectangle operator+(const Rectangle &r1, const Vector2 pos);
struct RectSize {
  float width;
  float height;
  RectSize(Vector2 size) : width(size.x), height(size.y) {}
  RectSize(float width, float height) : width(width), height(height) {}
  Vector2 toVec2();
};
Rectangle rectFromCenter(Vector2 center, float width, float height);
Rectangle rectFromCenter(Vector2 center, RectSize rectSize);
Rectangle rectFromPos(Vector2 position, float width, float height);
Rectangle rectFromPos(Vector2 position, RectSize rectSize);
struct Circle {
  Vector2 center;
  float radius;
};
void DrawCircleCir(Circle cir, Color color);
bool CheckCollisionPointCircle(Vector2 point, Circle cir);
std::string vectorToString(const std::vector<std::string> &vec);

int levenshteinDistance(const std::string &s1, const std::string &s2);
int levenshteinDistance(const Chars &s1, const Chars &s2);

// Function to perform fuzzy search in a vector of strings

// Function to perform fuzzy search in a vector of strings
std::vector<std::string> fuzzySearch(const std::string &query,
                                     const std::vector<std::string> &vec);
std::vector<Chars> fuzzySearch(const Chars &query,
                               const std::vector<Chars> &vec);

Vector2 rect_pos(const Rectangle &rect);
void drawRectangleWithLines(Rectangle rect, Color rect_color = WHITE,
                            Color line_color = BLACK, float width = 3);
Vector2 position(Rectangle rect);

Vector2 middle(Rectangle rect);

void DrawRectangleGradientHRec(Rectangle rect, Color color1, Color color2);
void DrawCircleLinesCir(Circle cir, Color color, float width);
void DrawText(std::string text, Vector2 position, int fontSize = 11,
              Color color = BLACK);

void DrawText(Chars text, Vector2 position, int fontSize = 11,
              Color color = BLACK);

template <typename T> class BoundedQueue {
public:
  BoundedQueue(size_t max_size) : max_size(max_size) {}

  void push(T item);
  usize size();
  Error pop();
  auto begin();
  auto end();
  T at(usize x);

private:
  std::deque<T> queue;
  size_t max_size;
};
template <typename T> void BoundedQueue<T>::push(T item) {
  if (queue.size() == max_size) {
    queue.pop_front();
  }
  queue.push_back(item);
}
template <typename T> usize BoundedQueue<T>::size() { return queue.size(); }
template <typename T> Error BoundedQueue<T>::pop() {
  if (queue.empty()) {
    return Error::OUTOFBOUND_ERROR;
  }
  queue.pop_front();
  return Error::OK;
}
template <typename T> auto BoundedQueue<T>::begin() { return queue.begin(); }
template <typename T> auto BoundedQueue<T>::end() { return queue.end(); }
template <typename T> T BoundedQueue<T>::at(usize x) { return queue.at(x); }
std::ostream& operator<<(std::ostream& os,const Chars& ch);
//Generic function.Don't move them.
template <typename T> Error eraseItem(Vec<T> &vec, const T item);
template <typename T>
std::vector<T> vectorNemement(size_t n, const std::function<T()> &func);
template <typename T>
std::vector<T> vectorNemement(usize n, const std::function<T()> &func) {
  std::vector<T> vec;
  vec.reserve(n); // Reserve space to avoid reallocations
  std::generate_n(std::back_inserter(vec), n, [func]() { return func(); });
  return vec;
}
template <typename T> Error eraseItem(Vec<T> &vec, const T item) {
  auto it = std::find(vec.begin(), vec.end(), item);
  if (it == vec.end()) {
    return Error::ITEMNOTFOUND_ERROR;
  } else {
    vec.erase(it);
    return Error::OK;
  }
}
template <typename T,typename... Args>
std::vector<T> make_vector(Args&&... args) {
    std::vector<T> result;
    result.reserve(sizeof...(args));
    (result.emplace_back(std::forward<Args>(args)), ...);
    return result;
}
