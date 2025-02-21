#pragma once

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "ConstString.h"

struct FUserInfo
{
    std::string Name;
    std::string PhoneNumber;
    int Age;
    bool bIsMarriage;
};

