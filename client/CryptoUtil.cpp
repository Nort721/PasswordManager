#include "CryptoUtil.hpp"
#include "SHA256.hpp"

std::string XOR(std::string data, char key[])
{
    std::string xorstring = data;
    for (int i = 0; i < xorstring.size(); i++)
    {
        xorstring[i] = data[i] ^ key[i % (sizeof(key) / sizeof(char))];
    }
    return xorstring;
}

std::string GenerateSHA256(std::string input) {
    SHA256 sha;
    sha.update(input);
    uint8_t* digest = sha.digest();
    std::string hashed = SHA256::toString(digest);
    delete[] digest;
    return hashed;
}

std::string HashAuthKey(std::string plainTextKey) {
    const std::string static_salt_back = "otrijiuhwyuvfhdfgweoltpgrjhkoifvkdsfiu";
    const std::string static_salt_front = "jbgviuhnb8ycjsiodsklpkiariuedhtycrxfret";
    std::string hashedStr = plainTextKey;
    for (int i = 0; i < 2; i++) {
        hashedStr = GenerateSHA256(static_salt_back + hashedStr + static_salt_front);
    }
    return hashedStr;
}

std::string GenerateVaultKey(std::string username, std::string password) {
    std::string vaultKey = username + "[<->]|[<->]" + password;
    for (int i = 0; i < 5; i++) {
        vaultKey = GenerateSHA256(vaultKey);
    }
    return vaultKey;
}