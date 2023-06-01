#pragma once
#ifdef _PESCAN_LIB
#define PESCAN_API __declspec(dllexport)
#else
#define PESCAN_API 
#endif