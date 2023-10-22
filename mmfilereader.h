#ifndef MMFILE_READER_H
#define MMFILE_READER_H

#include "header.h"
#include <string>
#include <optional>

class MMFileReader {
public:
    MMFileReader(std::string filepath);
    ~MMFileReader() noexcept;

    template<typename T>
    std::optional<T> read_one() {
        if(read_offset + sizeof(T) > header().size) {
            return {};
        }
        std::optional<T> res;
        res = static_cast<T*>(read_posi());
        read_offset += sizeof(T);
        return res;
    }
    
    size_t offset() const;
    size_t size() const;
    std::string const& path() const;

private:
    Header const& header() const;
    void release() noexcept;
    void* read_posi() const;

private:
    std::string filepath;
    int fd = -1;
    void* base_ptr = nullptr;
    size_t read_offset = 0;
    size_t real_mapped_size = 0;

};

#endif