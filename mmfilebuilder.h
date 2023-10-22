#ifndef MMFILE_BUILDER_H
#define MMFILE_BUILDER_H

#include "header.h"

#include <string>
#include <optional>

class MMFileBuilder
{
public:
    // create file for write
    MMFileBuilder(std::string filepath, size_t max_size);

    ~MMFileBuilder();

    template <typename T>
    bool write_one(T const &one)
    {
        if (header().size + one > max_size)
        {
            return false;
        }
        memcpy(write_posi(), &one, sizeof(one));
        header().size += sizeof(one);
        return true;
    }

    // only necessary for in-memory fs
    void sync();

    size_t size() const;
    size_t maxsize() const;
    std::string const &path() const;

private:
    Header &header();
    Header const &header() const;
    void *write_posi() const;
    void release() noexcept;

private:
    std::string filepath;
    int fd = -1;
    void *base_ptr = nullptr;
    size_t max_size = 0; // available for create file
    size_t real_mapped_size = 0;
};

#endif