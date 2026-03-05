#include "storage_engine.h"
#include "sstable.h"

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
}

void StorageEngine::put(const std::string &key, const std::string &value)
{
    wal_.append_put(key, value);
    memtable_.put(key, value);

    if (memtable_.size() >= MEMTABLE_LIMIT) {
        flush_memtable();
    }
}

void StorageEngine::remove(const std::string &key)
{
    wal_.append_delete(key);
    memtable_.remove(key);
}

void StorageEngine::flush_memtable()
{
    std::string filename = SSTable::write(memtable_.get_table(), next_sstable_id_++);

    sstables_.push_back(filename);

    memtable_.clear();
}

std::optional<std::string> StorageEngine::get(const std::string &key) const
{
    return memtable_.get(key);
}
