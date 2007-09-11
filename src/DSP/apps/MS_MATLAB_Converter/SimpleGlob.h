// File:    SimpleGlob.h
// Library: SimpleOpt
// Author:  Brodie Thiesfield <code@jellycan.com>
// Source:  http://code.jellycan.com/simpleopt/
// Version: 2.3
//
// MIT LICENCE
// ===========
// The licence text below is the boilerplate "MIT Licence" used from:
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2006, Brodie Thiesfield
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef INCLUDED_SimpleGlob
#define INCLUDED_SimpleGlob

// on Windows we want to use MBCS aware string functions and mimic the
// Unix glob functionality. On Unix we just use glob.
#ifdef _WIN32
# include <mbstring.h>
# define sg_strchr          ::_mbschr
# define sg_strlen          ::_mbslen
# if __STDC_WANT_SECURE_LIB__
#  define sg_strcpy_s(a,n,b) ::_mbscpy_s(a,n,b)
# else
#  define sg_strcpy_s(a,n,b) ::_mbscpy(a,b)
# endif
# define sg_strcmp          ::_mbscmp
# define sg_stricmp         ::_mbsicmp
# define SOCHAR_T           unsigned char
#else
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# define sg_strchr          ::strchr
# define sg_strlen          ::strlen
# define sg_strcpy_s(a,n,b) ::strcpy(a,b)
# define sg_strcmp          ::strcmp
# define sg_stricmp         ::strcasecmp
# define SOCHAR_T           char
#endif

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// flags
enum SG_Flags {
    SG_GLOB_ERR         = 1 << 0,   // return upon read error (e.g. directory does not have read permission)
    SG_GLOB_MARK        = 1 << 1,   // append a slash (backslash in Windows) to each path which corresponds to a directory
    SG_GLOB_NOSORT      = 1 << 2,   // don't sort the returned pathnames (they are by default)
    SG_GLOB_NOCHECK     = 1 << 3,   // if no pattern matches, return the original pattern
    SG_GLOB_TILDE       = 1 << 4,   // tilde expansion is carried out (on Unix platforms)
    SG_GLOB_ONLYDIR     = 1 << 5,   // return only directories which match (not compatible with SG_GLOB_ONLYFILE)
    SG_GLOB_ONLYFILE    = 1 << 6,   // return only files which match (not compatible with SG_GLOB_ONLYDIR, Windows only)
    SG_GLOB_NODOT       = 1 << 7    // do not return the "." or ".." special files
};

// errors
enum SG_Error {
    SG_SUCCESS          = 0,        // success
    SG_ERR_MEMORY       = -1,       // out of memory
    SG_ERR_FAILURE      = -2        // general failure
};

enum SG_FileType {
    SG_FILETYPE_INVALID,
    SG_FILETYPE_FILE,
    SG_FILETYPE_DIR
};

// ----------------------------------------------------------------------------
// Util functions
class CSimpleGlobUtil
{
public:
    static const char * strchr(const char *s, char c)           { return (char *) sg_strchr((const SOCHAR_T *)s, c); }
    static const wchar_t * strchr(const wchar_t *s, wchar_t c)  { return ::wcschr(s, c); }

    // Note: char strlen returns number of bytes, not characters
    static size_t strlen(const char *s)                         { return ::strlen(s); }
    static size_t strlen(const wchar_t *s)                      { return ::wcslen(s); }

    static void strcpy_s(char *dst, size_t n, const char *src)  {
        sg_strcpy_s((SOCHAR_T *)dst, n, (const SOCHAR_T *)src);
    }
    static void strcpy_s(wchar_t *dst, size_t n, const wchar_t *src) {
# if __STDC_WANT_SECURE_LIB__
        ::wcscpy_s(dst, n, src);
#else
        ::wcscpy(dst, src);
#endif
    }

    static int strcmp(const char *s1, const char *s2)           { return sg_strcmp((const SOCHAR_T *)s1, (const SOCHAR_T *)s2); }
    static int strcmp(const wchar_t *s1, const wchar_t *s2)     { return ::wcscmp(s1, s2); }

    static int stricmp(const char *s1, const char *s2)          { return sg_stricmp((const SOCHAR_T *)s1, (const SOCHAR_T *)s2); }
#if _WIN32
    static int stricmp(const wchar_t *s1, const wchar_t *s2)    { return ::_wcsicmp(s1, s2); }
#endif // _WIN32
};

// ----------------------------------------------------------------------------
// Windows implementation
#ifdef _WIN32

