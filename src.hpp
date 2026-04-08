#ifndef SRC_HPP
#define SRC_HPP

#include <stdexcept>
#include <initializer_list>
#include <typeinfo>
#include <utility>

namespace sjtu {

class any_ptr {
private:
    struct ControlBlockBase {
        int ref_count;
        ControlBlockBase() : ref_count(1) {}
        virtual ~ControlBlockBase() = default;
        virtual void* get_ptr() = 0;
        virtual const std::type_info& type() const = 0;
    };

    template <typename T>
    struct ControlBlock : ControlBlockBase {
        T* ptr;
        ControlBlock(T* p) : ptr(p) {}
        ~ControlBlock() override {
            delete ptr;
        }
        void* get_ptr() override {
            return ptr;
        }
        const std::type_info& type() const override {
            return typeid(T);
        }
    };

    ControlBlockBase* cb;

public:
    any_ptr() : cb(nullptr) {}

    any_ptr(std::nullptr_t) : cb(nullptr) {}

    any_ptr(const any_ptr &other) : cb(other.cb) {
        if (cb) {
            cb->ref_count++;
        }
    }

    any_ptr(any_ptr &&other) noexcept : cb(other.cb) {
        other.cb = nullptr;
    }

    template <class T> 
    any_ptr(T *ptr) {
        if (ptr) {
            cb = new ControlBlock<T>(ptr);
        } else {
            cb = nullptr;
        }
    }

    ~any_ptr() {
        if (cb) {
            cb->ref_count--;
            if (cb->ref_count == 0) {
                delete cb;
            }
        }
    }

    any_ptr &operator=(const any_ptr &other) {
        if (this != &other) {
            if (cb) {
                cb->ref_count--;
                if (cb->ref_count == 0) {
                    delete cb;
                }
            }
            cb = other.cb;
            if (cb) {
                cb->ref_count++;
            }
        }
        return *this;
    }

    any_ptr &operator=(any_ptr &&other) noexcept {
        if (this != &other) {
            if (cb) {
                cb->ref_count--;
                if (cb->ref_count == 0) {
                    delete cb;
                }
            }
            cb = other.cb;
            other.cb = nullptr;
        }
        return *this;
    }

    any_ptr &operator=(std::nullptr_t) {
        if (cb) {
            cb->ref_count--;
            if (cb->ref_count == 0) {
                delete cb;
            }
        }
        cb = nullptr;
        return *this;
    }

    template <class T> 
    any_ptr &operator=(T *ptr) {
        if (cb) {
            cb->ref_count--;
            if (cb->ref_count == 0) {
                delete cb;
            }
        }
        if (ptr) {
            cb = new ControlBlock<T>(ptr);
        } else {
            cb = nullptr;
        }
        return *this;
    }

    template <class T> 
    T &unwrap() {
        if (!cb) throw std::bad_cast();
        if (cb->type() != typeid(T)) throw std::bad_cast();
        return *static_cast<T*>(cb->get_ptr());
    }

    template <class T> 
    const T &unwrap() const {
        if (!cb) throw std::bad_cast();
        if (cb->type() != typeid(T)) throw std::bad_cast();
        return *static_cast<T*>(cb->get_ptr());
    }
};

template <class T> 
any_ptr make_any_ptr(const T &t) { 
    return any_ptr(new T(t)); 
}

template <class T, class... Args>
any_ptr make_any_ptr(Args&&... args) {
    return any_ptr(new T{std::forward<Args>(args)...});
}

}  // namespace sjtu

#endif
