// File:    SimpleIni.h
// Library: SimpleIni
// Author:  Brodie Thiesfield <brofield1@jellycan.com>
// Source:  http://code.jellycan.com/SimpleIni/
// Version: 1.5
//
// This component allows an INI-style configuration file to be used on both
// Windows and Linux/Unix. It is fast, simple and source code using this
// component will compile unchanged on either OS.
//
// FEATURES
// ========
//  * public domain, free use in all software (including GPL and commercial)
//  * multi-platform (Windows 95/98/ME/NT/2K/XP/2003, Linux, Unix)
//  * loading and saving of INI-style configuration files
//  * liberal acceptance of file format
//      - key/values with no section
//      - removal of whitespace around sections, keys and values
//  * optional case-insensitive sections and keys (for ASCII characters only)
//  * supports both char or wchar_t programming interfaces
//  * supports both MBCS (system locale) and UTF-8 file encodings
//  * system locale does not need to be UTF-8 on Linux/Unix to load UTF-8 file
//  * support for non-ASCII characters in section, keys, values and comments
//  * support for non-standard character types or file encodings
//    via user-written converter classes
//  * support for adding/modifying values programmatically
//
// USAGE SUMMARY
// =============
//  1. Declare an instance the appropriate class. Note that the following
//     definitions are just shortcuts for commonly used types. Other types
//     (e.g. PRUnichar, unsigned short, unsigned char) are also possible.
//
//      Interface   Case-sensitive      Typedef
//      ---------   --------------      ---------------
//      char        No                  CSimpleIniA
//      char        Yes                 CSimpleIniCaseA
//      wchar_t     No                  CSimpleIniW
//      wchar_t     Yes                 CSimpleIniCaseW
//
//  2. Call LoadFile() to load and parse the INI configuration file
//
//  3. Access the data of the file using the following functions
//
//      GetAllSections  Return all section names
//      GetAllKeys      Return all key names for a section
//      GetSection      Return all key names and values in a section
//      GetSectionSize  Return the number of keys in a section
//      GetValue        Return a value for a section & key
//      SetValue        Add or update a value for a section & key
//
//  4. Call SaveFile() to save the INI configuration file (if necessary)
//
// NOTES
// =====
//  * The collation (sorting) order used for sections and keys returned from
//    iterators is NOT DEFINED. If collation order of the text is important
//    then it should be done yourself by either supplying a replacement
//    SI_STRCMP class, or by sorting the strings external to this library.
//  * Linux/Unix systems must also compile and link ConvertUTF.c  
//  * To load a UTF-8 file on Windows and expose it with SI_CHAR == char,
//    you need to define SI_USING_WIN32_CHAR_FOR_UTF8
//
// DISCLAIMER
// ==========
//  This code is released as public domain, you can do with it whatever you
//  like: use it, modify it, distribute it, sell it, delete it, or send
//  it to your mother-in-law.  I make no promises or guarantees that this
//  code will work correctly or at all. Use it completely at your own risk.

#ifndef INCLUDED_SimpleIni_h
#define INCLUDED_SimpleIni_h

#ifdef _WIN32
# include <windows.h>
# include <mbstring.h>
#else // !_WIN32
# include "ConvertUTF.h"
#endif // _WIN32

#include <map>
#include <string>
#include <vector>

#include <stdio.h>
#include <errno.h>

// ---------------------------------------------------------------------------
//                                  HELPER CLASSES
// ---------------------------------------------------------------------------

#ifdef _WIN32
# define SI_EOL          '\n'
# define SI_NEWLINE      "\r\n"
# define SI_NEWLINE_W    L"\r\n"
#else // !_WIN32
# define SI_EOL          '\n'
# define SI_NEWLINE      "\n"
# define SI_NEWLINE_W    L"\n"
#endif // _WIN32

/**
 * Generic case-sensitive <less> comparison
 *
 * This class returns numerically ordered ASCII case-sensitive text for all
 * possible sizes and types of SI_CHAR.
 */
template<class SI_CHAR>
struct SI_Case {
	bool operator()(const SI_CHAR * pLeft, const SI_CHAR * pRight) const {
        long cmp;
        for ( ;*pLeft && *pRight; ++pLeft, ++pRight) {
            cmp = (long) *pLeft - (long) *pRight;
            if (cmp != 0) {
                return cmp < 0;
            }
        }
        return *pRight != 0;
    }
};

/**
 * Generic ASCII case-insensitive <less> comparison
 *
 * This class returns numerically ordered ASCII case-insensitive text for all
 * possible sizes and types of SI_CHAR. It is not safe for MBCS text
 * comparison where ASCII A-Z characters are used in the encoding of
 * multi-byte characters.
 */
