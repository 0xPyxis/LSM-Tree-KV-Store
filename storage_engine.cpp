#include "storage_engine.h"

StorageEngine::StorageEngine()
    : wal_("wal.log")
{
}

void StorageEngine::put(const std::string &key, const std::string &value)
{
    wal_.append_put(key, value);
    memtable_.put(key, value);
}

void StorageEngine::remove(const std::string &key)
{
    wal_.append_delete(key);
    memtable_.remove(key);
}

std::optional<std::string> StorageEngine::get(const std::string &key) const
{
    return memtable_.get(key);
}
