#pragma once
#include "memtable.h"
#include <string>
#include <optional>
#include "wal.h"
#include <vector>
#include "bloom_filter.h"
#include <unordered_map>
#include <map>

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

    static constexpr size_t MEMTABLE_LIMIT = 5;
    static constexpr size_t COMPACTION_THRESHOLD = 3;

    void flush_memtable();
    void run_compaction();

    std::unordered_map<std::string, BloomFilter> bloom_filters_;

    std::unordered_map<
        std::string,
        std::vector<std::pair<std::string, std::streampos>>>
        sparse_indexes_;
};