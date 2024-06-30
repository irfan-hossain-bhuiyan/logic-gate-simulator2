#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <queue>
#include <raylib.h>
#include <raymath.h>
#include <stdexcept>
#include <string>
#include <vector>
const std::string TRUE = "TRUE";
const std::string FALSE = "FalSE";
using i32 = int;
using i64 = long long;
using u32 = unsigned int;
using u64 = unsigned long long;
using u8 = char;
using usize = uintptr_t;
template <typename T> using Vec = std::vector<T>;
using Rect = Rectangle;
#define pii pair<int, int>
#define pb push_back
#define mp make_pair
// Loop macros for brevity
#define range(i, a, b) for (int i = (a); i < (b); ++i)
#define rangeb(i, a, b, s) for (i64 i = (a); i < (b) - (s) + 1; i += (s))
#define ranges(i, a, b, s) for (i64 i = (a); i < (b); i += (s))
#define rangerev(i, a, b) for (int i = (b) - 1; i >= (a); --i)
#define rangerevs(i, a, b, s) for (i64 i = (b) - 1; i >= (a); i -= s)
Vector2 operator+(const Vector2 &v1, const Vector2 &v2) {
  return Vector2Add(v1, v2);
}
void operator+=(Vector2 &v1, const Vector2 &v2) {
  v1.x += v2.x;
  v1.y += v2.y;
}
void operator-=(Vector2 &v1, const Vector2 &v2) {
  v1.x -= v2.x;
  v1.y -= v2.y;
}
Vector2 operator-(const Vector2 &v1, const Vector2 &v2) {
  return Vector2Subtract(v1, v2);
}

Vector2 operator*(const Vector2 &v, float scalar) {
  return Vector2Scale(v, scalar);
}

Vector2 operator/(const Vector2 &v, float scalar) {
  return Vector2Divide(v, {scalar, scalar});
}
Rectangle operator+(const Rectangle &r1, const Vector2 pos) {
  return Rectangle{r1.x + pos.x, r1.y + pos.y, r1.width, r1.height};
}
Rectangle rectFromCenter(Vector2 center,float width,float height){
	Vector2 pos=center-Vector2{width,height}/2;
	return Rectangle{pos.x,pos.y,width,height};
}

struct Circle {
  Vector2 center;
  float radius;
};
void DrawCircleCir(Circle cir, Color color) {
  DrawCircle(cir.center.x, cir.center.y, cir.radius, color);
}
bool CheckCollisionPointCircle(Vector2 point, Circle cir) {
  return CheckCollisionPointCircle(point, cir.center, cir.radius);
}
std::string vectorToString(const std::vector<std::string> &vec) {
  std::string result;
  for (size_t i = 0; i < vec.size(); ++i) {
    result += vec[i];
    if (i < vec.size() - 1) {
      result += "\n";
    }
  }
  return result;
}

int levenshteinDistance(const std::string &s1, const std::string &s2) {
  // Convert both strings to lowercase
  std::string lowerS1 = s1;
  std::string lowerS2 = s2;
  std::transform(lowerS1.begin(), lowerS1.end(), lowerS1.begin(), ::tolower);
  std::transform(lowerS2.begin(), lowerS2.end(), lowerS2.begin(), ::tolower);

  int len1 = lowerS1.length();
  int len2 = lowerS2.length();

  std::vector<std::vector<int>> matrix(len1 + 1, std::vector<int>(len2 + 1));

  for (int i = 0; i <= len1; ++i) {
    matrix[i][0] = i;
  }

  for (int j = 0; j <= len2; ++j) {
    matrix[0][j] = j;
  }

  for (int i = 1; i <= len1; ++i) {
    for (int j = 1; j <= len2; ++j) {
      int cost = (lowerS1[i - 1] == lowerS2[j - 1]) ? 0 : 1;
      matrix[i][j] = std::min({matrix[i - 1][j] + 1, matrix[i][j - 1] + 1,
                               matrix[i - 1][j - 1] + cost});
    }
  }

  return matrix[len1][len2];
}

// Function to perform fuzzy search in a vector of strings