template<class SI_CHAR>
struct SI_NoCase {
    inline SI_CHAR locase(SI_CHAR ch) const {
        return (ch < 'A' || ch > 'Z') ? ch : (ch - 'A' + 'a');
    }
	bool operator()(const SI_CHAR * pLeft, const SI_CHAR * pRight) const {
        long cmp;
        for ( ;*pLeft && *pRight; ++pLeft, ++pRight) {
            cmp = (long) locase(*pLeft) - (long) locase(*pRight);
            if (cmp != 0) {
                return cmp < 0;
            }
        }
        return *pRight != 0;
    }
};

#ifdef _WIN32
/**
 * Case-insensitive comparison class using Win32 MBCS functions
 *
 * This class returns a case-insensitive semi-collation order for
 * MBCS text. It is not safe for UTF-8 text returned in char format!
 * If you are using SI_CHAR == char and UTF-8 text then the SI_NoCase
 * class is the one you need.
 */
template<class SI_CHAR>
struct SI_WindowsNoCase {
	bool operator()(const SI_CHAR * pLeft, const SI_CHAR * pRight) const {
        if (sizeof(SI_CHAR) == sizeof(char)) {
            return _mbsicmp((const unsigned char *)pLeft,
                (const unsigned char *)pRight) < 0;
        }
        if (sizeof(SI_CHAR) == sizeof(wchar_t)) {
            return wcsicmp((const wchar_t *)pLeft,
                (const wchar_t *)pRight) < 0;
        }
        return SI_NoCase<SI_CHAR>()(pLeft, pRight);
    }
};
#endif // _WIN32

/**
 * Null conversion class for MBCS/UTF-8 to char (or equivalent)
 */
template<class SI_CHAR>
class SI_ConvertA {
public:
    SI_ConvertA(bool /*a_bInputIsUtf8*/) { }

    // Return the number of SI_CHAR for the converted data, -1 on error
    int SizeFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen)
    {
        // ascii/UTF-8 needs no conversion and uses the same number of chars
        return (int) a_uInputDataLen;
    }

    // Convert the input data into SI_CHAR in the output buffer.
    // Length of the output buffer is supplied as a_nOutputDataLen.
    // Return true for success, false for error
    bool ConvertFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen,
        SI_CHAR *       a_pOutputData,
        size_t          a_uOutputDataLen)
    {
        // ascii/UTF-8 needs no conversion
        memcpy(a_pOutputData, a_pInputData, a_uOutputDataLen);
        return true;
    }

    // Return the number of char for the converted data, -1 on error
    int SizeToStore(
        const SI_CHAR * a_pInputData,
        size_t          a_uInputDataLen)
    {
        // ascii/UTF-8 needs no conversion and uses the same number of chars
        if (a_uInputDataLen == -1) {
            a_uInputDataLen = 0;
            while (a_pInputData[a_uInputDataLen++]) /*loop*/ ;
        }
        return (int) a_uInputDataLen;
    }

    // Convert the input data into SI_CHAR in the output buffer.
    // Length of the output buffer is supplied as a_nOutputDataLen.
    // Return true for success, false for error
    bool ConvertToStore(
        const SI_CHAR * a_pInputData,
        size_t          a_uInputDataLen,
        char *          a_pOutputData,
        size_t          a_uOutputDataLen)
    {
        // ascii/UTF-8 needs no conversion
        memcpy(a_pOutputData, a_pInputData, a_uOutputDataLen);
        return true;
    }
};

#ifdef _WIN32
/**
 * Converts MBCS and UTF-8 to a wchar_t (or equivalent) on Windows
 */
template<class SI_CHAR>
class SI_ConvertW {
    UINT m_uCodePage;
public:
    SI_ConvertW(bool a_bInputIsUtf8)
    {
        m_uCodePage = a_bInputIsUtf8 ? CP_UTF8 : CP_ACP;
    }

    // Return the number of SI_CHAR for the converted data, -1 on error
    int SizeFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen)
    {
        int retval = MultiByteToWideChar(
            m_uCodePage, MB_ERR_INVALID_CHARS,
            a_pInputData, (int) a_uInputDataLen,
            0, 0);
        return retval > 0 ? retval : -1;
    }

    // Convert the input data into SI_CHAR in the output buffer.
    // Length of the output buffer is supplied as a_nOutputDataLen.
    // Return true for success, false for error
    bool ConvertFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen,
        SI_CHAR *       a_pOutputData,
        size_t          a_uOutputDataLen)
    {
        int nSize = MultiByteToWideChar(
            m_uCodePage, MB_ERR_INVALID_CHARS,
            a_pInputData, (int) a_uInputDataLen,
            (wchar_t *) a_pOutputData, (int) a_uOutputDataLen);
        return (nSize > 0);
    }

    // Return the number of char for the converted data, -1 on error
    int SizeToStore(
        const SI_CHAR * a_pInputData,
        size_t          a_uInputDataLen)
    {
        int retval = WideCharToMultiByte(
            m_uCodePage, 0,
            (const wchar_t *) a_pInputData, (int) a_uInputDataLen,
            0, 0, 0, 0);
        return retval > 0 ? retval : -1;
    }

    // Convert the input data into char in the output buffer.
    // Length of the output buffer is supplied as a_nOutputDataLen.
    // Return true for success, false for error
    bool ConvertToStore(
        const SI_CHAR * a_pInputData,
        size_t          a_uInputDataLen,
        char *          a_pOutputData,
        size_t          a_uOutputDataLen)
    {
        int nSize = WideCharToMultiByte(
            m_uCodePage, 0,
            (const wchar_t *) a_pInputData, (int) a_uInputDataLen,
            a_pOutputData, (int) a_uOutputDataLen, 0, 0);
        return (nSize > 0);
    }
};
#else // !_WIN32
/**
 * Converts UTF-8 to a wchar_t (or equivalent) on Linux/Unix
 */
