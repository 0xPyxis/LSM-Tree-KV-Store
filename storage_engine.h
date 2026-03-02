#pragma once
#include "memtable.h"
#include <string>
#include <optional>
#include "wal.h"

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
};