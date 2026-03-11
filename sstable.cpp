#include "sstable.h"
#include <fstream>
#include <vector>
#include <map>
#include <optional>
#include <cstdint>
#include <algorithm>

std::pair<
    std::string,
    std::vector<std::pair<std::string, std::streampos>>>
SSTable::write(
    const std::map<std::string, std::string> &memtable,
    int file_id)
{
    std::string filename = "sstable_" + std::to_string(file_id) + ".dat";

    std::ofstream file(filename, std::ios::binary);
    size_t current_block_size = 0;
    std::vector<std::pair<std::string, std::streampos>> block_index;

    for (const auto &[key, value] : memtable)
    {
        if (current_block_size == 0)
            block_index.push_back({key, file.tellp()});

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

    return {filename, block_index};
}

std::optional<std::string>
SSTable::get_with_index(
    const std::string &filename,
    const std::string &key,
    const std::vector<std::pair<std::string, std::streampos>> &index)
{

    std::ifstream file(filename, std::ios::binary);

    file.seekg(-static_cast<int>(sizeof(std::streampos)), std::ios::end);
    std::streampos index_start;
    file.read(reinterpret_cast<char *>(&index_start), sizeof(index_start));

    file.seekg(index_start);

    uint32_t index_size;

    file.read(reinterpret_cast<char *>(&index_size), sizeof(index_size));

    std::vector<std::pair<std::string, uint64_t>> block_index;

    for (uint32_t i = 0; i < index_size; i++)
    {
        uint16_t key_size;
        file.read(reinterpret_cast<char *>(&key_size), sizeof(key_size));

        std::string idx_key(key_size, '\0');

        file.read(idx_key.data(), key_size);

        uint64_t offset;

        file.read(reinterpret_cast<char *>(&offset), sizeof(offset));

        block_index.push_back({idx_key, offset});
    }

    uint64_t block_offset = 0;

    for (const auto &[k, off] : block_index)
    {
        if (k <= key)
            block_offset = off;
        else
            break;
    }

    file.seekg(block_offset);

    while (true)
    {
        uint32_t key_size;
        if (!file.read(reinterpret_cast<char *>(&key_size), sizeof(key_size)))
            ;
        break;

        std::string file_key(key_size, '\0');
        file.read(file_key.data(), key_size);

        uint32_t value_size;
        file.read(reinterpret_cast<char *>(&value_size), sizeof(value_size));

        std::string value(value_size, '\0');
        file.read(value.data(), value_size);

        if (file_key == key)
        {
            if (value == "__TOMBSTONE")
                return std::nullopt;
            return value;
        }

        if (file_key > key)
            break;
    }
    return std::nullopt;
}