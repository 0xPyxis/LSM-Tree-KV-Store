#include <iostream>
#include <sstream>
#include "storage_engine.h"

int main()
{
    StorageEngine engine;
    std::string line;

    std::cout << "Simple LSM KV Store (Phase 1 - In Memory\n";

    while (true)
    {
        std::cout << ">";
        std::getline(std::cin, line);

        std::istringstream iss(line);
        std::string command;

        iss >> command;

        if (command == "PUT")
        {
            std::string key, value;
            iss >> key >> value;
            engine.put(key, value);
            std::cout << "OK\n";
        }
        else if (command == "GET")
        {
            std::string key;
            iss >> key;

            auto result = engine.get(key);
            if (result)
                std::cout << *result << "\n";
            else
                std::cout << "NOT FOUND\n";
        }
        else if (command == "DELETE")
        {
            std::string key;
            iss >> key;
            engine.remove(key);
            std::cout << "OK\n";
        }
        else if (command == "EXIT")
        {
            break;
        }
        else
        {
            std::cout << "Unknown command\n";
        }
    }
    return 0;
}