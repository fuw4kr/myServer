/**
 * @brief Bcrypt hashing/verification helpers.
 */
#include "PasswordHasher.h"

#include <stdexcept>

extern "C" {
#include "bcrypt/bcrypt_wrapper.h"
}

/**
 * @brief Generates a bcrypt hash for the given password.
 * @param password Plain text password.
 * @param workFactor Cost factor controlling bcrypt rounds.
 * @return Bcrypt hash string.
 * @throws std::runtime_error When salt or hash generation fails.
 */
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

/**
 * @brief Verifies a password against a bcrypt hash.
 * @param password Plain text password.
 * @param hashed Stored bcrypt hash string.
 * @return true when the password matches; false otherwise.
 */
bool PasswordHasher::verifyPassword(const std::string& password, const std::string& hashed)
{
    if (hashed.empty())
        return false;

    return bcrypt_checkpw(password.c_str(), hashed.c_str()) == 0;
}
