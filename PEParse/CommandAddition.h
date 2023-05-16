#pragma once
#include "typedef.h"
#include <map>

struct CommandAddition {
    vector<tstring> group;
    shared_ptr<map<tstring, tstring>> options;
};