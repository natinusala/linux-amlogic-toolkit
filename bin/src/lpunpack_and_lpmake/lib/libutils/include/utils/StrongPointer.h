/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_STRONG_POINTER_H
#define ANDROID_STRONG_POINTER_H

#include <functional>
#include <type_traits>  // for common_type.

// ---------------------------------------------------------------------------
namespace android {

template<typename T> class wp;

// ---------------------------------------------------------------------------

template<typename T>
class sp {
public:
    inline sp() : m_ptr(nullptr) { }

    sp(T* other);  // NOLINT(implicit)
    sp(const sp<T>& other);
    sp(sp<T>&& other) noexcept;
    template<typename U> sp(U* other);  // NOLINT(implicit)
    template<typename U> sp(const sp<U>& other);  // NOLINT(implicit)
    template<typename U> sp(sp<U>&& other);  // NOLINT(implicit)

    ~sp();

    // Assignment

    sp& operator = (T* other);
    sp& operator = (const sp<T>& other);
    sp& operator=(sp<T>&& other) noexcept;

    template<typename U> sp& operator = (const sp<U>& other);
    template<typename U> sp& operator = (sp<U>&& other);
    template<typename U> sp& operator = (U* other);

    //! Special optimization for use by ProcessState (and nobody else).
    void force_set(T* other);

    // Reset

    void clear();

    // Accessors

    inline T&       operator* () const     { return *m_ptr; }
    inline T*       operator-> () const    { return m_ptr;  }
    inline T*       get() const            { return m_ptr; }
    inline explicit operator bool () const { return m_ptr != nullptr; }

    // Punt these to the wp<> implementation.
    template<typename U>
    inline bool operator == (const wp<U>& o) const {
        return o == *this;
    }

