#pragma once


#if defined(WASM)
#define PLATFORM_STR "Wasm"

#include <stdint.h>

enum { /* TODO */
     SYS_PROT_NONE  = 0,
     SYS_PROT_READ  = 0, 
     SYS_PROT_WRITE = 0,
     SYS_PROT_EXEC  = 0,
     SYS_MADV_RANDOM = 0,
     SYS_MADV_SEQUENTIAL = 0,
};

static uint8_t g_buf[1024*1024*1024];
static size_t g_buf_len = 0;

static void* sys_mmap_anon_shared(size_t size, int, int)
{
    /* FIXME: this program relies on few memory allocations, a simple bump
     * allocator works for now, but will cause memory leaks in the future */

    size = (size + 7ULL) & ~7ULL;

    if (g_buf_len + size > sizeof g_buf) {
        return NULL;
    }
    void* x = &g_buf[g_buf_len];
    g_buf_len += size;
    return x;
}

#elif defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
#define PLATFORM_STR "Posix"

#define _DARWIN_C_SOURCE 1 /* defines MAP_ANON */
#include <sys/mman.h>

#define index string_index
#include <string.h>
#undef index

#include <errno.h>
#include <stdio.h>

enum {
     SYS_PROT_NONE  = PROT_NONE,
     SYS_PROT_READ  = PROT_READ, 
     SYS_PROT_WRITE = PROT_WRITE,
     SYS_PROT_EXEC  = PROT_EXEC,

     SYS_MADV_RANDOM = MADV_RANDOM,
     SYS_MADV_SEQUENTIAL = MADV_SEQUENTIAL,
};

static void* sys_mmap_anon_shared(size_t size, int prot, int madv)
{
    void* x = mmap(NULL, size, prot, MAP_ANON | MAP_SHARED, -1, 0);
    if (x == MAP_FAILED) {
#ifndef NDEBUG
        fprintf(stderr, "%s (" PLATFORM_STR "): %s\n", __func__, strerror(errno));
#endif
        return NULL;
    }

    /* mmap is de-facto zero filled on all platforms ^1, but zero it just in case
       [1]: https://stackoverflow.com/a/65084762 */
    memset(x, 0, size);

    if (madv) {
        int ok = madvise(x, size, madv);
        if (ok == -1) {
            fprintf(stderr, "%s (" PLATFORM_STR "): %s\n", __func__, strerror(errno));
            munmap(x, size);
            return NULL;
        }
    }

    return x;
}

#elif defined(_WIN32)
#define PLATFORM_STR "Win32"

/* TODO: this is untested */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdio.h>

enum {
    SYS_PROT_NONE  = 0,
    SYS_PROT_READ  = 1 << 0,
    SYS_PROT_WRITE = 1 << 1,
    SYS_PROT_EXEC  = 1 << 2,
};

static DWORD sys_win_page_protect_from_prot(int prot)
{
    const int r = (prot & SYS_PROT_READ)  != 0;
    const int w = (prot & SYS_PROT_WRITE) != 0;
    const int x = (prot & SYS_PROT_EXEC)  != 0;

    if (!r && !w && !x)
        return PAGE_NOACCESS;

    if (x) {
        if (w) return PAGE_EXECUTE_READWRITE;
        if (r) return PAGE_EXECUTE_READ;
        return PAGE_EXECUTE;
    } else {
        if (w) return PAGE_READWRITE;   /* no write-only pages on Windows */
        if (r) return PAGE_READONLY;
        return PAGE_NOACCESS;
    }
}

static void sys_win_debug_print_last_error(const char* func_name)
{
#ifdef NDEBUG
    (void)func_name
#else
    DWORD err = GetLastError();
    char* msg = NULL;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&msg,
        0,
        NULL
    );

    if (msg) {
        fprintf(stderr, "%s (" PLATFORM_STR "): %s", func_name, msg);
        LocalFree(msg);
    } else {
        fprintf(stderr, "%s (" PLATFORM_STR "): GetLastError=%lu\n",
                func_name, (unsigned long)err);
    }
#endif
}

static void* sys_mmap_anon_shared(size_t size, int prot)
{
    if (size == 0)
        return NULL;

    DWORD protect = sys_win_page_protect_from_prot(prot);

    /* VirtualAlloc doesn't support PAGE_NOACCESS */
    DWORD alloc_protect = (protect == PAGE_NOACCESS)
                        ? PAGE_READWRITE
                        : protect;

    void* x = VirtualAlloc(
        NULL,
        size,
        MEM_RESERVE | MEM_COMMIT,
        alloc_protect
    );

    if (!x) {
        sys_win_debug_print_last_error(__func__);
        return NULL;
    }

    if (protect == PAGE_NOACCESS) {
        DWORD oldp;
        if (!VirtualProtect(p, size, PAGE_NOACCESS, &oldp)) {
            sys_win_debug_print_last_error(__func__);
            VirtualFree(p, 0, MEM_RELEASE);
            return NULL;
        }
    }

    return p;
}

#else

#error Unsupported platform

#endif
