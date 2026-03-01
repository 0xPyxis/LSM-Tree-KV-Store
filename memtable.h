#pragma once
#include <string>
#include <map>
#include <optional>

class Memtable
{
public:
    Memtable();

    void put(const std::string &key, const std::string &value);
    void remove(const std::string &key);

    std::optional<std::string> get(const std::string &key) const;

    size_t size() const;
    void clear();

private:
    std::map<std::string, std::string> table_;
    static const std::string TOMBSTONE;
};