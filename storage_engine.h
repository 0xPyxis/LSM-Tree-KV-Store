#pragma once
#include "memtable.h"
#include <string>
#include <optional>
#include "wal.h"
#include <vector>
#include "bloom_filter.h"
#include <unordered_map>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

class StorageEngine
{
public:
    StorageEngine();

    void put(const std::string &key, const std::string &value);
    void remove(const std::string &key);

    std::optional<std::string> get(const std::string &key) const;

    ~StorageEngine();


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

    std::thread compaction_thread_;

    std::mutex compaction_mutex_;
    std::condition_variable compaction_cv_;

    std::queue<bool> compaction_tasks_;

    std::atomic<bool> stop_background_{false};

    void compaction_worker();

    Memtable memtable_;
    std::unique_ptr<Memtable> immutable_memtable_;
};