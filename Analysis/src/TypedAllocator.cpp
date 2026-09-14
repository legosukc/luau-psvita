// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "Luau/TypedAllocator.h"

#include "Luau/Common.h"

#include <malloc.h>

#include <stdint.h>
#include <stdlib.h>

#include <psp2/kernel/sysmem.h>
#include <psp2kern/kernel/sysmem/memtype.h>
#include <psp2kern/kernel/sysmem.h>

const size_t kPageSize = 4096;

LUAU_FASTFLAG(DebugLuauFreezeArena)

namespace Luau
{

static size_t pageAlign(size_t size)
{
    return (size + kPageSize - 1) & ~(kPageSize - 1);
}

void* pagedAllocate(size_t size)
{
    // By default we use operator new/delete instead of malloc/free so that they can be overridden externally
    if (!FFlag::DebugLuauFreezeArena)
    {
        return ::operator new(size, std::nothrow);
    }
    return memalign(kPageSize, size);
}

void pagedDeallocate(void* ptr, size_t size)
{
    // By default we use operator new/delete instead of malloc/free so that they can be overridden externally
    if (!FFlag::DebugLuauFreezeArena)
        return ::operator delete(ptr);

    free(ptr);
}

void pagedFreeze(void* ptr, size_t size)
{
    LUAU_ASSERT(FFlag::DebugLuauFreezeArena);
    LUAU_ASSERT(uintptr_t(ptr) % kPageSize == 0);
    /*
#ifdef _WIN32
    DWORD oldProtect;
    BOOL rc = VirtualProtect(ptr, pageAlign(size), PAGE_READONLY, &oldProtect);
    LUAU_ASSERT(rc);
#else
    int rc = mprotect(ptr, pageAlign(size), PROT_READ);
    LUAU_ASSERT(rc == 0);
#endif*/
}

void pagedUnfreeze(void* ptr, size_t size)
{
    LUAU_ASSERT(FFlag::DebugLuauFreezeArena);
    LUAU_ASSERT(uintptr_t(ptr) % kPageSize == 0);
    /*
#ifdef _WIN32
    DWORD oldProtect;
    BOOL rc = VirtualProtect(ptr, pageAlign(size), PAGE_READWRITE, &oldProtect);
    LUAU_ASSERT(rc);
#else
    int rc = mprotect(ptr, pageAlign(size), PROT_READ | PROT_WRITE);
    LUAU_ASSERT(rc == 0);
#endif*/
}

} // namespace Luau
