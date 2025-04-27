#include "basic_template.hpp"
#include "globals.hpp"
#include "ui.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <queue>
#include <raylib.h>
#include <raymath.h>
#include <stdexcept>
#include <string>
Chars::Chars(const char *chars) {
  // Endptr is the ptr to the \0.
  usize str_len = strlen(chars);
  end_ptr = core.begin() + str_len;
  if (end_ptr < core.begin() + CHARS_MAX_SIZE) {
    std::copy(chars, chars + str_len, core.begin());
  } else {
    throw std::overflow_error(
        "string is greater than CHARS_MAX_SIZE. String is of length" +
        std::to_string(str_len));
  }
  *end_ptr = '\0';
}
Chars::Chars(const Chars &other) noexcept {
  std::copy(other.begin(), other.end(), this->begin());
  end_ptr = core.begin() + other.length();
  *end_ptr = '\0';
}

char *Chars::c_str() { return core.data(); }
const char *Chars::c_str() const { return core.data(); }
char Chars::at(int a) {
  if (0 <= a && a < length()) {
    return core[a];
  }
  throw std::out_of_range("Out of range.Size is" + std::to_string(length()) +
                          "Index is " + std::to_string(a));
}
Chars::operator bool() const { return !empty(); }
char Chars::operator[](int a) {
  if (0 <= a && a < length()) {
    return core[a];
  }
  return '\0';
}
ErrorS Chars::push_back(char c) {
  if (length() + 1 < CHARS_MAX_SIZE) {
    *end_ptr = c;
    end_ptr++;
    *end_ptr = '\0';
    return ErrorS::OK;
  }
  return ErrorS::OVERFLOW_ERROR;
}
char Chars::pop_back() {
  if (length() == 0) {
    return '\0';
  }
  end_ptr--;
  auto ans = *end_ptr;
  *end_ptr = '\0';
  return ans;
}
ErrorS Chars::insert(char c, u8 position) {
  if (!(length() + 1 < CHARS_MAX_SIZE)) {
    return ErrorS::OVERFLOW_ERROR; // Overflow erroR
  }
  if (position > length())
    return ErrorS::OUTOFBOUND_ERROR; // Out of bound error.
  auto pointer = core.begin() + position;
  memmove(pointer + 1, pointer, end_ptr - pointer);
  *pointer = c;
  end_ptr++;
  *end_ptr = '\0';
  return ErrorS::OK;
}
ErrorS Chars::erase(u8 position) {
  if (!(position < length()))
    return ErrorS::OUTOFBOUND_ERROR;
  auto pointer = core.begin() + position;
  auto pointer1 = pointer + 1;
  memmove(pointer, pointer1, end_ptr - pointer1);
  end_ptr--;
  *end_ptr = '\0';
  return ErrorS::OK;
}
u8 Chars::length() const { return this->end_ptr - this->begin(); }
bool Chars::empty() const { return this->begin() == this->end_ptr; }
Chars::Iterator Chars::begin() { return core.begin(); }
Chars::Iterator Chars::end() { return end_ptr; }

Chars::CIterator Chars::begin() const { return core.cbegin(); }

