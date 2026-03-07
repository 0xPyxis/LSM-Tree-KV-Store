#include "compaction.h"
#include <sstream>
#include <map>
#include <fstream>

std::string Compaction::run(
    const std::vector<std::string> &sstables,
    int new_file_id)
{
    std::map<std::string, std::string> merged;

    for (const auto &file : sstables)
    {
        std::ifstream in(file);
        std::string line;

        while (std::getline(in, line))
        {
            std::istringstream iss(line);

            std::string key, value;

            if (std::getline(iss, key, '|') && std::getline(iss, value))
            {
                merged[key] = value;
            }
        }
    }

    std::string new_file = "sstable_" + std::to_string(new_file_id) + ".dat";

    std::ofstream out(new_file);

    for (const auto &[key, value] : merged)
    {
        if (value != "__TOMBSTONE__")
            out << key << "|" << value << "\n";
    }
    out.close();

    return new_file;
}