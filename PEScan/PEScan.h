#pragma once
#include "pch.h"
#include "Common/PEScanAPI.h"
#include "Common/ILogger.h"

extern "C" PESCAN_API LogUtils::ILogger * CreateLogger();