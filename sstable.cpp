#include "sstable.h"
#include <fstream>

std::string SSTable::write(
    const std::map<std::string, std::string> &memtable,
    int file_id)
{
    std::string filename = "sstable_" + std::to_string(file_id) + ".dat";

    std::ofstream file(filename);

    for (const auto &[key, value] : memtable)
    {
        file << key << "|" << value << "\n";
    }

    file.close();

    return filename;
}