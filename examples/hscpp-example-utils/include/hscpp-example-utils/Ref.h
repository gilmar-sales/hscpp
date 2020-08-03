#pragma once

#include <stdint.h>

#include "hscpp-example-utils/GetMemory.h"

// Maps an id to memory.
template <typename T>
struct Ref
{
    size_t id = 0;

    T* operator->()
    {
        return reinterpret_cast<T*>(GetMemory(id));
    }

    T* operator*()
    {
        return reinterpret_cast<T*>(GetMemory(id));
    }

    template <typename U>
    typename std::enable_if<std::is_base_of<U, T>::value, Ref<U>>::type
    As()
    {
        Ref<U> base;
        base.id = id;

        return base;
    }
};