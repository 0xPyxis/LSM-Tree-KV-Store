#pragma once
#include "memtable.h"
#include <string>
#include <optional>

class StorageEngine
{
public:
    StorageEngine();

    void put(const std::string &key, const std::string &value);
    void remove(const std::string &key);

    std::optional<std::string> get(const std::string &key) const;

private:
    Memtable memtable_;
};