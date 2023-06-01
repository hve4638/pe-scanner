#pragma once
#include <functional>

class ReserveCall {
private:
    std::function<void()> m_callback = NULL;

public:
    ReserveCall(std::function<void()> callback = NULL);
    ~ReserveCall();
    
    void reset(std::function<void()>);
};