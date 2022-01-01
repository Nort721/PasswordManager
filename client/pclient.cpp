#include "iostream"
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <winsock2.h>
#include "CryptoUtil.hpp"
#include "NetworkUtil.hpp"

extern bool running = true;
extern const std::string system_prefix = "system >> ";
extern std::string authKeyHash = "";

extern std::string username = "", password = "";

static std::vector<std::string> vault;

//extern std::shared_ptr<std::vector<std::string>> vaultPtr = std::shared_ptr<std::vector<std::string>>();

class command {
public:
    command() {}
    virtual void onCommand(std::vector<std::string> args) {}
};

class helpCommand : public command {
public:
    helpCommand() {}
    void onCommand(std::vector<std::string> args) override {
        std::cout << "====================\n - exit\n - help\n - printVault\n - createVault\n - updateVault\n - addData\n====================" << "\n";
    }
};

class exitCommand : public command {
public:
    exitCommand() {}
    void onCommand(std::vector<std::string> args) override {
        std::cout << system_prefix + "exiting . . ." << "\n";
        running = false;
    }
};

class createVaultCommand : public command {
public:
    createVaultCommand() {}
    void onCommand(std::vector<std::string> args) override {
        if (args.size() > 1) {
            std::cout << system_prefix + "incorrect args" << "\n";
            return;
        }
        std::string response = SendCreateVaultRequest(authKeyHash, "", GenerateVaultKey(username, password));
        std::cout << response << "\n";
    }
};

std::string deformatVault() {
    std::string vault_string = "";

    for (size_t i = 0; i < vault.size(); i++) {
        if (i != 0)
            vault_string += "/";
        vault_string += vault[i];
    }

    return vault_string;
}

class updateVaultCommand : public command {
public:
    updateVaultCommand() {}
    void onCommand(std::vector<std::string> args) override {
        if (args.size() > 1) {
            std::cout << system_prefix + "incorrect args" << "\n";
            return;
        }
        std::string response = SendVaultUpdateRequest(authKeyHash, deformatVault(), GenerateVaultKey(username, password));

        if (response == "-999")
        {
            std::cout << system_prefix + "task failed, lost connection to server" << "\n";
            return;
        }

        std::cout << response << "\n";
    }
};

class addDataCommand : public command {
public:
    addDataCommand() {}
    void onCommand(std::vector<std::string> args) override {
        if (args.size() != 2) {
            std::cout << system_prefix + "incorrect args" << "\n";
            return;
        }
        if (args[1].find('/') != std::string::npos) {
            std::cout << system_prefix + "illegal character in data, data can't contains '/'" << "\n";
            return;
        }
        vault.push_back(args[1]);
        std::cout << system_prefix + "data added to local-vault successfully, type 'updateVault' to update the vault on the server" << "\n";
    }
};

class printVaultCommand : public command {
public:
    printVaultCommand() {}
    void onCommand(std::vector<std::string> args) override {
        if (vault.empty()) {
            std::cout << system_prefix + "vault is empty" << "\n";
            return;
        }
        for (int i = 0; unsigned(i) < vault.size(); i++) {
            std::cout << "acc[" << i+1 << "]: " << vault[i] << "\n";
        }
    }
};

std::vector<std::string> split(std::string args, char seperator) {
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

void formatVault(std::string vaultStr) {
    // first decrypt vault

    std::string word;

    vaultStr += "/";

    for (size_t i = 0; i < vaultStr.length(); i++) {
        if (vaultStr[i] == '/') {
            vault.push_back(word);
            word = "";
        }
        else
        {
            word += vaultStr[i];
        }
    }
}



void init() {

    const std::string version = "v0.0.1";

    std::cout << "+=+------------------+=+" << "\n";
    std::cout << "PasswordManager-Client-" << version << "\n";
    std::cout << "+=+------------------+=+" << "\n";
    std::cout << "" << "\n";
    std::cout << "" << "\n";

    std::map<std::string, command*> commands;

    commands.insert(std::make_pair("help", new helpCommand()));
    commands.insert(std::make_pair("exit", new exitCommand()));
    commands.insert(std::make_pair("createVault", new createVaultCommand()));
    commands.insert(std::make_pair("updateVault", new updateVaultCommand()));
    commands.insert(std::make_pair("addData", new addDataCommand()));
    commands.insert(std::make_pair("printVault", new printVaultCommand()));

    std::string startingAction = "";

    std::cout << "login/register?" << "\n";
    while (startingAction != "login" && startingAction != "register")
    {
        std::cout << ">> ";
        getline(std::cin, startingAction);
    }
    std::cout << "" << "\n";

    while (true)
    {
        std::cout << "username: ";
        getline(std::cin, username);
        std::cout << "password: ";
        getline(std::cin, password);

        authKeyHash = HashAuthKey(username + password);

        if (startingAction == "login") {
            std::cout << system_prefix + "requesting vault from server . . ." << "\n";

            std::string response = SendVaultRequest(authKeyHash);

            // if can't connect to server, stop the program
            if (response == "-999") {
                return;
            }

            if (response != "vault not found") {
                std::cout << system_prefix << "Vault received!, decrypting vault . . ." << "\n";

                if (response != "vault is empty")
                    formatVault(response);

                std::cout << system_prefix << "vault decrypted and formatted" << "\n";
                break;
            }
            else {
                std::cout << system_prefix << "No vault found for these credentials, do you want to register a new user or retry logging in to existing one?" << "\n";
                std::string input = " ";
                std::cout << "(register/relogin)>> ";
                getline(std::cin, input);
                if (input == "register") {
                    std::cout << "type 'createVault' to create a new empty vault" << "\n";
                    break;
                }
            }
        }
        else {
            commands["createVault"]->onCommand(split("createVault", ' '));
            break;
        }
    }
    std::cout << "" << "\n";

    std::string input = " ";

    while (running) {

        std::cout << ">> ";
        getline(std::cin, input);

        if (input.size() > 0) {

            // get command name
            std::string commandName = "";

            // gets the first word in a string
            for (size_t i = 0; i < input.size(); i++) {
                if (input[i] == ' ') {
                    break;
                }
                else {
                    commandName += input[i];
                }
            }

            // if the name is in the map keys, get and execute the command
            if (commands.count(commandName)) {
                commands[commandName]->onCommand(split(input, ' '));
            }
            else
            {
                std::cout << system_prefix + "Unknown command. Type help for help." << "\n";
            }
        }
    }

}

int main(int argc, char const* argv[])
{
    init();
    return 0;
}