#ifndef INVALID_FILE_ATTRIBUTES
# define INVALID_FILE_ATTRIBUTES    ((DWORD)-1)
#endif

template<class SOCHAR>
struct SimpleGlobImplWin
{
    bool FindFirstFileS(const char * a_pszFileSpec) {
        m_hFind = FindFirstFileA(a_pszFileSpec, &m_oFindDataA);
        return m_hFind != INVALID_HANDLE_VALUE;
    }
    bool FindFirstFileS(const wchar_t * a_pszFileSpec) {
        m_hFind = FindFirstFileW(a_pszFileSpec, &m_oFindDataW);
        return m_hFind != INVALID_HANDLE_VALUE;
    }

    bool FindNextFileS(char) {
        return FindNextFileA(m_hFind, &m_oFindDataA) != FALSE;
    }
    bool FindNextFileS(wchar_t) {
        return FindNextFileW(m_hFind, &m_oFindDataW) != FALSE;
    }

    void FindDone() {
        FindClose(m_hFind);
    }

    const char * GetFileNameS(char) const {
        return m_oFindDataA.cFileName;
    }
    const wchar_t * GetFileNameS(wchar_t) const {
        return m_oFindDataW.cFileName;
    }

    bool IsDirS(char) const {
        return GetFileTypeS(m_oFindDataA.dwFileAttributes) == SG_FILETYPE_DIR;
    }
    bool IsDirS(wchar_t) const {
        return GetFileTypeS(m_oFindDataW.dwFileAttributes) == SG_FILETYPE_DIR;
    }

    SG_FileType GetFileTypeS(const char * a_pszPath) {
        return GetFileTypeS(GetFileAttributesA(a_pszPath));
    }
    SG_FileType GetFileTypeS(const wchar_t * a_pszPath)  {
        return GetFileTypeS(GetFileAttributesW(a_pszPath));
    }
    SG_FileType GetFileTypeS(DWORD a_dwAttribs) const {
        if (a_dwAttribs == INVALID_FILE_ATTRIBUTES) {
            return SG_FILETYPE_INVALID;
        }
        if (a_dwAttribs & FILE_ATTRIBUTE_DIRECTORY) {
            return SG_FILETYPE_DIR;
        }
        return SG_FILETYPE_FILE;
    }

    static int fileSortCompare(const void *a1, const void *a2) {
        const SOCHAR *s1 = *(const SOCHAR **)a1;
        const SOCHAR *s2 = *(const SOCHAR **)a2;
        return CSimpleGlobUtil::stricmp(s1, s2);
    }

private:
    HANDLE              m_hFind;
    WIN32_FIND_DATAA    m_oFindDataA;
    WIN32_FIND_DATAW    m_oFindDataW;
};

#endif // _WIN32

// ----------------------------------------------------------------------------
// Unix implementation

#ifndef _WIN32

struct SimpleGlobImplUnix
{
    SimpleGlobImplUnix()
        : m_pDir(NULL)
        , m_pDirEnt(NULL)
    { }

    bool FindFirstFileS(const char * a_pszFileSpec) {
        m_pDir = opendir(a_pszFileSpec);
        if (!m_pDir) return false;
        m_pDirEnt = readdir(m_pDir);
        return m_pDirEnt != NULL;
    }

    bool FindNextFileS(char) {
        m_pDirEnt = readdir(m_pDir);
        return m_pDirEnt != NULL;
    }

    void FindDone() {
        closedir(m_pDir);
        m_pDir = NULL;
        m_pDirEnt = NULL;
    }

    const char * GetFileNameS(char) const {
        return m_pDirEnt->d_name;
    }

    bool IsDirS(char) const {
#if 0 // if we have dirent.d_type
        return m_pDirEnt->d_type == DT_DIR;
#else
        return GetFileTypeS(m_pDirEnt->d_name) == SG_FILETYPE_DIR;
#endif
    }

    SG_FileType GetFileTypeS(const char * a_pszPath) const {
        struct stat sb;
        if (0 != stat(a_pszPath, &sb)) {
            return SG_FILETYPE_INVALID;
        }
        if (S_ISDIR(sb.st_mode)) {
            return SG_FILETYPE_DIR;
        }
        if (S_ISREG(sb.st_mode)) {
            return SG_FILETYPE_FILE;
        }
        return SG_FILETYPE_INVALID;
    }

    static int fileSortCompare(const void *a1, const void *a2) {
        const char *s1 = *(const char **)a1;
        const char *s2 = *(const char **)a2;
        return CSimpleGlobUtil::stricmp(s1, s2);
    }

private:
    DIR *           m_pDir;
    struct dirent * m_pDirEnt;
};

