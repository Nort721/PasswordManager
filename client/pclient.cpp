#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <winsock2.h>
#include "CryptoUtil.hpp"
#include "NetworkUtil.hpp"
#include "StringUtil.hpp"

extern bool running = true;
extern const std::string system_prefix = "system >> ";
extern std::string authKeyHash = "";

extern std::string username = "", password = "";

static std::vector<std::string> *vault = new std::vector<std::string>();

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
        std::string response = SendVaultCreateRequest(authKeyHash, "", GenerateVaultKey(username, password));

        if (response == "you already have a vault") {
            response += ", type 'updateVault' to update your vault";
        }
        std::cout << system_prefix + response << "\n";
    }
};

class updateVaultCommand : public command {
public:
    updateVaultCommand() {}
    void onCommand(std::vector<std::string> args) override {
        if (args.size() > 1) {
            std::cout << system_prefix + "incorrect args" << "\n";
            return;
        }
        std::string response = SendVaultUpdateRequest(authKeyHash, DeformatVault(vault), GenerateVaultKey(username, password));

        if (response == "-999")
        {
            std::cout << system_prefix + "task failed, lost connection to server" << "\n";
            return;
        }

        std::cout << response << "\n";
    }
};

class deleteVaultCommand : public command {
public:
    deleteVaultCommand() {}
    void onCommand(std::vector<std::string> args) override {

        std::cout << system_prefix + "this action will PERMANETLY delete your vault from the server, and there is no backup of it, are you sure you want to proceed?" << "\n";
        std::cout << "Y/N" << "\n";
        std::string input;
        std::cout << ">> ";
        std::cin >> input;

        if (input == "N") {
            std::cout << "action canceled." << "\n";
            return;
        }
        else if (input == "Y") {
            std::cout << system_prefix + "Are you sure you want to delete your vault and lose all the data in it forever" << "\n";
            std::cout << system_prefix + "Yes/No'" << "\n";
            std::cout << ">> ";
            std::cin >> input;
            // if they didn't type that they know what they are doing, cancel anyways
            if (input != "Yes") {
                std::cout << "action canceled." << "\n";
                return;
            }
        }

        std::string response = SendVaultDeleteRequest(authKeyHash);

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
        vault->push_back(args[1]);
        std::cout << system_prefix + "data added to local-vault successfully, type 'updateVault' to update the vault on the server" << "\n";
    }
};

class printVaultCommand : public command {
public:
    printVaultCommand() {}
    void onCommand(std::vector<std::string> args) override {
        if (vault->empty()) {
            std::cout << system_prefix + "vault is empty" << "\n";
            return;
        }
        for (int i = 0; unsigned(i) < vault->size(); i++) {
            std::cout << "acc[" << i+1 << "]: " << (*vault)[i] << "\n";
        }
    }
};

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
    commands.insert(std::make_pair("deleteVault", new deleteVaultCommand()));
    commands.insert(std::make_pair("updateVault", new updateVaultCommand()));
    commands.insert(std::make_pair("addData", new addDataCommand()));
    commands.insert(std::make_pair("printVault", new printVaultCommand()));

    std::string startingAction = "";

    std::cout << "login/register?" << "\n";
    while (true)
    {
        std::cout << ">> ";
        getline(std::cin, startingAction);
        if (startingAction != "login" && startingAction != "register")
            std::cout << system_prefix + "incorrect input, options are login/register" << "\n";
        else break;
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
                    FormatVault(response, vault);

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
                std::cout << "\n";
            }
        }
        else {
            commands["createVault"]->onCommand(Split("createVault", ' '));
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
                commands[commandName]->onCommand(Split(input, ' '));
            }
            else
            {
                std::cout << system_prefix + "Unknown command. Type 'help' for help." << "\n";
            }
        }
    }

}

int main(int argc, char const* argv[])
{
    init();
    return 0;
}