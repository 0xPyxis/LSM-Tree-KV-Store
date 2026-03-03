#include "wal.h"
#include <iostream>
#include <sstream>

WAL::WAL(const std::string &filename)
    : filename_(filename)
{
    file_.open(filename_, std::ios::app);
    if (!file_.is_open())
    {
        throw std::runtime_error("Failed to open WAL file");
    }
}

WAL::~WAL()
{
    close();
}

void WAL::append_put(const std::string &key, const std::string &value)
{
    file_ << "PUT " << key << " " << value << "\n";
    file_.flush();
}

void WAL::append_delete(const std::string &key)
{
    file_ << "DEL " << key << "\n";
    file_.flush();
}

void WAL::sync()
{
    file_.flush();
}

void WAL::close()
{
    if (file_.is_open())
    {
        file_.close();
    }
}

void WAL::replay(const std::function<void(const std::string&, const std::string&, bool)> & apply) {
    std::ifstream infile(filename_);
    if(!infile.is_open())
        return;
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string command;

        iss>>command;

        if(command == "PUT") {
            std::string key,value;
            if (!(iss>>key>>value)) 
                break; // incomplete line - stop replay
            apply(key, value, false);
        
        } else if (command == "DEL") {
            std::string key;
            if (!(iss>>key)) 
                break; // incomplete line
            apply(key, "", true) ;
        
        } else {
            break; // corrupted line - stop safely
        }
    }
}
