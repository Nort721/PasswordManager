#ifndef CryptoUtil_HPP
#define CryptoUtil_HPP

#include <string>

std::string XOR(std::string data, char key[]);

std::string GenerateSHA256(std::string input);

std::string HashAuthKey(std::string plainTextKey);

std::string GenerateVaultKey(std::string username, std::string password);

#endif

