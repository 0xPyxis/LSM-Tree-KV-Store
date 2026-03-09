#pragma once
#include <vector>
#include <string>

class BloomFilter {
public:
    BloomFilter(size_t size = 10000);

    void add(const std::string &key);
    bool possibly_contains(const std::string &key) const;

private:
    size_t size_;
    std::vector<bool> bits_;

    size_t hash1(const std::string &key) const;
    size_t hash2(const std::string &key) const;
    size_t hash3(const std::string &key) const;
};