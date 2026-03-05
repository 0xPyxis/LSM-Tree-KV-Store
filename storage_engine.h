#pragma once
#include "memtable.h"
#include <string>
#include <optional>
#include "wal.h"
#include <vector>

class StorageEngine
{
public:
    StorageEngine();

    void put(const std::string &key, const std::string &value);
    void remove(const std::string &key);

    std::optional<std::string> get(const std::string &key) const;

private:
    Memtable memtable_;
    WAL wal_;

    std::vector<std::string> sstables_;
    int next_sstable_id_ = 1;

    const size_t MEMTABLE_LIMIT = 5;

    void flush_memtable();
};