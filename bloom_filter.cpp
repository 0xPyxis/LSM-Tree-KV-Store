#include "bloom_filter.h"
#include <functional>

BloomFilter::BloomFilter(size_t size) : size_(size), bits_(size, false)
{
}

size_t BloomFilter::hash1(const std::string &key) const
{
    return std::hash<std::string>{}(key) % size_;
}

size_t BloomFilter::hash2(const std::string &key) const
{
    return (std::hash<std::string>{}(key) * 31) % size_;
}

size_t BloomFilter::hash3(const std::string &key) const
{
    return (std::hash<std::string>{}(key) * 131) % size_;
}

void BloomFilter::add(const std::string &key)
{
    bits_[hash1(key)] = true;
    bits_[hash2(key)] = true;
    bits_[hash3(key)] = true;
}

bool BloomFilter::possibly_contains(const std::string &key) const
{
    return bits_[hash1(key)] &&
           bits_[hash2(key)] &&
           bits_[hash3(key)];
}