template<class SI_CHAR>
class SI_ConvertW {
    bool m_bInputIsUtf8;
public:
    SI_ConvertW(bool a_bInputIsUtf8) : m_bInputIsUtf8(a_bInputIsUtf8) { }

    // Return the number of SI_CHAR for the converted data, -1 on error
    int SizeFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen)
    {
        if (m_bInputIsUtf8) {
            // worst case scenario for UTF-8 to wchar_t is 1 char -> 1 wchar_t
            // so we just return the same number of characters required as for
            // the source text.
            return (int) a_uInputDataLen;
        }
        else {
            size_t retval = mbstowcs(NULL, a_pInputData, a_uInputDataLen);
            return retval != (size_t) -1;
        }
    }

    // Convert the input data into SI_CHAR in the output buffer.
    // Length of the output buffer is supplied as a_nOutputDataLen.
    // Return true for success, false for error
    bool ConvertFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen,
        SI_CHAR *       a_pOutputData,
        size_t          a_uOutputDataLen)
    {
        if (m_bInputIsUtf8) {
            // This uses the Unicode reference implementation to do the
            // conversion from UTF-8 to wchar_t. The required files are
            // ConvertUTF.h and ConvertUTF.c which should be included in
            // the distribution but are publically available from unicode.org
            // at http://www.unicode.org/Public/PROGRAMS/CVTUTF/
            ConversionResult retval;
            const UTF8 * pUtf8 = (const UTF8 *) a_pInputData;
            if (sizeof(wchar_t) == sizeof(UTF32)) {
                UTF32 * pUtf32 = (UTF32 *) a_pOutputData;
                retval = ConvertUTF8toUTF32(
                    &pUtf8, pUtf8 + a_uInputDataLen,
                    &pUtf32, pUtf32 + a_uOutputDataLen,
                    lenientConversion);
            }
            else if (sizeof(wchar_t) == sizeof(UTF16)) {
                UTF16 * pUtf16 = (UTF16 *) a_pOutputData;
                retval = ConvertUTF8toUTF16(
                    &pUtf8, pUtf8 + a_uInputDataLen,
                    &pUtf16, pUtf16 + a_uOutputDataLen,
                    lenientConversion);
            }
            return retval == conversionOK;
        }
        else {
            size_t retval = mbstowcs(a_pOutputData,
                a_pInputData, a_uOutputDataLen);
            return retval != (size_t) -1;
        }
    }

    // Return the number of char for the converted data, -1 on error
    int SizeToStore(
        const SI_CHAR * a_pInputData,
        size_t          a_uInputDataLen)
    {
        if (m_bInputIsUtf8) {
            // worst case scenario for wchar_t to UTF-8 is 1 wchar_t -> 6 char 
            if (a_uInputDataLen == -1) {
                a_uInputDataLen = 0;
                while (a_pInputData[a_uInputDataLen++]) /*loop*/ ;
            }
            return (int) 6 * a_uInputDataLen;
        }
        else {
            size_t retval = wcstombs(NULL, a_pInputData, a_uInputDataLen);
            return retval != (size_t) -1;
        }
    }

    // Convert the input data into char in the output buffer.
    // Length of the output buffer is supplied as a_nOutputDataLen.
    // Return true for success, false for error
    bool ConvertToStore(
        const SI_CHAR * a_pInputData,
        size_t          a_uInputDataLen,
        char *          a_pOutputData,
        size_t          a_uOutputDataLen)
    {
        if (m_bInputIsUtf8) {
            // This uses the Unicode reference implementation to do the
            // conversion from wchar_t to UTF-8. The required files are
            // ConvertUTF.h and ConvertUTF.c which should be included in
            // the distribution but are publically available from unicode.org
            // at http://www.unicode.org/Public/PROGRAMS/CVTUTF/
            ConversionResult retval;
            UTF8 * pUtf8 = (UTF8 *) a_pOutputData;
            if (sizeof(wchar_t) == sizeof(UTF32)) {
                const UTF32 * pUtf32 = (const UTF32 *) a_pInputData;
                retval = ConvertUTF32toUTF8(
                    &pUtf32, pUtf32 + a_uInputDataLen,
                    &pUtf8, pUtf8 + a_uOutputDataLen,
                    lenientConversion);
            }
            else if (sizeof(wchar_t) == sizeof(UTF16)) {
                const UTF16 * pUtf16 = (const UTF16 *) a_pInputData;
                retval = ConvertUTF16toUTF8(
                    &pUtf16, pUtf16 + a_uInputDataLen,
                    &pUtf8, pUtf8 + a_uOutputDataLen,
                    lenientConversion);
            }
            return retval == conversionOK;
        }
        else {
            size_t retval = wcstombs(a_pOutputData,
                a_pInputData, a_uOutputDataLen);
            return retval != (size_t) -1;
        }
    }
};
#endif // _WIN32

