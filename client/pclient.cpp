#include "iostream"
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <winsock2.h>
#include "SocketUtil.hh"

extern bool running = true;
extern const std::string system_prefix = "system >> ";

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
        std::cout << "====================\n - exit\n - help\n - printVault\n====================" << std::endl;
    }
};

class exitCommand : public command {
public:
    exitCommand() {}
    void onCommand(std::vector<std::string> args) override {
        std::cout << "exiting . . ." << std::endl;
        running = false;
    }
};

class printVaultCommand : public command {
public:
    printVaultCommand() {}
    void onCommand(std::vector<std::string> args) override {
        if (vault.empty()) {
            std::cout << "vault is empty" << std::endl;
            return;
        }
        for (int i = 0; i < vault.size(); i++) {
            std::cout << "acc[" << i+1 << "]: " << vault[i] << std::endl;
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

    vaultStr += "|";

    for (size_t i = 0; i < vaultStr.length(); i++) {
        if (vaultStr[i] == '|') {
            vault.push_back(word);
            word = "";
        }
        else
        {
            word += vaultStr[i];
        }
    }
}

std::string hashAuthKey(std::string plainTextKey) {
    // Hash the key 50 times
    return plainTextKey;
}

bool sendRequestToServer(std::string username, std::string password) {
    std::string responseFromServer = SendSocketMessage("vaultRequest|" + hashAuthKey(username + password) + "|" + "dummy");

    bool authSuccessful = responseFromServer != "wrong login credentials";

    // if authentication was successful, format our vault in memory
    if (authSuccessful)
        formatVault(responseFromServer);

    return authSuccessful;
}

void init() {

    const std::string version = "v0.0.1";

    std::cout << "+=+------------------+=+" << std::endl;
    std::cout << "PasswordManager-Client-" << version << std::endl;
    std::cout << "+=+------------------+=+" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;

    std::string username;
    std::string password;

    while (true)
    {
        std::cout << "username: ";
        getline(std::cin, username);
        std::cout << "password: ";
        getline(std::cin, password);

        if (sendRequestToServer(username, password)) {
            std::cout << system_prefix << "Access granted!, vault received" << std::endl;
            break;
        }
        else {
            std::cout << system_prefix << "incorrect login credentials, please try again" << std::endl;
        }
    }

    std::map<std::string, command*> commands;

    commands.insert(std::make_pair("help", new helpCommand()));
    commands.insert(std::make_pair("exit", new exitCommand()));
    commands.insert(std::make_pair("printVault", new printVaultCommand()));

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
                std::cout << system_prefix + "Unknown command. Type help for help." << std::endl;
            }
        }
    }

}

int main(int argc, char const* argv[])
{
    init();
    return 0;
}