Chars::CIterator Chars::end() const { return end_ptr; }

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
Rectangle rectFromCenter(Vector2 center, float width, float height) {
  Vector2 pos = center - Vector2{width, height} / 2;
  return Rectangle{pos.x, pos.y, width, height};
}
Vector2 RectSize::toVec2() { return Vector2{width, height}; }
Rectangle rectFromCenter(Vector2 center, RectSize rectSize) {
  Vector2 pos = center - rectSize.toVec2() / 2;
  return rectFromPos(pos, rectSize);
}
Rectangle rectFromPos(Vector2 position, float width, float height) {
  return Rectangle{position.x, position.y, width, height};
}
Rectangle rectFromPos(Vector2 position, RectSize rectSize) {
  return Rectangle{position.x, position.y, rectSize.width, rectSize.height};
}
void DrawCircleCir(Circle cir, Color color) {

  DrawCircleV(cir.center, cir.radius, color);
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

  usize len1 = lowerS1.length();
  usize len2 = lowerS2.length();

  std::vector<std::vector<int>> matrix(len1 + 1, std::vector<int>(len2 + 1));

  for (usize i = 0; i <= len1; ++i) {
    matrix[i][0] = int(i);
  }

  for (usize j = 0; j <= len2; ++j) {
    matrix[0][j] = int(j);
  }

  for (usize i = 1; i <= len1; ++i) {
    for (usize j = 1; j <= len2; ++j) {
      int cost = (lowerS1[i - 1] == lowerS2[j - 1]) ? 0 : 1;
      matrix[i][j] = std::min({matrix[i - 1][j] + 1, matrix[i][j - 1] + 1,
                               matrix[i - 1][j - 1] + cost});
    }
  }

  return matrix[len1][len2];
}
int levenshteinDistance(const Chars &s1, const Chars &s2) {
  // Convert both strings to lowercase
  Chars lowerS1 = s1;
  Chars lowerS2 = s2;
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
    int adjustedDistance = distance +query.size()-word.size();
    if (adjustedDistance <= threshold) {
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
std::vector<Chars> fuzzySearch(const Chars &query,
                               const std::vector<Chars> &vec) {
  if (query.empty()) {
    return vec;
  }
  const int threshold = 2; // Adjust threshold for fuzzy search
  std::vector<Chars> ans;

  // Define a priority queue to store pairs of (distance, string)
  std::priority_queue<std::pair<int, Chars>> pq;

  // Iterate through each word in the vector
  for (const Chars &word : vec) {
    int distance = levenshteinDistance(query, word);
    int adjustedDistance = distance +query.length()-word.length();
    if (adjustedDistance <= threshold) {
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

Vector2 rectPos(const Rectangle &rect) { return Vector2{rect.x, rect.y}; }
void drawRectangleWithLines(Rectangle rect, Color rect_color, Color line_color,
                            float width) {
  DrawRectangleRec(rect, rect_color);
  DrawRectangleLinesEx(rect, width, line_color);
}
Vector2 rectCenter(Rectangle rect) {
  return rectPos(rect) + Vector2{rect.width, rect.height} / 2;
}
void DrawRectangleGradientHRec(Rectangle rect, Color color1, Color color2) {
  DrawRectangleGradientEx(rect, color1,
                         color2,color2,color1);
}
void drawText(const std::string &text, Vector2 position, float fontSize,
              Color color) {
  using namespace Resource::Fonts;
  DrawTextEx(DEFAULT_FONT, text.c_str(), position, fontSize, 0, color);
}
void drawText(const Chars &text, Vector2 position, float fontSize,
              Color color) {
  using namespace Resource::Fonts;
  DrawTextEx(DEFAULT_FONT, text.c_str(), position, fontSize, 0, color);
}
// Example usage:
void DrawCircleLinesCir(Circle cir, Color color, float width) {
  DrawRing(cir.center, cir.radius, cir.radius + width, 0.0, 360.0, 0, color);
}
auto Chars::operator<=>(const Chars &ch) const {
  return std::lexicographical_compare(this->begin(), this->end(), ch.begin(),
                                      ch.end());
}
bool Chars::operator==(const Chars &ch) const {
  return std::equal(this->begin(), this->end(), ch.begin());
}
Chars &Chars::operator=(const Chars &ch) noexcept {
  std::copy(ch.begin(), ch.end(), this->begin());
  end_ptr = this->begin() + ch.length();
  *end_ptr = '\0';
  return *this;
}
std::ostream &operator<<(std::ostream &os, const Chars &ch) {
  os << (ch.c_str());
  return os;
}
//RectSize measureText(const Chars &text, float size) {
//  using namespace Resource::Fonts;
//  return MeasureTextEx(LUMITIVE_FONT, text.c_str(), size, 0);
//}
Vector2 textPosition(Rectangle rect, const Chars &text, TextPositionS textPos,
                     float fontSize, float border) {
  RectSize textSize = measureText(text, fontSize);
  float textPosy = rectCenter(rect).y - (textSize.height / 2);
  switch (textPos) {
  case TextPositionS::left: {
    float textPosX = rect.x + border;
    return Vector2{textPosX, textPosy};
  } break;
  case TextPositionS::right: {
    float textPosX = rect.x + rect.width - textSize.width - border;
    return Vector2{textPosX, textPosy};
  } break;
  case TextPositionS::center: {
    auto textRect = rectFromCenter(rectCenter(rect), textSize);
    return rectPos(textRect);
  } break;
  default:
    throw std::runtime_error("All edge case not completed.");
  }
}
Vector2 textPosition(Rectangle rect, const Chars &text, float size,
                     float border) {
  return textPosition(rect, text, TextPositionS::left, size, border);
}
// Templates declaration
template class BoundedQueue<Chars>;
template class BoundedQueue<std::string>;
RectSize measureText(const Chars& text,float fontSize,float spacing){
	return MeasureTextEx(Resource::Fonts::DEFAULT_FONT,text.c_str(),fontSize,spacing);
}
std::ostream& operator<<(std::ostream& os,const Vector2& vec2){
	os<<"{ x:"<<vec2.x<<"y:"<<vec2.y<<"}";
	return os;
}
