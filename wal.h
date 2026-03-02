#pragma once
#include <string>
#include <fstream>

class WAL {
public:
    WAL(const std::string &filename);
    ~WAL();
    
    void append_put(const std::string &key, const std::string &value);
    void append_delete(const std::string &key);

    void sync();
    void close();

private:
    std::string filename_;
    std::ofstream file_;
};