// ---------------------------------------------------------------------------
//                              MAIN TEMPLATE CLASS
// ---------------------------------------------------------------------------

/**
 * Simple INI file reader
 *
 * This can be instantiated with the choice of unicode or native characterset,
 * and case sensitive or insensitive comparisons of section and key names.
 * The supported combinations are pre-defined with the following typedefs:
 *
 *  Interface   Case-sensitive      Typedef
 *  ---------   --------------      ---------------
 *  char        No                  CSimpleIniA
 *  char        Yes                 CSimpleIniCaseA
 *  wchar_t     No                  CSimpleIniW
 *  wchar_t     Yes                 CSimpleIniCaseW
 *
 * Note that using other types for the SI_CHAR is supported. For instance,
 * unsigned char, unsigned short, etc. Note that where the alternative type
 * is a different size to char/wchar_t you may need to supply new helper
 * classes for SI_STRCMP and SI_CONVERTER.
 */
template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
class CSimpleIniTempl
{
public:
    typedef SI_CHAR char_type;
    typedef std::map<const SI_CHAR *,const SI_CHAR *,SI_STRCMP> TKeyVal;
    typedef std::map<const SI_CHAR *,TKeyVal,SI_STRCMP>         TSection;
    typedef std::vector<const SI_CHAR *>                        TNames;

public:
    /**
     * Default constructor
     */
    CSimpleIniTempl();

    /**
     * Destructor
     */
    ~CSimpleIniTempl();

    /**
     * Deallocate all memory stored by this object
     */
    void Reset();

    /**
     * Load an INI file from disk into memory
     *
     * \param a_pszFile     Path of the file to be loaded. This will be passed
     *                      to fopen() and so must be a valid path for the
     *                      current platform.
     * \param a_bIsUtf8     The file encoding is normally assumed to be the
     *                      OS native file encoding. This is the system locale
     *                      on Linux/Unix and the legacy MBCS encoding on
     *                      Windows NT/2K/XP/etc. If this flag is set to true
     *                      then the file will be loaded as a UTF-8 encoded
     *                      file regardless of the native file encoding. If
     *                      SI_CHAR == char then char* accepted and returned
     *                      is in UTF-8 format.
     *
     * \retval 0            Success, no error
     * \retval errno        Error codes from errno
     */
    int LoadFile(
        const char *    a_pszFile,
        bool            a_bIsUtf8 = false);

#ifdef _WIN32
    /**
     * Load an INI file from disk into memory
     *
     * \param a_pwszFile    Path of the file to be loaded in UTF-16. This will
     *                      be passed to _wfopen() on Windows. There is no
     *                      wchar_t fopen function on Linux/Unix so this
     *                      function is not supported there.
     * \param a_bIsUtf8     The file encoding is normally assumed to be the
     *                      OS native file encoding. This is the system locale
     *                      on Linux/Unix and the legacy MBCS encoding on
     *                      Windows NT/2K/XP/etc. If this flag is set to true
     *                      then the file will be loaded as a UTF-8 encoded
     *                      file regardless of the native file encoding. If
     *                      SI_CHAR == char then char* accepted and returned
     *                      is in UTF-8 format.
     *
     * \retval 0            Success, no error
     * \retval errno        Error codes from errno
     */
    int LoadFile(
        const wchar_t * a_pwszFile,
        bool            a_bIsUtf8 = false);
#endif // _WIN32

    /**
     * Load INI file data direct from memory
     *
     * \param a_pData       Data to be loaded
     * \param a_uDataLen    Length of the data in bytes
     * \param a_bIsUtf8     The file encoding is normally assumed to be the
     *                      OS native file encoding. This is the system locale
     *                      on Linux/Unix and the legacy MBCS encoding on
     *                      Windows NT/2K/XP/etc. If this flag is set to true
     *                      then the file will be loaded as a UTF-8 encoded
     *                      file regardless of the native file encoding. If
     *                      SI_CHAR == char then char* accepted and returned
     *                      is in UTF-8 format.
     *
     * \retval 0            Success, no error
     * \retval errno        Error codes from errno
     */
    int LoadFile(
        const char *    a_pData,
        size_t          a_uDataLen,
        bool            a_bIsUtf8 = false);

