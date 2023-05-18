#pragma once
#include "typedef.h"

class ReserveCloseRegKey {
    HKEY m_key = NULL;

public:
    ReserveCloseRegKey();
    ReserveCloseRegKey(HKEY);
    ~ReserveCloseRegKey();
    void reset(HKEY);
};