#include "mmfilebuilder.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <errno.h>
#include <cstring>
#include <sys/mman.h>
#include <cassert>
#include <unistd.h>
#include <cmath>

// create file for write
MMFileBuilder::MMFileBuilder(std::string filepath_, size_t max_size_)
    : filepath{std::move(filepath)}, max_size{max_size}
{
    if (max_size < sizeof(Header))
    {
        throw std::runtime_error("max_size too small");
    }

    auto page_size = sysconf(_SC_PAGE_SIZE);
    real_mapped_size = page_size * ceil(static_cast<double>(max_size) / page_size);

    int res;
    // create a file
    res = open(filepath.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (res == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    fd = res;

    // allocate size
    res = fallocate(fd, 0, 0, real_mapped_size);
    if (res == -1)
    {
        release();
        throw std::runtime_error(strerror(errno));
    }

    // map to memory
    base_ptr = mmap(0, real_mapped_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base_ptr == MAP_FAILED)
    {
        release();
        throw std::runtime_error(strerror(errno));
    }

    // offset = sizeof(Header);
    header().size = sizeof(Header);
    header().version = 1;
}

MMFileBuilder::~MMFileBuilder()
{
    int res;

    res = fallocate(fd, FALLOC_FL_PUNCH_HOLE, header().size, real_mapped_size - header().size);
    if (res == -1)
    {
        release();
        throw std::runtime_error(strerror(errno));
    }

    res = munmap(base_ptr, real_mapped_size);
    if (res == -1)
    {
        release();
        throw std::runtime_error(strerror(errno));
    }
}



size_t MMFileBuilder::size() const
{
    return header().size;
}

Header &MMFileBuilder::header()
{
    return *static_cast<Header *>(base_ptr);
}

Header const &MMFileBuilder::header() const
{
    return *static_cast<Header *>(base_ptr);
}

void *MMFileBuilder::write_posi() const
{
    return static_cast<char *>(base_ptr) + header().size;
}

void MMFileBuilder::release() noexcept
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


void MMFileBuilder::sync() {
    syncfs(fd);
}