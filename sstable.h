#pragma once
#include <string>
#include <map>
#include <optional>

class SSTable
{
public:
    static std::string write(
        const std::map<std::string, std::string> &memtable,
        int file_id);

    static std::optional<std::string> get(
        const std::string &filename,
        const std::string &key
    );
};
