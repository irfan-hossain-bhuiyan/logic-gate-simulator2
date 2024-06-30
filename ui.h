
#ifndef TOUCHABLE_COLLECTION_H
#define TOUCHABLE_COLLECTION_H

#include <unordered_set>
#include <vector>
#include <algorithm>
#include <tuple>
#include "raylib.h"
#include "./basic_template.cpp"
class TouchableCollection;

class Touchable {
public:
    virtual bool CollisionPoint(Vector2 pos) = 0;
    Touchable(TouchableCollection* tc);
    virtual ~Touchable();

    bool is_touching(); 
    bool is_clicked();
    bool is_clickedone();
    void add_to(TouchableCollection* tc);

private:
    TouchableCollection *child_to = nullptr;
    friend class TouchableCollection;
};

class TouchableCollection {
private:
    std::unordered_set<Touchable *> touchables;
    Touchable *touching = nullptr;
    Touchable *last_click = nullptr;

    void push_back(Touchable *touchable);
    void erase(Touchable *touchable);

public:
    void click_update();
    friend class Touchable;
};

#endif // TOUCHABLE_COLLECTION_H
