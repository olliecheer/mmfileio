#ifndef MMFILE_IO_HEADER
#define MMFILE_IO_HEADER

#include <cstddef>
#include <cstdint>

    struct Header {
        uint8_t version;
        size_t size;
    } __attribute__((packed));

#endif