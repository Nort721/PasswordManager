#include "StringUtil.hpp"

std::vector<std::string> Split(std::string args, char seperator) {
    std::vector<std::string> vec;
    std::string word;

    args += " ";

    for (size_t i = 0; i < args.length(); i++) {
        if (args[i] == seperator) {
            vec.push_back(word);
            word = "";
        }
        else
        {
            word += args[i];
        }
    }

    return vec;
}

void FormatVault(std::string vaultStr, std::vector<std::string> *vault) {
    // first decrypt vault

    std::string word;

    vaultStr += "/";

    for (size_t i = 0; i < vaultStr.length(); i++) {
        if (vaultStr[i] == '/') {
            vault->push_back(word);
            word = "";
        }
        else
        {
            word += vaultStr[i];
        }
    }
}

std::string DeformatVault(std::vector<std::string> *vault) {
    std::string vault_string = "";

    for (size_t i = 0; i < vault->size(); i++) {
        if (i != 0)
            vault_string += "/";
        vault_string += (*vault)[i];
    }

    return vault_string;
}