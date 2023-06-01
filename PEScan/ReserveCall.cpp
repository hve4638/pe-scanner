#include "ReserveCall.h"

ReserveCall::ReserveCall(std::function<void()> callback) {
    reset(callback);
}

void ReserveCall::reset(std::function<void()> callback) {
    m_callback = callback;
}

ReserveCall::~ReserveCall() {
    if (m_callback != NULL) {
        m_callback();
    }
}