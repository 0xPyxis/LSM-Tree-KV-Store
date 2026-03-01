#include "storage_engine.h"

StorageEngine::StorageEngine() {}

void StorageEngine::put(const std::string &key, const std::string &value)
{
    memtable_.put(key, value);
}

void StorageEngine::remove(const std::string &key)
{
    memtable_.remove(key);
}

std::optional<std::string> StorageEngine::get(const std::string &key) const
{
    return memtable_.get(key);
}