    /**
     * Save the INI file data to a file 
     *
     * The data will be written to the output file in a format appropriate to
     * the current data. That is:
     *
     *      SI_CHAR     FORMAT
     *      char        same format as loaded (MBCS or UTF-8)
     *      wchar_t     UTF-8 (no BOM)
     *      other       UTF-8 (no BOM)
     *
     * Note that comments, etc from the original file are not saved. Only the 
     * valid data contents are rewritten back to the new file. If any data is
     * written before or after the data written by this function, then it 
     * should be in the same format as the file is written.
     * 
     * \param a_pFile       Handle to a file. File should be opened for 
     *                      binary output.
     * \param a_pHeader     Header data to write to the file before the data.
     *                      When SI_CHAR is char, this must be the same format
     *                      as in the table above.
     *
     * \retval 0            Success, no error
     * \retval errno        Error codes from errno
     */
    int SaveFile(
        FILE *          a_pFile, 
        const SI_CHAR * a_pHeader = 0) const;

    /**
     * Retrieve the value for a specific key
     *
     * \param a_pSection        Section to search
     * \param a_pKey            Key to search for
     * \param a_pDefault        Value to return if the key is not found
     *
     * \retval a_pDefault       Key was not found in the section
     * \retval other            Value of the key
     */
    const SI_CHAR * GetValue(
        const SI_CHAR * a_pSection,
        const SI_CHAR * a_pKey,
        const SI_CHAR * a_pDefault = 0 ) const;

    /**
     * Add or update a section or value
     *
     * Empty sections may be added by setting a_pKey and a_pValue to NULL.
     *
     * \param a_pSection        Section to add or update
     * \param a_pKey            Key to add or update
     * \param a_pValue          Value to set
     *
     * \retval 0                Failed to add/update the value
     * \retval 1                Value was updated
     * \retval 2                Value was inserted
     */
    int SetValue(
        const SI_CHAR * a_pSection,
        const SI_CHAR * a_pKey,
        const SI_CHAR * a_pValue)
    {
        return AddEntry(a_pSection, a_pKey, a_pValue, true);
    }

    /**
     * Query the number of entries in a specific section
     *
     * \param a_pSection        Section to request data for
     *
     * \retval -1               Section does not exist in the file
     * \retval >=0              Number of keys in the section
     */
    int GetSectionSize(
        const SI_CHAR * a_pSection ) const;

    /**
     * Retrieve all key and value pairs for a section
     *
     * The data is returned as a pointer to an STL map and can be
     * iterated or searched as necessary.
     *
     * \param a_pSection        Name of the section to return
     * \param a_pData           Pointer to the section data.
     * \retval boolean          Was a section matching the supplied
     *                          name found.
     */
    const TKeyVal * GetSection(
        const SI_CHAR * a_pSection) const
    {
        if (a_pSection) {
            TSection::const_iterator i = m_data.find(a_pSection);
            if (i != m_data.end()) {
                return &(i->second);
            }
        }
        return 0;
    }

    /**
     * Retrieve all section names
     *
     * The list is returned as an STL vector of names and can be iterated or
     * searched as necessary. Note that the collation order of the returned
     * strings is NOT DEFINED.
     *
     * \param a_names           Vector of strings that will receive all
     *                          of the section names.
     */
    void GetAllSections(
        TNames & a_names ) const;

    /**
     * Retrieve all key names in a section
     *
     * The list is returned as an STL vector of names and can be iterated or
     * searched as necessary. Note that the collation order of the returned
     * strings is NOT DEFINED.
     *
     * \param a_names           Vector of strings that will receive all
     *                          of the key names.
     */
    void GetAllKeys(
        const SI_CHAR * a_pSection,
        TNames &        a_names ) const;

private:
    int LoadFile(FILE * a_fpFile, bool a_bIsUtf8);
    bool FindEntry(
        SI_CHAR *&  a_pData,
        SI_CHAR *&  a_pSection,
        SI_CHAR *&  a_pKey,
        SI_CHAR *&  a_pVal ) const;
    int AddEntry(
        const SI_CHAR * a_pSection,
        const SI_CHAR * a_pKey,
        const SI_CHAR * a_pValue,
        bool            a_bCopyStrings);
    inline bool IsSpace(SI_CHAR ch) const {
        return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
    }
    inline bool IsComment(SI_CHAR ch) const {
        return (ch == ';' || ch == '#');
    }
    bool Copy(const SI_CHAR *& a_pString);

private:
    SI_CHAR *   m_pData;
    TSection    m_data;
    TNames      m_strings;
};

