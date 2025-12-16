#pragma once
#include <string>

void initCrypto();

std::string hashPassword(const std::string& password);

bool verifyPassword(const std::string& password, const std::string& hash);