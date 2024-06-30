
#include <iostream>
#include <typeinfo>
#include <stdexcept>

class Any {
private:
    struct Base {
        virtual ~Base() {}
        virtual std::unique_ptr<Base> clone() const = 0;
    };

    template <typename T>
    struct Derived : Base {
        T value;

        template <typename U>
        Derived(U&& v) : value(std::forward<U>(v)) {}

        std::unique_ptr<Base> clone() const override {
            return std::make_unique<Derived<T>>(value);
        }
    };

    std::unique_ptr<Base> ptr;

public:
    template <typename T>
    Any(T&& value) : ptr(std::make_unique<Derived<T>>(std::forward<T>(value))) {}

    Any(const Any& other) {
        if (other.ptr) {
            ptr = other.ptr->clone();
        }
    }

    Any& operator=(const Any& other) {
        if (this != &other) {
            ptr = nullptr;
            if (other.ptr) {
                ptr = other.ptr->clone();
            }
        }
        return *this;
    }

    template <typename T>
    T& cast() {
        if (!ptr) {
            throw std::runtime_error("Bad cast: empty Any");
        }
        auto derived = dynamic_cast<Derived<T>*>(ptr.get());
        if (!derived) {
            throw std::bad_cast();
        }
        return derived->value;
    }

    bool empty() const {
        return !ptr;
    }
};

int main() {
    Any a = 5;
    std::cout << a.cast<int>() << std::endl;  // Output: 5

    a = std::string("Hello");
    std::cout << a.cast<std::string>() << std::endl;  // Output: Hello

    try {
        a.cast<int>();  // Throws std::bad_cast
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