    template<typename U>
    inline bool operator != (const wp<U>& o) const {
        return o != *this;
    }

private:
    template<typename Y> friend class sp;
    template<typename Y> friend class wp;
    void set_pointer(T* ptr);
    static inline void check_not_on_stack(const void* ptr);
    T* m_ptr;
};

#define COMPARE_STRONG(_op_)                                           \
    template <typename T, typename U>                                  \
    static inline bool operator _op_(const sp<T>& t, const sp<U>& u) { \
        return t.get() _op_ u.get();                                   \
    }                                                                  \
    template <typename T, typename U>                                  \
    static inline bool operator _op_(const T* t, const sp<U>& u) {     \
        return t _op_ u.get();                                         \
    }                                                                  \
    template <typename T, typename U>                                  \
    static inline bool operator _op_(const sp<T>& t, const U* u) {     \
        return t.get() _op_ u;                                         \
    }                                                                  \
    template <typename T>                                              \
    static inline bool operator _op_(const sp<T>& t, std::nullptr_t) { \
        return t.get() _op_ nullptr;                                   \
    }                                                                  \
    template <typename T>                                              \
    static inline bool operator _op_(std::nullptr_t, const sp<T>& t) { \
        return nullptr _op_ t.get();                                   \
    }

template <template <typename C> class comparator, typename T, typename U>
static inline bool _sp_compare_(T* a, U* b) {
    return comparator<typename std::common_type<T*, U*>::type>()(a, b);
}

#define COMPARE_STRONG_FUNCTIONAL(_op_, _compare_)                     \
    template <typename T, typename U>                                  \
    static inline bool operator _op_(const sp<T>& t, const sp<U>& u) { \
        return _sp_compare_<_compare_>(t.get(), u.get());              \
    }                                                                  \
    template <typename T, typename U>                                  \
    static inline bool operator _op_(const T* t, const sp<U>& u) {     \
        return _sp_compare_<_compare_>(t, u.get());                    \
    }                                                                  \
    template <typename T, typename U>                                  \
    static inline bool operator _op_(const sp<T>& t, const U* u) {     \
        return _sp_compare_<_compare_>(t.get(), u);                    \
    }                                                                  \
    template <typename T>                                              \
    static inline bool operator _op_(const sp<T>& t, std::nullptr_t) { \
        return _sp_compare_<_compare_>(t.get(), nullptr);              \
    }                                                                  \
    template <typename T>                                              \
    static inline bool operator _op_(std::nullptr_t, const sp<T>& t) { \
        return _sp_compare_<_compare_>(nullptr, t.get());              \
    }

COMPARE_STRONG(==)
COMPARE_STRONG(!=)
COMPARE_STRONG_FUNCTIONAL(>, std::greater)
COMPARE_STRONG_FUNCTIONAL(<, std::less)
COMPARE_STRONG_FUNCTIONAL(<=, std::less_equal)
COMPARE_STRONG_FUNCTIONAL(>=, std::greater_equal)

#undef COMPARE_STRONG
#undef COMPARE_STRONG_FUNCTIONAL

// For code size reasons, we do not want these inlined or templated.
void sp_report_race();
void sp_report_stack_pointer();

// ---------------------------------------------------------------------------
// No user serviceable parts below here.

// Check whether address is definitely on the calling stack.  We actually check whether it is on
// the same 4K page as the frame pointer.
//
// Assumptions:
// - Pages are never smaller than 4K (MIN_PAGE_SIZE)
// - Malloced memory never shares a page with a stack.
//
// It does not appear safe to broaden this check to include adjacent pages; apparently this code
// is used in environments where there may not be a guard page below (at higher addresses than)
// the bottom of the stack.
//
// TODO: Consider adding make_sp<T>() to allocate an object and wrap the resulting pointer safely
// without checking overhead.
template <typename T>
void sp<T>::check_not_on_stack(const void* ptr) {
    static constexpr int MIN_PAGE_SIZE = 0x1000;  // 4K. Safer than including sys/user.h.
    static constexpr uintptr_t MIN_PAGE_MASK = ~static_cast<uintptr_t>(MIN_PAGE_SIZE - 1);
    uintptr_t my_frame_address =
            reinterpret_cast<uintptr_t>(__builtin_frame_address(0 /* this frame */));
    if (((reinterpret_cast<uintptr_t>(ptr) ^ my_frame_address) & MIN_PAGE_MASK) == 0) {
        sp_report_stack_pointer();
    }
}

template<typename T>
sp<T>::sp(T* other)
        : m_ptr(other) {
    if (other) {
        check_not_on_stack(other);
        other->incStrong(this);
    }
}

template<typename T>
sp<T>::sp(const sp<T>& other)
        : m_ptr(other.m_ptr) {
    if (m_ptr)
        m_ptr->incStrong(this);
}

template <typename T>
sp<T>::sp(sp<T>&& other) noexcept : m_ptr(other.m_ptr) {
    other.m_ptr = nullptr;
}

template<typename T> template<typename U>
sp<T>::sp(U* other)
        : m_ptr(other) {
    if (other) {
        check_not_on_stack(other);
        (static_cast<T*>(other))->incStrong(this);
    }
}

template<typename T> template<typename U>
sp<T>::sp(const sp<U>& other)
        : m_ptr(other.m_ptr) {
    if (m_ptr)
        m_ptr->incStrong(this);
}

template<typename T> template<typename U>
sp<T>::sp(sp<U>&& other)
        : m_ptr(other.m_ptr) {
    other.m_ptr = nullptr;
}

template<typename T>
sp<T>::~sp() {
    if (m_ptr)
        m_ptr->decStrong(this);
}

template<typename T>
sp<T>& sp<T>::operator =(const sp<T>& other) {
    // Force m_ptr to be read twice, to heuristically check for data races.
    T* oldPtr(*const_cast<T* volatile*>(&m_ptr));
    T* otherPtr(other.m_ptr);
    if (otherPtr) otherPtr->incStrong(this);
    if (oldPtr) oldPtr->decStrong(this);
    if (oldPtr != *const_cast<T* volatile*>(&m_ptr)) sp_report_race();
    m_ptr = otherPtr;
    return *this;
}

template <typename T>
sp<T>& sp<T>::operator=(sp<T>&& other) noexcept {
    T* oldPtr(*const_cast<T* volatile*>(&m_ptr));
    if (oldPtr) oldPtr->decStrong(this);
    if (oldPtr != *const_cast<T* volatile*>(&m_ptr)) sp_report_race();
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
}

template<typename T>
sp<T>& sp<T>::operator =(T* other) {
    T* oldPtr(*const_cast<T* volatile*>(&m_ptr));
    if (other) {
        check_not_on_stack(other);
        other->incStrong(this);
    }
    if (oldPtr) oldPtr->decStrong(this);
    if (oldPtr != *const_cast<T* volatile*>(&m_ptr)) sp_report_race();
    m_ptr = other;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator =(const sp<U>& other) {
    T* oldPtr(*const_cast<T* volatile*>(&m_ptr));
    T* otherPtr(other.m_ptr);
    if (otherPtr) otherPtr->incStrong(this);
    if (oldPtr) oldPtr->decStrong(this);
    if (oldPtr != *const_cast<T* volatile*>(&m_ptr)) sp_report_race();
    m_ptr = otherPtr;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator =(sp<U>&& other) {
    T* oldPtr(*const_cast<T* volatile*>(&m_ptr));
    if (m_ptr) m_ptr->decStrong(this);
    if (oldPtr != *const_cast<T* volatile*>(&m_ptr)) sp_report_race();
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator =(U* other) {
    T* oldPtr(*const_cast<T* volatile*>(&m_ptr));
    if (other) (static_cast<T*>(other))->incStrong(this);
    if (oldPtr) oldPtr->decStrong(this);
    if (oldPtr != *const_cast<T* volatile*>(&m_ptr)) sp_report_race();
    m_ptr = other;
    return *this;
}

template<typename T>
void sp<T>::force_set(T* other) {
    other->forceIncStrong(this);
    m_ptr = other;
}

template<typename T>
void sp<T>::clear() {
    T* oldPtr(*const_cast<T* volatile*>(&m_ptr));
    if (oldPtr) {
        oldPtr->decStrong(this);
        if (oldPtr != *const_cast<T* volatile*>(&m_ptr)) sp_report_race();
        m_ptr = nullptr;
    }
}

template<typename T>
void sp<T>::set_pointer(T* ptr) {
    m_ptr = ptr;
}

}  // namespace android

// ---------------------------------------------------------------------------

#endif // ANDROID_STRONG_POINTER_H
