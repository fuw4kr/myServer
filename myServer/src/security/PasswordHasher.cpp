#include "PasswordHasher.h"

#include <stdexcept>

extern "C" {
#include "bcrypt/bcrypt_wrapper.h"
}

std::string PasswordHasher::hashPassword(const std::string& password, int workFactor)
{
    char salt[BCRYPT_HASHSIZE];
    if (bcrypt_gensalt(workFactor, salt) != 0)
        throw std::runtime_error("Failed to generate bcrypt salt");

    char hash[BCRYPT_HASHSIZE];
    if (bcrypt_hashpw(password.c_str(), salt, hash) != 0)
        throw std::runtime_error("Failed to hash password with bcrypt");

    return std::string(hash);
}

bool PasswordHasher::verifyPassword(const std::string& password, const std::string& hashed)
{
    if (hashed.empty())
        return false;

    return bcrypt_checkpw(password.c_str(), hashed.c_str()) == 0;
}
