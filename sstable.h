#pragma once
#include <string>
#include <map>
#include <vector>
#include <optional>

class SSTable
{
public:
    static std::pair<std::string, std::vector<
                                      std::pair<std::string, std::streampos>>>
    write(
        const std::map<std::string, std::string> &memtable,
        int file_id);

    static std::optional<std::string> get(
        const std::string &filename,
        const std::string &key);
};
