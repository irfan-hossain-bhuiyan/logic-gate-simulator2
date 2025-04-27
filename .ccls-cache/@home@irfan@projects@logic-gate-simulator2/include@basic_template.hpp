#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <vector>
#include <array>
#include <functional>
#include <iterator>
using i32 = int;
using i64 = long long;
using u32 = unsigned int;
using u64 = unsigned long long;
using u8 = char;
using usize = uintptr_t;
using std::make_unique;
using std::unique_ptr;

struct endl {};
static endl end;
template <typename T> using Vec = std::vector<T>;
template <typename T> using Box = std::unique_ptr<T>;
template <typename T> using Rc = std::shared_ptr<T>;
using Rect = Rectangle;
const u8 CHARS_MAX_SIZE = 16;
enum class ErrorS {
  OVERFLOW_ERROR,
  OUTOFBOUND_ERROR,
  ITEMNOTFOUND_ERROR,
  OK,
};
const float DEFAULT_FONT_SIZE = 13.0;
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
  Chars &operator=(const Chars &other) noexcept;
  Chars(const std::string &str) : Chars(str.c_str()) {}
  char *c_str();
  const char *c_str() const;
  char at(int a);
  explicit operator bool() const;
  char operator[](int a);
  auto operator<=>(const Chars &ch) const;
  bool operator==(const Chars &ch) const;
  ErrorS push_back(char c);
  char pop_back();
  ErrorS insert(char c, u8 position);
  ErrorS erase(u8 position);
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

Vector2 rectPos(const Rectangle &rect);
void drawRectangleWithLines(Rectangle rect, Color rect_color = WHITE,
                            Color line_color = BLACK, float width = 3);

Vector2 rectCenter(Rectangle rect);

void DrawRectangleGradientHRec(Rectangle rect, Color color1, Color color2);
void DrawCircleLinesCir(Circle cir, Color color, float width);
void drawText(const std::string &text, Vector2 position,
              float fontSize = DEFAULT_FONT_SIZE, Color color = BLACK);

void drawText(const Chars &text, Vector2 position,
              float fontSize = DEFAULT_FONT_SIZE, Color color = BLACK);
RectSize measureText(const Chars &text, float fontSize = DEFAULT_FONT_SIZE,
                     float spacing = 0);
template <typename T> class BoundedQueue {
public:
  BoundedQueue(size_t max_size) : max_size(max_size) {}

  void push(T item);
  usize size() const;
  ErrorS pop();
  auto cbegin() const;
  auto cend() const;
  const T &at(usize x) const;

private:
  std::deque<T> queue;
  size_t max_size;
};

class StringQueue {
private:
  BoundedQueue<std::string> bq;
  std::stringstream ss;

public:
  StringQueue(size_t maxSize) : bq(maxSize) {}
  template <typename T> StringQueue &operator<<(const T &t) {
    this->ss << t;
    return *this;
  }
  usize size() const { return bq.size(); }
  template <> StringQueue &operator<<(const endl &t) {
    bq.push(ss.str());
    ss.str("");
    ss.clear();
    return *this;
  }
  const std::string &at(usize i) const { return bq.at(i); }
};

RectSize textSize(const Chars &text, float size);
enum class TextPositionS;
Vector2 textPosition(Rectangle rect, const Chars &text,
                     float fontSize = DEFAULT_FONT_SIZE, float border = 3);
Vector2 textPosition(Rectangle rect, const Chars &text, TextPositionS textPos,
                     float fontSize = DEFAULT_FONT_SIZE, float border = 3);

// Template is needed.
template <typename T> ErrorS eraseItem(Vec<T> &collection, T item) {
  auto it = std::find(collection.begin(), collection.end(), item);
  if (it == collection.end())
    return ErrorS::ITEMNOTFOUND_ERROR;
  collection.erase(it);
  return ErrorS::OK;
}

template <typename T, typename... Args>
std::vector<T> make_vector(Args &&...args) {
  std::vector<T> result;
  result.reserve(sizeof...(args));
  (result.emplace_back(std::forward<Args>(args)), ...);
  return result;
}
template <typename T> void BoundedQueue<T>::push(T item) {
  if (queue.size() == max_size) {
    queue.pop_front();
  }
  queue.push_back(item);
}
template <typename T> usize BoundedQueue<T>::size() const {
  return queue.size();
}
template <typename T> ErrorS BoundedQueue<T>::pop() {
  if (queue.empty()) {
    return ErrorS::OUTOFBOUND_ERROR;
  }
  queue.pop_front();
  return ErrorS::OK;
}
template <typename T> auto BoundedQueue<T>::cbegin() const {
  return queue.begin();
}
template <typename T> auto BoundedQueue<T>::cend() const { return queue.end(); }
template <typename T> const T &BoundedQueue<T>::at(usize x) const {
  return queue.at(x);
}
template <typename T>
std::vector<T> vectorNemement(usize n, const std::function<T()> &func) {
  std::vector<T> vec;
  vec.reserve(n); // Reserve space to avoid reallocations
  std::generate_n(std::back_inserter(vec), n, [func]() { return func(); });
  return vec;
}

std::ostream &operator<<(std::ostream &os, const Vector2 &vec2);

template <typename T> class unorderedVec {
private:
  std::unordered_map<int, T> core;

public:
  int pushBack(T &&object) {
    int id;
    while (core.count(id) == 0) {
      id = rand();
    }
    core[id] = object;
    return id;
  }
  T &operator[](int id) { return core[id]; }
  std::optional<T &> get(int id) {
    if (core.count(id) == 0)
      return std::optional<T &>();
    T &obj = core[id];
    return std::optional(obj);
  }
};

template <typename T> struct Id {
  static inline const Id Null = Id{._core = 0};
  u64 _core;
  bool operator==(const Id &) const = default;
  const static Id<T> generateId() {
    static u64 idNum = 1;
    return Id<T>{._core = idNum++}; // Id with 0 will be null
  }
  bool isNull() const { return *this == Null; }
  auto operator<=>(const Id &) const = default;
  operator bool() const { return _core; }
};
// template<typename T>
// const Id<T> Id<T>::Null=Id<T>{._core=0};
template struct Id<std::string>;
