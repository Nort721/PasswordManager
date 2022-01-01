#include "CryptoUtil.hpp"
#include "SHA256.hpp"

std::string XOR(std::string data, char key[])
{
    std::string output = data;

    for (int i = 0; unsigned(i) < data.size(); i++)
        output[i] = data[i] ^ key[i % (sizeof(key) / sizeof(char))];

    return output;
}

std::string GenerateSHA256(std::string input) {
    return sha256(input);
}

std::string HashAuthKey(std::string plainTextKey) {
    //const std::string static_salt_back = "otrijiuhwyuvfhdfgweoltpgrjhkoifvkdsfiu";
    //const std::string static_salt_front = "jbgviuhnb8ycjsiodsklpkiariuedhtycrxfret";
    //std::string hashedStr = plainTextKey;
    //for (int i = 0; i < 2; i++) {
    //    hashedStr = GenerateSHA256(static_salt_back + hashedStr + static_salt_front);
    //}
    return plainTextKey;
}

std::string GenerateVaultKey(std::string username, std::string password) {
    std::string vaultKey = username + "[<->]|[<->]" + password;
    //for (int i = 0; i < 5; i++) {
    //    vaultKey = GenerateSHA256(vaultKey);
    //}
    return vaultKey;
}