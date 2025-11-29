#pragma once

#include <string>

/**
 * @brief Bcrypt-based password hashing utilities.
 *
 * @example
 * std::string hash = PasswordHasher::hashPassword("secret");
 * bool ok = PasswordHasher::verifyPassword("secret", hash);
 */
class PasswordHasher
{
public:
    /**
     * @brief Hashes a password with bcrypt.
     * @param password Plain text password.
     * @param workFactor Cost factor for bcrypt (default 12).
     * @return Bcrypt hash string.
     * @throws std::runtime_error When salt or hash generation fails.
     */
    static std::string hashPassword(const std::string& password, int workFactor = 12);

    /**
     * @brief Verifies a password against a bcrypt hash.
     * @param password Plain text password.
     * @param hashed Stored bcrypt hash.
     * @return true when the password matches; false otherwise.
     */
    static bool verifyPassword(const std::string& password, const std::string& hashed);
};
