#include "mmfilereader.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <cmath>
#include <sys/mman.h>

MMFileReader::MMFileReader(std::string filepath_)
: filepath{std::move(filepath_)} {
    int res;
        struct stat st_buf;
        res = stat(filepath.c_str(), &st_buf);
        if(res == -1) {
            throw std::runtime_error(strerror(errno));
        }
        auto file_size = st_buf.st_size;
        if(file_size < sizeof(Header)) {
            throw std::runtime_error("file size too small");
        }
        auto page_size = sysconf(_SC_PAGE_SIZE);
        real_mapped_size = page_size * ceil(static_cast<double>(file_size) / page_size);

    res = open(filepath.c_str(), O_RDONLY);
    if(res == -1) {
        throw std::runtime_error(strerror(errno));
    }
    fd = res;

    base_ptr = mmap(0, real_mapped_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(MAP_FAILED == base_ptr) {
        release();
        throw std::runtime_error(strerror(errno));
    }

    if(header().size != file_size) {
        release();
        throw std::runtime_error("file size does not match header");
    }

    read_offset = sizeof(Header);
}

MMFileReader::~MMFileReader() noexcept {
    release();
}

Header const& MMFileReader::header() const {
    return *static_cast<Header*>(base_ptr);
}

void MMFileReader::release() noexcept
{
    if (fd != -1)
    {
        ::close(fd);
    }
    if (base_ptr != 0)
    {
        ::munmap(base_ptr, real_mapped_size);
    }
}

void* MMFileReader::read_posi()  const {
    return static_cast<char*>(base_ptr) + read_offset;
}

size_t MMFileReader::offset() const {
    return read_offset;
}

size_t MMFileReader::size() const {
    return header().size;
}

std::string const& MMFileReader::path() const {
    return filepath;
}