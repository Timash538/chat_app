#include "server/PasswordHash.h"
#include <sodium.h>
#include <string>
#include <stdexcept>

// Вызывать ОДИН РАЗ в main()
void initCrypto() {
    if (sodium_init() < 0) {
        throw std::runtime_error("Failed to init libsodium");
    }
}
std::string hashPassword(const std::string& password) {
    char hash[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(
        hash,
        password.c_str(),
        password.size(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
    ) != 0) {
        throw std::runtime_error("Password hash failed");
    }
    return std::string(hash);
}

bool verifyPassword(const std::string& password, const std::string& hash) {
    return crypto_pwhash_str_verify(
        hash.c_str(),
        password.c_str(),
        password.size()
    ) == 0;
}