#pragma once
#include <string>

//Для безопасности. Но надо реализовывать безопасную передачу данных по TLS

void initCrypto();

std::string hashPassword(const std::string& password);

bool verifyPassword(const std::string& password, const std::string& hash);