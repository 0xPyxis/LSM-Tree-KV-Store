#include "wal.h"
#include <iostream>

WAL::WAL(const std::string &filename)
    : filename_(filename)
{
    file_.open(filename_, std::ios::app);
    if (!file_.is_open())
    {
        throw std::runtime_error("Failed to open WAL file");
    }
}

WAL::~WAL()
{
    close();
}

void WAL::append_put(const std::string &key, const std::string &value)
{
    file_ << "PUT" << key << " " << value << "\n";
    file_.flush();
}

void WAL::append_delete(const std::string &key)
{
    file_ << "DEL " << key << "\n";
    file_.flush();
}

void WAL::sync()
{
    file_.flush();
}

void WAL::close()
{
    if (file_.is_open())
    {
        file_.close();
    }
}
