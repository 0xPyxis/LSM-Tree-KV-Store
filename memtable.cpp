#include "memtable.h"

const std::string Memtable::TOMBSTONE = "__TOMBSTONE__";

Memtable::Memtable() {}

void Memtable::put(const std::string &key, const std::string &value)
{
    table_[key] = value;
}

void Memtable::remove(const std::string &key)
{
    table_[key] = TOMBSTONE;
}

std::optional<std::string> Memtable::get(const std::string &key) const
{
    auto it = table_.find(key);

    if (it == table.end())
        return std::nullopt;

    if (it->second == TOMBSTONE)
        return std::nullopt;

    return it->second;
}

size_t Memtable::size() const
{
    return table_.size();
}

void Memtable::clear()
{
    table_.clear();
}