#endif // _WIN32

// ----------------------------------------------------------------------------
// Main Template

#define SG_ARGV_INITIAL_SIZE       32
#define SG_BUFFER_INITIAL_SIZE     1024

template<class SOCHAR>
class CSimpleGlobTempl
#ifdef _WIN32
    : private SimpleGlobImplWin<SOCHAR>
#else
    : private SimpleGlobImplUnix
#endif
{
public:
    // see Init() for details
    CSimpleGlobTempl(unsigned int a_uiFlags = 0, int a_nReservedSlots = 0) {
        m_uiFlags           = 0;
        m_nReservedSlots    = 0;
        m_bArgsIsIndex      = true;
        m_nArgsLen          = 0;
        m_nArgsSize         = 0;
        m_rgpArgs           = 0;
        m_uiBufferLen       = 0;
        m_uiBufferSize      = 0;
        m_pBuffer           = 0;

        Init(a_uiFlags, a_nReservedSlots);
    }

    ~CSimpleGlobTempl() {
        if (m_rgpArgs) free(m_rgpArgs);
        if (m_pBuffer) free(m_pBuffer);
    }

    // a_uiFlags            combination of the SG_GLOB flags above
    // a_nReservedSlots     number of empty slots that should be reserved in the returned argv array
    inline void Init(unsigned int a_uiFlags = 0, int a_nReservedSlots = 0) {
        m_uiFlags           = a_uiFlags;
        m_nReservedSlots    = a_nReservedSlots;
        m_nArgsLen          = a_nReservedSlots;
        m_uiBufferLen       = 0;
    }

    // a_pszFileSpec        filespec to parse and add files for
    inline int Add(const SOCHAR *a_pszFileSpec) {
        if (!m_bArgsIsIndex) {
            SwapArgsType();
        }

        // if this doesn't contain wildcards then we can just add it directly
        if (!CSimpleGlobUtil::strchr(a_pszFileSpec, '*') &&
            !CSimpleGlobUtil::strchr(a_pszFileSpec, '?'))
        {
            SG_FileType nType = GetFileTypeS(a_pszFileSpec);
            if (nType == SG_FILETYPE_INVALID) {
                if (m_uiFlags & SG_GLOB_NOCHECK) {
                    return AppendName(a_pszFileSpec, false);
                }
                return SG_ERR_FAILURE;
            }
            return AppendName(a_pszFileSpec, nType == SG_FILETYPE_DIR);
        }

        // search for the first match on the file
        if (!FindFirstFileS(a_pszFileSpec)) {
            if (m_uiFlags & SG_GLOB_NOCHECK) {
                return AppendName(a_pszFileSpec, false);
            }
            return SG_ERR_FAILURE;
        }

        // add it and find all subsequent matches
        int nError, nStartLen = m_nArgsLen;
        bool bSuccess;
        do {
            nError = AppendName(GetFileNameS((SOCHAR)0), IsDirS((SOCHAR)0));
            bSuccess = FindNextFileS((SOCHAR)0);
        }
        while (nError == SG_SUCCESS && bSuccess);
        FindDone();

        // sort if not disabled
        if (m_nArgsLen > (nStartLen + 1) && !(m_uiFlags & SG_GLOB_NOSORT)) {
            SwapArgsType();
            qsort(
                m_rgpArgs + nStartLen,
                m_nArgsLen - nStartLen,
                sizeof(m_rgpArgs[0]),
                fileSortCompare);
        }

        return nError;
    }

    // add an array of filespecs
    int Add(int a_nCount, const SOCHAR * const * a_rgpszFileSpec) {
        int nResult;
        for (int n = 0; n < a_nCount; ++n) {
            nResult = Add(a_rgpszFileSpec[n]);
            if (nResult != SG_SUCCESS) {
                return nResult;
            }
        }
        return SG_SUCCESS;
    }

    inline int FileCount() const {
        return m_nArgsLen;
    }

    SOCHAR ** Files() {
        if (m_bArgsIsIndex) {
            SwapArgsType();
        }
        return m_rgpArgs;
    }

    inline SOCHAR * File(int n) {
        return Files()[n];
    }

private:
    int AppendName(const SOCHAR *a_pszFileName, bool a_bIsDir) {
        // check for special cases which cause us to ignore this entry
        if ((m_uiFlags & SG_GLOB_ONLYDIR) && !a_bIsDir) {
            return SG_SUCCESS;
        }
        if ((m_uiFlags & SG_GLOB_ONLYFILE) && a_bIsDir) {
            return SG_SUCCESS;
        }
        if ((m_uiFlags & SG_GLOB_NODOT) && a_bIsDir) {
            if (a_pszFileName[0] == '.') {
                if (a_pszFileName[1] == '\0') {
                    return SG_SUCCESS;
                }
                if (a_pszFileName[1] == '.' && a_pszFileName[2] == '\0') {
                    return SG_SUCCESS;
                }
            }
        }

        // ensure that we have enough room in the argv array
        if (m_nArgsLen + 1 >= m_nArgsSize) {
            int nNewSize = (m_nArgsSize > 0) ? m_nArgsSize * 2 : SG_ARGV_INITIAL_SIZE;
            while (m_nArgsLen + 1 >= nNewSize) {
                nNewSize *= 2;
            }
            void * pNewBuffer = realloc(m_rgpArgs, nNewSize * sizeof(SOCHAR*));
            if (!pNewBuffer) {
                return SG_ERR_MEMORY;
            }
            m_nArgsSize = nNewSize;
            m_rgpArgs = (SOCHAR**) pNewBuffer;
        }

        // ensure that we have enough room in the string buffer
        size_t uiLen = CSimpleGlobUtil::strlen(a_pszFileName) + 1; // len in characters + null
        if (a_bIsDir && (m_uiFlags & SG_GLOB_MARK) == SG_GLOB_MARK) {
            ++uiLen;    // need space for the backslash
        }
        if (m_uiBufferLen + uiLen >= m_uiBufferSize) {
            size_t uiNewSize = (m_uiBufferSize > 0) ? m_uiBufferSize * 2 : SG_BUFFER_INITIAL_SIZE;
            while (m_uiBufferLen + uiLen >= uiNewSize) {
                uiNewSize *= 2;
            }
            void * pNewBuffer = realloc(m_pBuffer, uiNewSize * sizeof(SOCHAR));
            if (!pNewBuffer) {
                return SG_ERR_MEMORY;
            }
            m_uiBufferSize = uiNewSize;
            m_pBuffer = (SOCHAR*) pNewBuffer;
        }

        // add this entry
        m_rgpArgs[m_nArgsLen++] = (SOCHAR*)m_uiBufferLen;    // offset from beginning of buffer
        CSimpleGlobUtil::strcpy_s(m_pBuffer + m_uiBufferLen,
            m_uiBufferSize - m_uiBufferLen, a_pszFileName);
        m_uiBufferLen += uiLen;

        // add the backslash if desired
        if (a_bIsDir && (m_uiFlags & SG_GLOB_MARK) == SG_GLOB_MARK) {
            SOCHAR szBackslash[] = { '\\', '\0' };
            CSimpleGlobUtil::strcpy_s(m_pBuffer + m_uiBufferLen - 2,
                m_uiBufferSize - (m_uiBufferLen - 2), szBackslash);
        }

        return SG_SUCCESS;
    }

    void SwapArgsType() {
        if (m_bArgsIsIndex) {
            for (int n = 0; n < m_nArgsLen; ++n) {
                m_rgpArgs[n] = (SOCHAR*) (m_pBuffer + (size_t) m_rgpArgs[n]);
            }
        }
        else {
            for (int n = 0; n < m_nArgsLen; ++n) {
                m_rgpArgs[n] = (SOCHAR*) (m_rgpArgs[n] - m_pBuffer);
            }
        }
        m_bArgsIsIndex = !m_bArgsIsIndex;
    }

private:
    unsigned int        m_uiFlags;
    int                 m_nReservedSlots;   // number of argv slots reserved for the client
    bool                m_bArgsIsIndex;     // is the argv array storing indexes or pointers
    SOCHAR **           m_rgpArgs;          // argv array
    int                 m_nArgsSize;        // allocated size of array
    int                 m_nArgsLen;         // used length
    SOCHAR *            m_pBuffer;          // argv string buffer
    size_t              m_uiBufferSize;     // allocated size of buffer
    size_t              m_uiBufferLen;      // used length of buffer
};

// we supply both ASCII and WIDE char versions, plus a
// SOCHAR style that changes depending on the build setting
typedef CSimpleGlobTempl<char>    CSimpleGlobA;
typedef CSimpleGlobTempl<wchar_t> CSimpleGlobW;
#if defined(_UNICODE)
# define CSimpleGlob CSimpleGlobW
#else
# define CSimpleGlob CSimpleGlobA
#endif

#endif // INCLUDED_SimpleGlob
