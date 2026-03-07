#pragma once
#include <vector>
#include <string>

class Compaction
{
public:
    static std::string run(
        const std::vector<std::string> &sstables,
        int new_file_id);
};