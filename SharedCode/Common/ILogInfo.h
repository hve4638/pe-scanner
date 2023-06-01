#pragma once
#include "Common/typedef.h"

namespace PEScan {
    struct ILogInfo {
        virtual ~ILogInfo() {}
        virtual tstring message() = 0;
    };
}