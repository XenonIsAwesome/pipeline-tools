#pragma once

#include <iostream>
class MockDeleter {
public:
    static bool deleted;

    template<typename T>
    void operator()(T* ptr) {
        MockDeleter::deleted = true;
        delete ptr;
    }
};
