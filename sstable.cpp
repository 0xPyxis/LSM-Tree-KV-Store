#include "sstable.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <optional>
#include <cstdint>

std::pair<
    std::string,
    std::vector<std::pair<std::string, std::streampos>>>
SSTable::write(const std::map<std::string, std::string> &memtable, int file_id)
{
    std::string filename = "sstable_" + std::to_string(file_id) + ".dat";

    std::ofstream file(filename, std::ios::binary);
    size_t current_block_size = 0;
    std::vector<std::pair<std::string, std::streampos>> block_index;

    std::vector<std::pair<std::string, std::streampos>> index;

    int counter = 0;
    const int INDEX_STEP = 3;

    for (const auto &[key, value] : memtable)
    {
        if (current_block_size == 0)
            block_index.push_back({key, file.tellp()});

        std::streampos pos = file.tellp();

        if (counter % INDEX_STEP == 0)
            index.push_back({key, pos});

        uint32_t key_size = key.size();
        uint32_t value_size = value.size();

        file.write(reinterpret_cast<char *>(&key_size), sizeof(key_size));
        file.write(key.data(), key_size);

        file.write(reinterpret_cast<char *>(&value_size), sizeof(value_size));
        file.write(value.data(), value_size);

        size_t record_size = sizeof(key_size) + key_size +
                             sizeof(value_size) + value_size;

        current_block_size += record_size;

        if (current_block_size >= BLOCK_SIZE)
        {
            current_block_size = 0;
        }

        counter++;
    }

    std::streampos index_start = file.tellp();

    uint32_t index_size = block_index.size();

    file.write(reinterpret_cast<char *>(&index_size), sizeof(index_size));

    for (const auto &entry : block_index)
    {

        const std::string &key = entry.first;
        uint64_t offset = static_cast<uint64_t>(entry.second);

        uint32_t key_size = key.size();

        file.write(reinterpret_cast<char *>(&key_size), sizeof(key_size));
        file.write(key.data(), key_size);

        file.write(reinterpret_cast<char *>(&offset), sizeof(offset));
    }

    file.write(reinterpret_cast<char *>(&index_start), sizeof(index_start));

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

std::optional<std::string>
SSTable::get_with_index(
    const std::string &filename,
    const std::string &key,
    const std::vector<std::pair<std::string, std::streampos>> &index)
{
    std::ifstream file(filename, std::ios::binary);

    std::streampos start = 0;

    for (const auto &[k, pos] : index)
    {
        if (k <= key)
            start = pos;
        else
            break;
    }

    file.seekg(start);

    while (true)
    {
        uint32_t key_size;
        if (!file.read(reinterpret_cast<char *>(&key_size), sizeof(key_size)))
            break;

        std::string file_key(key_size, '\0');
        file.read(file_key.data(), key_size);

        uint32_t value_size;
        file.read(reinterpret_cast<char *>(&value_size), sizeof(value_size));

        std::string value(value_size, '\0');
        file.read(value.data(), value_size);

        if (file_key == key)
        {
            if (value == "__TOMBSTONE__")
                return std::nullopt;
            return value;
        }

        if (file_key > key)
            break;
    }
    return std::nullopt;
}