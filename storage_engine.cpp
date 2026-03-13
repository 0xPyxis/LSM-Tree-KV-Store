#include "storage_engine.h"
#include "sstable.h"
#include "compaction.h"

StorageEngine::StorageEngine()
    : wal_("wal.log")
{
    wal_.replay([this](const std::string &key,
                       const std::string &value,
                       bool is_delete)
                {
            if(is_delete) 
                memtable_.remove(key);
            else
                memtable_.put(key, value); });

    wal_.open_for_append(); // open only after replay (since we are opening in append mode)
                            // the cursor will go to last.
    compaction_thread_ = std::thread(&StorageEngine::compaction_worker, this);
}

StorageEngine::~StorageEngine()
{
    stop_background_ = true;

    compaction_cv_.notify_all();

    if (compaction_thread_.joinable())
        compaction_thread_.join();
}

void StorageEngine::put(const std::string &key, const std::string &value)
{
    wal_.append_put(key, value);
    memtable_.put(key, value);

    if (memtable_.size() >= MEMTABLE_LIMIT)
    {
        immutable_memtable_ = std::make_unique<Memtable>(std::move(memtable_));
        memtable_ = Memtable();
        std::lock_guard<std::mutex> lock(compaction_mutex_);
        compaction_tasks_.push(true);
        compaction_cv_.notify_one();
    }
}

void StorageEngine::remove(const std::string &key)
{
    wal_.append_delete(key);
    memtable_.remove(key);

    if (memtable_.size() >= MEMTABLE_LIMIT)
    {
        immutable_memtable_ = std::make_unique<Memtable>(std::move(memtable_));
        memtable_ = Memtable();
        std::lock_guard<std::mutex> lock(compaction_mutex_);
        compaction_tasks_.push(true);
        compaction_cv_.notify_one();
    }
}

void StorageEngine::flush_memtable()
{
    if (!immutable_memtable_)
        return;

    BloomFilter filter;

    for (const auto &[key, value] : immutable_memtable_->get_table())
        filter.add(key);

    auto result = SSTable::write(memtable_.get_table(), next_sstable_id_++);
    std::string filename = result.first;

    sstables_.push_back(filename);

    bloom_filters_[filename] = filter;

    immutable_memtable_.reset();
}

void StorageEngine::run_compaction()
{
    std::string new_file = Compaction::run(sstables_, next_sstable_id_++);

    for (const auto &f : sstables_)
        std::remove(f.c_str());

    sstables_.clear();

    sstables_.push_back(new_file);
}

std::optional<std::string> StorageEngine::get(const std::string &key) const
{
    auto result = memtable_.get(key);

    if (result)
        return result;

    for (auto it = sstables_.rbegin(); it != sstables_.rend(); ++it)
    {
        const std::string &file = *it;

        auto bf = bloom_filters_.find(file);

        if (bf != bloom_filters_.end())
        {
            if (!bf->second.possibly_contains(key))
                continue;
        }

        auto idx = sparse_indexes_.find(file);

        if (idx != sparse_indexes_.end())
        {
            auto val = SSTable::get_with_index(file, key, idx->second);

            if (val)
                return val;
        }
    }
    return std::nullopt;
}

void StorageEngine::compaction_worker()
{
    while (!stop_background_)
    {
        std::unique_lock<std::mutex> lock(compaction_mutex_);
        compaction_cv_.wait(lock, [&]
                            { return !compaction_tasks_.empty() || stop_background_; });

        if (stop_background_)
            return;
        compaction_tasks_.pop();

        lock.unlock();

        if (immutable_memtable_)
            flush_memtable();

        if (sstables_.size() > COMPACTION_THRESHOLD)
            run_compaction();
    }
}
