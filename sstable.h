#pragma once
#include <string>
#include <map>

class SSTable
{
public:
    static std::string write(
        const std::map<std::string, std::string> &memtable,
        int file_id);
};