// Function to perform fuzzy search in a vector of strings
std::vector<std::string> fuzzySearch(const std::string &query,
                                     const std::vector<std::string> &vec) {
  const int threshold = 2; // Adjust threshold for fuzzy search
  std::vector<std::string> ans;

  // Define a priority queue to store pairs of (distance, string)
  std::priority_queue<std::pair<int, std::string>> pq;

  // Iterate through each word in the vector
  for (const std::string &word : vec) {
    int distance = levenshteinDistance(query, word);
    if (distance <= threshold) {
      // Invert the distance before inserting into the priority queue
      pq.push(std::make_pair(-distance, word));
    }
  }

  // Extract strings from the priority queue in ascending order of distance
  while (!pq.empty()) {
    ans.push_back(pq.top().second);
    pq.pop();
  }

  return ans;
}
Vector2 rect_pos(const Rectangle &rect) { return Vector2{rect.x, rect.y}; }
void DrawRectangleWithLines(Rectangle rect, Color rect_color = WHITE,
                            Color line_color = BLACK, int width = 3) {
  DrawRectangleRec(rect, rect_color);
  DrawRectangleLinesEx(rect, width, line_color);
}
const u8 CHARS_MAX_SIZE = 128;
enum Error {
  OVERFLOW_ERROR,
  OUTOFBOUND_ERROR,
  OK,
};
class Chars {
public:
  Chars(const char *chars) {
    end_ptr = core + strlen(chars);
    if (end_ptr < core + CHARS_MAX_SIZE) {
      strcpy(core, chars);
    } else {
      throw std::overflow_error("string is greater than CHARS_MAX_SIZE");
    }
  }
  Chars() { Chars(""); }
  Chars(const Chars &other) {
    strcpy(core, other.core);
    end_ptr = core + other.length();
  }
  Chars(const std::string &&str) { Chars(c_str()); }
  char *c_str() { return core; }
  Error push_back(char c) {
    if (length() + 1 < CHARS_MAX_SIZE) {
      *end_ptr = c;
      end_ptr++;
      *end_ptr = '\0';
      return OK;
    }
    return OVERFLOW_ERROR;
  }
  char pop_back() {
    if (length() == 0) {
      return '\0';
    }
    end_ptr--;
    auto ans = *end_ptr;
    *end_ptr = '\0';
    return ans;
  }
  Error insert(char c, u8 position) {
    if (!(length() + 1 < CHARS_MAX_SIZE)) {
      return OVERFLOW_ERROR; // Overflow erroR
    }
    if (position > length())
      return OUTOFBOUND_ERROR; // Out of bound error.
    auto pointer = core + position;
    memmove(pointer + 1, pointer, end_ptr - pointer);
    *pointer = c;
    end_ptr++;
    *end_ptr = '\0';
    return OK;
  }
  Error erase(u8 position) {
    if (!(position < length()))
      return OUTOFBOUND_ERROR;
    auto pointer = core + position;
    auto pointer1 = pointer + 1;
    memmove(pointer, pointer1, end_ptr - pointer1);
    end_ptr--;
    *end_ptr = '\0';
    return OK;
  }
  u8 length() const { return end_ptr - core; }
  bool empty() const { return core == end_ptr; }

private:
  char core[CHARS_MAX_SIZE];
  char *end_ptr;
};
Vector2 position(Rectangle rect) { return Vector2{rect.x, rect.y}; }
Vector2 middle(Rectangle rect) {
  return position(rect) + Vector2{rect.width, rect.height} / 2;
}
void DrawRectangleGradientHRec(Rectangle rect, Color color1, Color color2) {
  DrawRectangleGradientH(rect.x, rect.y, rect.width, rect.height, color1,
                         color2);
}
void DrawText(std::string text, Vector2 position, int fontSize = 11,
              Color color = BLACK) {
  DrawText(text.c_str(), position.x, position.y, fontSize, color);
}
void DrawText(Chars text, Vector2 position, int fontSize = 11,
              Color color = BLACK) {
  DrawText(text.c_str(), position.x, position.y, fontSize, color);
}


#include <deque>
template <typename T> class BoundedQueue {
public:
  BoundedQueue(size_t max_size) : max_size(max_size) {}

  void push(T item) {
    if (queue.size() == max_size) {
      queue.pop_front();
    }
    queue.push_back(item);
  }
  auto size() { return queue.size(); }
  int pop() {
    if (queue.empty()) {
      return 1;
    }
    int front = queue.front();
    queue.pop_front();
    return 0;
  }
  auto begin() { return queue.begin(); }
  auto end() { return queue.end(); }
  auto at(int x) { return queue.at(x); }

private:
  std::deque<T> queue;
  size_t max_size;
};

// Example usage:
