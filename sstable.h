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

    static std::optional<std::string>
    get_with_index(
        const std::string &filename,
        const std::string &key,
        const std::vector<std::pair<std::string, std::streampos>> &index);
    
    static constexpr size_t BLOCK_SIZE = 4096;
};
