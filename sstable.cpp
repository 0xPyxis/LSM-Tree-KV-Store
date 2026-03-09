#include "sstable.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <optional>

std::pair<
    std::string,
    std::vector<std::pair<std::string, std::streampos>>>
SSTable::write(const std::map<std::string, std::string> &memtable, int file_id)
{
    std::string filename = "sstable_" + std::to_string(file_id) + ".dat";

    std::ofstream file(filename);

    std::vector<std::pair<std::string, std::streampos>> index;

    int counter = 0;
    const int INDEX_STEP = 3;

    for (const auto &[key, value] : memtable)
    {
        std::streampos pos = file.tellp();

        file << key << "|" << value << "\n";

        if (counter % INDEX_STEP == 0)
            index.push_back({key, pos});

        counter++;
    }

    file.close();

    return {filename, index};
}

std::optional<std::string> SSTable::get(
    const std::string &filename,
    const std::string &key)
{
    std::ifstream file(filename);

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);

        std::string file_key;
        std::string value;

        if (std::getline(iss, file_key, '|') &&
            std::getline(iss, value))
        {
            if (file_key == key)
            {
                if (value == "__TOMBSTONE__")
                    return std::nullopt;
                return value;
            }
        }
    }
    return std::nullopt;
}