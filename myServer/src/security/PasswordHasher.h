#pragma once

#include <string>

class PasswordHasher
{
public:
    static std::string hashPassword(const std::string& password, int workFactor = 12);
    static bool verifyPassword(const std::string& password, const std::string& hashed);
};

