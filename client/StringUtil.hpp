#ifndef StringUtil_HPP
#define StringUtil_HPP

#include <string>
#include <vector>

std::vector<std::string> Split(std::string args, char seperator);

void FormatVault(std::string vaultStr, std::vector<std::string>* vault);

std::string DeformatVault(std::vector<std::string>* vault);

#endif