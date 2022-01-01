#ifndef NetworkUtil_HPP
#define NetworkUtil_HPP

#include <string>

std::string SendSocketMessage(std::string msg);

std::string SendVaultRequest(std::string authkey);

std::string SendVaultUpdateRequest(std::string authkey, std::string newdata, std::string vaultKey);

std::string SendCreateVaultRequest(std::string authkey, std::string data, std::string vaultKey);

#endif