// ---------------------------------------------------------------------------
//                                  IMPLEMENTATION
// ---------------------------------------------------------------------------

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::CSimpleIniTempl()
    : m_pData(0)
{ }

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::~CSimpleIniTempl()
{
    Reset();
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
void
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::Reset()
{
    // remove all data
    delete[] m_pData;
    m_pData = 0;
    if (!m_data.empty()) {
        m_data.erase(m_data.begin(), m_data.end());
    }

    // remove all strings
    if (!m_strings.empty()) {
        TNames::iterator i = m_strings.begin();
        for (; i != m_strings.end(); ++i) {
            delete[] *i;
        }
        m_strings.erase(m_strings.begin(), m_strings.end());
    }
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
int
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::LoadFile(
    const char *    a_pszFile,
    bool            a_bIsUtf8)
{
    FILE * fp = fopen(a_pszFile, "rb");
    if (!fp) {
        return errno;
    }
    int retval = LoadFile(fp, a_bIsUtf8);
    fclose(fp);
    return retval;
}

#ifdef _WIN32
template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
int
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::LoadFile(
    const wchar_t * a_pwszFile,
    bool            a_bIsUtf8)
{
    FILE * fp = _wfopen(a_pwszFile, L"rb");
    if (!fp) {
        return errno;
    }
    int retval = LoadFile(fp, a_bIsUtf8);
    fclose(fp);
    return retval;
}
#endif // _WIN32

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
int
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::LoadFile(
    FILE *  a_fpFile,
    bool    a_bIsUtf8)
{
    // load the raw file data
    int retval = fseek(a_fpFile, 0, SEEK_END);
    if (retval != 0) {
        return errno;
    }
    long lSize = ftell(a_fpFile);
    if (lSize < 0) {
        return errno;
    }
    char * pData = new char[lSize];
    fseek(a_fpFile, 0, SEEK_SET);
    size_t uRead = fread(pData, sizeof(char), lSize, a_fpFile);
    if (uRead != (size_t) lSize) {
        delete[] pData;
        return -1; //TODO: real error here
    }

    // convert the raw data to unicode
    retval = LoadFile(pData, uRead, a_bIsUtf8);
    delete[] pData;

    return retval;
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
int
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::LoadFile(
    const char *    a_pData,
    size_t          a_uDataLen,
    bool            a_bIsUtf8)
{
    SI_CONVERTER converter(a_bIsUtf8);

    // determine the length of the converted data
    int nLen = converter.SizeFromStore(a_pData, a_uDataLen);
    if (nLen == -1) {
        return EINVAL;
    }

    // allocate memory for the data, ensure that there is a NULL
    // terminator wherever the converted data ends
    SI_CHAR * pData = new SI_CHAR[nLen+1];
    if (!pData) {
        return ENOMEM;
    }
    memset(pData, 0, sizeof(SI_CHAR)*(nLen+1));

    // convert the data
    if (!converter.ConvertFromStore(a_pData, a_uDataLen, pData, nLen)) {
        delete[] pData;
        return EINVAL;
    }

    // store it and parse it
    delete[] m_pData;
    m_pData = pData;

    static SI_CHAR empty = 0;
    SI_CHAR * pWork = m_pData;
    SI_CHAR * pSection = &empty;
    SI_CHAR * pKey = 0;
    SI_CHAR * pVal = 0;

    // add every entry in the file to the data table
    while (FindEntry(pWork, pSection, pKey, pVal)) {
        AddEntry(pSection, pKey, pVal, false);
    }

    return 0;
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
bool
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::FindEntry(
    SI_CHAR *&  a_pData,
    SI_CHAR *&  a_pSection,
    SI_CHAR *&  a_pKey,
    SI_CHAR *&  a_pVal ) const
{
    SI_CHAR * pTrail;
    while (*a_pData) {
        // skip spaces and empty lines
        while (*a_pData && IsSpace(*a_pData)) {
            ++a_pData;
        }
        if (!*a_pData) {
            break;
        }

        // skip comment lines
        if (IsComment(*a_pData)) {
            while (*a_pData && *a_pData != SI_EOL) {
                ++a_pData;
            }
            continue;
        }

        // process section names
        if (*a_pData == '[') {
            // skip leading spaces
            ++a_pData;
            while (*a_pData && IsSpace(*a_pData)) {
                ++a_pData;
            }

            // find the end of the section name (it may contain spaces)
            // and convert it to lowercase as necessary
            a_pSection = a_pData;
            while (*a_pData && *a_pData != ']' && *a_pData != SI_EOL) {
                ++a_pData;
            }

            // if it's an invalid line, just skip it
            if (*a_pData != ']') {
                continue;
            }

            // remove trailing spaces from the section
            pTrail = a_pData - 1;
            while (pTrail >= a_pSection && IsSpace(*pTrail)) {
                --pTrail;
            }
            ++pTrail;
            *pTrail = 0;

            // skip to the end of the line
            ++a_pData;  // safe as checked that it == ']' above
            while (*a_pData && *a_pData != SI_EOL) {
                ++a_pData;
            }

            a_pKey = 0;
            a_pVal = 0;
            return true;
        }

        // find the end of the key name (it may contain spaces)
        // and convert it to lowercase as necessary
        a_pKey = a_pData;
        while (*a_pData && *a_pData != '=' && *a_pData != SI_EOL) {
            ++a_pData;
        }

        // if it's an invalid line, just skip it
        if (*a_pData != '=') {
            continue;
        }

        // empty keys are invalid
        if (a_pKey == a_pData) {
            while (*a_pData && *a_pData != SI_EOL) {
                ++a_pData;
            }
            continue;
        }

        // remove trailing spaces from the key
        pTrail = a_pData - 1;
        while (pTrail >= a_pKey && IsSpace(*pTrail)) {
            --pTrail;
        }
        ++pTrail;
        *pTrail = 0;

        // skip leading whitespace on the value
        ++a_pData;  // safe as checked that it == '=' above
        while (*a_pData && *a_pData != SI_EOL && IsSpace(*a_pData)) {
            ++a_pData;
        }

        // find the end of the value which is the end of this line
        a_pVal = a_pData;
        while (*a_pData && *a_pData != SI_EOL) {
            ++a_pData;
        }

        // remove trailing spaces from the value
        pTrail = a_pData - 1;
        if (*a_pData) { // prepare for the next round
            ++a_pData;
        }
        while (pTrail >= a_pVal && IsSpace(*pTrail)) {
            --pTrail;
        }
        ++pTrail;
        *pTrail = 0;

        // return the entry
        return true;
    }

    return false;
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
bool
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::Copy(
    const SI_CHAR *& a_pString)
{
    size_t uLen = 0;
    if (sizeof(SI_CHAR) == sizeof(char)) {
        uLen = strlen((const char *)a_pString);
    }
    else if (sizeof(SI_CHAR) == sizeof(wchar_t)) {
        uLen = wcslen((const wchar_t *)a_pString);
    }
    else {
        for ( ; a_pString[uLen]; ++uLen) /*loop*/ ;
    }
    ++uLen; // NULL character
    SI_CHAR * pCopy = new SI_CHAR[uLen];
    if (!pCopy) {
        return false;
    }
    memcpy(pCopy, a_pString, sizeof(SI_CHAR)*uLen);
    m_strings.push_back(pCopy);
    a_pString = pCopy;
    return true;
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
int
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::AddEntry(
    const SI_CHAR * a_pSection,
    const SI_CHAR * a_pKey,
    const SI_CHAR * a_pValue,
    bool            a_bCopyStrings)
{
    bool bInserted = false;

    // check for existence of the section first if we need string copies
    TSection::iterator iSection = m_data.end();
    if (a_bCopyStrings) {
        iSection = m_data.find(a_pSection);
        if (iSection == m_data.end()) {
            // if the section doesn't exist then we need a copy as the
            // string needs to last beyond the end of this function
            // because we will be inserting the section next
            if (!Copy(a_pSection)) {
                return 0;
            }
        }
    }

    // create the section entry
    if (iSection == m_data.end()) {
        std::pair<TSection::iterator,bool> i =
            m_data.insert( TSection::value_type( a_pSection, TKeyVal() ) );
        iSection = i.first;
        bInserted = true;
    }
    if (!a_pKey || !a_pValue) {
        // section only entries are specified with pKey and pVal as NULL
        return bInserted ? 2 : 1;
    }

    // check for existence of the key first if we need string copies
    TKeyVal & keyval = iSection->second;
    TKeyVal::iterator iKey = keyval.end();
    if (a_bCopyStrings) {
        iKey = keyval.find(a_pKey);
        if (iKey == keyval.end()) {
            // if the key doesn't exist then we need a copy as the
            // string needs to last beyond the end of this function
            // because we will be inserting the key next
            if (!Copy(a_pKey)) {
                return 0;
            }
        }

        // we always need a copy of the value
        if (!Copy(a_pValue)) {
            return 0;
        }
    }

    // create the key entry
    if (iKey == keyval.end()) {
        std::pair<TKeyVal::iterator,bool> i =
            keyval.insert( TKeyVal::value_type( a_pKey, 0 ) );
        iKey = i.first;
        bInserted = true;
    }
    iKey->second = a_pValue;
    return bInserted ? 2 : 1;
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
const SI_CHAR *
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::GetValue(
    const SI_CHAR * a_pSection,
    const SI_CHAR * a_pKey,
    const SI_CHAR * a_pDefault ) const
{
    if (!a_pSection || !a_pKey) {
        return a_pDefault;
    }
    TSection::const_iterator i = m_data.find(a_pSection);
    if (i == m_data.end()) {
        return a_pDefault;
    }
    TKeyVal::const_iterator j = i->second.find(a_pKey);
    if (j == i->second.end()) {
        return a_pDefault;
    }
    return j->second;
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
int
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::GetSectionSize(
    const SI_CHAR * a_pSection ) const
{
    if (a_pSection) {
        TSection::const_iterator i = m_data.find(a_pSection);
        if (i != m_data.end()) {
            return (int) i->second.size();
        }
    }
    return -1;
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
void
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::GetAllSections(
    TNames & a_names ) const
{
    a_names.resize( m_data.size() );
    TSection::const_iterator i = m_data.begin();
    for (int n = 0; i != m_data.end(); ++i, ++n ) {
        a_names[n] = i->first;
    }
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
void
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::GetAllKeys(
    const SI_CHAR * a_pSection,
    TNames &        a_names ) const
{
    if (a_pSection) {
        TSection::const_iterator i = m_data.find(a_pSection);
        if (i != m_data.end()) {
            const TKeyVal & section = i->second;
            a_names.resize( section.size() );
            TKeyVal::const_iterator i = section.begin();
            for (int n = 0; i != section.end(); ++i, ++n ) {
                a_names[n] = i->first;
            }
        }
    }
}

template<class SI_CHAR, class SI_STRCMP, class SI_CONVERTER>
int
CSimpleIniTempl<SI_CHAR,SI_STRCMP,SI_CONVERTER>::SaveFile(
    FILE *          a_pFile, 
    const SI_CHAR * a_pHeader) const
{
    SI_CONVERTER converter(true);
    size_t uScratchSize = 4096;
    char * pScratch = new char[uScratchSize];
    if (!pScratch) {
        return ENOMEM;
    }

    // output the header if we have one
    bool bFirstLine = true;
    if (a_pHeader && *a_pHeader) {
        int nLen = converter.SizeToStore(a_pHeader, -1);
        if ((size_t)nLen > uScratchSize) {
            while ((size_t)nLen > uScratchSize) {
                uScratchSize *= 2;
            }
            delete[] pScratch;
            pScratch = new char[uScratchSize];
            if (!pScratch) {
                return ENOMEM;
            }
        }
        if (!converter.ConvertToStore(a_pHeader, -1, pScratch, uScratchSize)) {
            return ERANGE;
        }
        fputs(pScratch, a_pFile);
        fputs(SI_NEWLINE, a_pFile);
        bFirstLine = false;
    }

    // iterate through our sections and output the data
    TSection::const_iterator iSection = m_data.begin();
    for ( ; iSection != m_data.end(); ++iSection ) {
        // write the section (unless there is no section name)
        if (iSection->first[0]) {
            if (!bFirstLine) {
                fputs(SI_NEWLINE, a_pFile);
            }
            if (!converter.ConvertToStore(iSection->first, -1, pScratch, uScratchSize)) {
                return ERANGE;
            }
            fprintf(a_pFile, "[%s]" SI_NEWLINE, pScratch);
        }

        // write all keys and values
        TKeyVal::const_iterator iKeyVal = iSection->second.begin();
        for ( ; iKeyVal != iSection->second.end(); ++iKeyVal) {
            // write the key
            if (!converter.ConvertToStore(iKeyVal->first, -1, pScratch, uScratchSize)) {
                return ERANGE;
            }
            fputs(pScratch, a_pFile);

            // write the value
            if (!converter.ConvertToStore(iKeyVal->second, -1, pScratch, uScratchSize)) {
                return ERANGE;
            }
            fprintf(a_pFile, "=%s" SI_NEWLINE, pScratch);
            bFirstLine = false;
        }
    }

    delete[] pScratch;
    return 0;
}

// ---------------------------------------------------------------------------
//                                  TYPE DEFINITIONS
// ---------------------------------------------------------------------------

#if defined(_WIN32) && !defined(SI_USING_WIN32_CHAR_FOR_UTF8)

// use Windows MBCS-aware no-case comparison
typedef CSimpleIniTempl<char,
    SI_WindowsNoCase<char>,SI_ConvertA<char> >          CSimpleIniA;
typedef CSimpleIniTempl<wchar_t,
    SI_WindowsNoCase<wchar_t>,SI_ConvertW<wchar_t> >    CSimpleIniW;

#else // !_WIN32 || SI_USING_WIN32_CHAR_FOR_UTF8

// use generic no-case comparison
typedef CSimpleIniTempl<char,
    SI_NoCase<char>,SI_ConvertA<char> >                 CSimpleIniA;
typedef CSimpleIniTempl<wchar_t,
    SI_NoCase<wchar_t>,SI_ConvertW<wchar_t> >           CSimpleIniW;

#endif // _WIN32 && !SI_USING_WIN32_CHAR_FOR_UTF8

typedef CSimpleIniTempl<char,
    SI_Case<char>,SI_ConvertA<char> >                   CSimpleIniCaseA;
typedef CSimpleIniTempl<wchar_t,
    SI_Case<wchar_t>,SI_ConvertW<wchar_t> >             CSimpleIniCaseW;

#if defined(_UNICODE)
# define CSimpleIni CSimpleIniW
#else
# define CSimpleIni CSimpleIniA
#endif

#endif // INCLUDED_SimpleIni_h

