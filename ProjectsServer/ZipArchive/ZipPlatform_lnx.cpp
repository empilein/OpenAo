////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2014 by Artpol Software - Tadeusz Dracz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// For the licensing details refer to the License.txt file.
//
// Web Site: http://www.artpol-software.com
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _ZIP_SYSTEM_LINUX

#if defined __APPLE__ || defined __CYGWIN__
	#define FILE_FUNCTIONS_64B_BY_DEFAULT
#else
	#undef FILE_FUNCTIONS_64B_BY_DEFAULT	
#endif	

#include "ZipPlatform.h"
#include "ZipFileHeader.h"
#include "ZipException.h"
#include "ZipAutoBuffer.h"

#include <utime.h>

#include "ZipPathComponent.h"
#include "ZipCompatibility.h"

#include <sys/types.h>

#if defined (__FreeBSD__) || defined (__APPLE__)
	#include <sys/param.h>
	#include <sys/mount.h>
#else
	#include <sys/vfs.h>
#endif
#include <sys/stat.h>

#include <unistd.h>
#include <stdio.h>

#include <fcntl.h>

const TCHAR CZipPathComponent::m_cSeparator = _T('/');

#ifndef _UTIMBUF_DEFINED
#define _utimbuf utimbuf
#endif

#define ZIP_DEFAULT_DIR_ATTRIBUTES (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

#ifdef _UNICODE
/* 
	The UNICODE conversions are derived from the UTF-8 CPP project at: http://sourceforge.net/projects/utfcpp/
	
	The original copyright notice is provided below.	
*/

/* BEGIN OF UNICODE CODE */

// Copyright 2006 Nemanja Trifunovic

/*
Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
 

#include <iterator>
#include <stdexcept>  

namespace utf8
{
    // The typedefs for 8-bit, 16-bit and 32-bit unsigned integers
    // You may need to change them to match your system.
    // These typedefs have the same names as ones from cstdint, or boost/cstdint
    typedef unsigned char   uint8_t;
    typedef unsigned short  uint16_t;
    typedef unsigned int    uint32_t;

// Helper code - not intended to be directly called by the library users. May be changed at any time
namespace internal
{
    // Unicode constants
    // Leading (high) surrogates: 0xd800 - 0xdbff
    // Trailing (low) surrogates: 0xdc00 - 0xdfff
    const uint16_t LEAD_SURROGATE_MIN  = 0xd800u;
    const uint16_t LEAD_SURROGATE_MAX  = 0xdbffu;
    const uint16_t TRAIL_SURROGATE_MIN = 0xdc00u;
    const uint16_t TRAIL_SURROGATE_MAX = 0xdfffu;
    const uint16_t LEAD_OFFSET         = LEAD_SURROGATE_MIN - (0x10000 >> 10);
    const uint32_t SURROGATE_OFFSET    = 0x10000u - (LEAD_SURROGATE_MIN << 10) - TRAIL_SURROGATE_MIN;

    // Maximum valid value for a Unicode code point
    const uint32_t CODE_POINT_MAX      = 0x0010ffffu;

    template<typename octet_type>
    inline uint8_t mask8(octet_type oc)
    {
        return static_cast<uint8_t>(0xff & oc);
    }
    template<typename u16_type>
    inline uint16_t mask16(u16_type oc)
    {
        return static_cast<uint16_t>(0xffff & oc);
    }
    template<typename octet_type>
    inline bool is_trail(octet_type oc)
    {
        return ((utf8::internal::mask8(oc) >> 6) == 0x2);
    }

    template <typename u16>
    inline bool is_lead_surrogate(u16 cp)
    {
        return (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX);
    }

    template <typename u16>
    inline bool is_trail_surrogate(u16 cp)
    {
        return (cp >= TRAIL_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    template <typename u16>
    inline bool is_surrogate(u16 cp)
    {
        return (cp >= LEAD_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    template <typename u32>
    inline bool is_code_point_valid(u32 cp)
    {
        return (cp <= CODE_POINT_MAX && !utf8::internal::is_surrogate(cp));
    }

    template <typename octet_iterator>
    inline typename std::iterator_traits<octet_iterator>::difference_type
    sequence_length(octet_iterator lead_it)
    {
        uint8_t lead = utf8::internal::mask8(*lead_it);
        if (lead < 0x80)
            return 1;
        else if ((lead >> 5) == 0x6)
            return 2;
        else if ((lead >> 4) == 0xe)
            return 3;
        else if ((lead >> 3) == 0x1e)
            return 4;
        else
            return 0;
    }

    template <typename octet_difference_type>
    inline bool is_overlong_sequence(uint32_t cp, octet_difference_type length)
    {
        if (cp < 0x80)
		{
            if (length != 1) 
                return true;
        }
        else if (cp < 0x800) 
		{
            if (length != 2) 
                return true;
        }
        else if (cp < 0x10000)
		{
            if (length != 3) 
                return true;
        }

        return false;
    }

    enum utf_error {UTF8_OK, NOT_ENOUGH_ROOM, INVALID_LEAD, INCOMPLETE_SEQUENCE, OVERLONG_SEQUENCE, INVALID_CODE_POINT};

    /// Helper for get_sequence_x
    template <typename octet_iterator>
    utf_error increase_safely(octet_iterator& it, octet_iterator end)
    {
        if (++it == end)
            return NOT_ENOUGH_ROOM;

        if (!utf8::internal::is_trail(*it))
            return INCOMPLETE_SEQUENCE;
        
        return UTF8_OK;
    }

    #define UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR(IT, END) {utf_error ret = increase_safely(IT, END); if (ret != UTF8_OK) return ret;}    

    /// get_sequence_x functions decode utf-8 sequences of the length x
    template <typename octet_iterator>
    utf_error get_sequence_1(octet_iterator& it, octet_iterator end, uint32_t& code_point)
    {
        if (it == end)
            return NOT_ENOUGH_ROOM;

        code_point = utf8::internal::mask8(*it);

        return UTF8_OK;
    }

    template <typename octet_iterator>
    utf_error get_sequence_2(octet_iterator& it, octet_iterator end, uint32_t& code_point)
    {
        if (it == end) 
            return NOT_ENOUGH_ROOM;
        
        code_point = utf8::internal::mask8(*it);

        UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR(it, end)

        code_point = ((code_point << 6) & 0x7ff) + ((*it) & 0x3f);

        return UTF8_OK;
    }

    template <typename octet_iterator>
    utf_error get_sequence_3(octet_iterator& it, octet_iterator end, uint32_t& code_point)
    {
        if (it == end)
            return NOT_ENOUGH_ROOM;
            
        code_point = utf8::internal::mask8(*it);

        UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR(it, end)

        code_point = ((code_point << 12) & 0xffff) + ((utf8::internal::mask8(*it) << 6) & 0xfff);

        UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR(it, end)

        code_point += (*it) & 0x3f;

        return UTF8_OK;
    }

    template <typename octet_iterator>
    utf_error get_sequence_4(octet_iterator& it, octet_iterator end, uint32_t& code_point)
    {
        if (it == end)
           return NOT_ENOUGH_ROOM;

        code_point = utf8::internal::mask8(*it);

        UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR(it, end)

        code_point = ((code_point << 18) & 0x1fffff) + ((utf8::internal::mask8(*it) << 12) & 0x3ffff);

        UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR(it, end)

        code_point += (utf8::internal::mask8(*it) << 6) & 0xfff;

        UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR(it, end)

        code_point += (*it) & 0x3f;

        return UTF8_OK;
    }

    #undef UTF8_CPP_INCREASE_AND_RETURN_ON_ERROR

    template <typename octet_iterator>
    utf_error validate_next(octet_iterator& it, octet_iterator end, uint32_t& code_point)
    {
        // Save the original value of it so we can go back in case of failure
        // Of course, it does not make much sense with i.e. stream iterators
        octet_iterator original_it = it;

        uint32_t cp = 0;
        // Determine the sequence length based on the lead octet
        typedef typename std::iterator_traits<octet_iterator>::difference_type octet_difference_type;
        const octet_difference_type length = utf8::internal::sequence_length(it);

        // Get trail octets and calculate the code point
        utf_error err = UTF8_OK;
        switch (length) 
		{
            case 0: 
                return INVALID_LEAD;
            case 1:
                err = utf8::internal::get_sequence_1(it, end, cp);
                break;
            case 2:
                err = utf8::internal::get_sequence_2(it, end, cp);
				break;
            case 3:
                err = utf8::internal::get_sequence_3(it, end, cp);
				break;
            case 4:
                err = utf8::internal::get_sequence_4(it, end, cp);
				break;
        }

        if (err == UTF8_OK) 
		{
            // Decoding succeeded. Now, security checks...
            if (utf8::internal::is_code_point_valid(cp)) 
			{
                if (!utf8::internal::is_overlong_sequence(cp, length))
				{
                    // Passed! Return here.
                    code_point = cp;
                    ++it;
                    return UTF8_OK;
                }
                else
                    err = OVERLONG_SEQUENCE;
            }
            else 
                err = INVALID_CODE_POINT;
        }

        // Failure branch - restore the original value of the iterator
        it = original_it;
        return err;
    }

    template <typename octet_iterator>
    inline utf_error validate_next(octet_iterator& it, octet_iterator end) 
	{
        uint32_t ignored;
        return utf8::internal::validate_next(it, end, ignored);
    }

} // namespace internal

    /// The library API - functions intended to be called by the users

    // Byte order mark
    const uint8_t bom[] = {0xef, 0xbb, 0xbf};

    template <typename octet_iterator>
    octet_iterator find_invalid(octet_iterator start, octet_iterator end)
    {
        octet_iterator result = start;
        while (result != end) {
            utf8::internal::utf_error err_code = utf8::internal::validate_next(result, end);
            if (err_code != internal::UTF8_OK)
                return result;
        }
        return result;
    }

    template <typename octet_iterator>
    inline bool is_valid(octet_iterator start, octet_iterator end)
    {
        return (utf8::find_invalid(start, end) == end);
    }

    template <typename octet_iterator>
    inline bool starts_with_bom (octet_iterator it, octet_iterator end)
    {
        return (
            ((it != end) && (utf8::internal::mask8(*it++)) == bom[0]) &&
            ((it != end) && (utf8::internal::mask8(*it++)) == bom[1]) &&
            ((it != end) && (utf8::internal::mask8(*it))   == bom[2])
           );
    }
	
    //Deprecated in release 2.3 
    template <typename octet_iterator>
    inline bool is_bom (octet_iterator it)
    {
        return (
            (utf8::internal::mask8(*it++)) == bom[0] &&
            (utf8::internal::mask8(*it++)) == bom[1] &&
            (utf8::internal::mask8(*it))   == bom[2]
           );
    }

	 // Base for the exceptions that may be thrown from the library
    class exception : public ::std::exception
	{
    };

    // Exceptions that may be thrown from the library functions.
    class invalid_code_point : public exception 
	{
        uint32_t cp;
    public:
        invalid_code_point(uint32_t cp) : cp(cp) {}
        virtual const char* what() const throw() { return "Invalid code point"; }
        uint32_t code_point() const {return cp;}
    };

    class invalid_utf8 : public exception
	{
        uint8_t u8;
    public:
        invalid_utf8 (uint8_t u) : u8(u) {}
        virtual const char* what() const throw() { return "Invalid UTF-8"; }
        uint8_t utf8_octet() const {return u8;}
    };

    class invalid_utf16 : public exception 
	{
        uint16_t u16;
    public:
        invalid_utf16 (uint16_t u) : u16(u) {}
        virtual const char* what() const throw() { return "Invalid UTF-16"; }
        uint16_t utf16_word() const {return u16;}
    };

    class not_enough_room : public exception
	{
    public:
        virtual const char* what() const throw() { return "Not enough space"; }
    };

    /// The library API - functions intended to be called by the users

    template <typename octet_iterator>
    octet_iterator append(uint32_t cp, octet_iterator result)
    {
        if (!utf8::internal::is_code_point_valid(cp))
            throw invalid_code_point(cp);

        if (cp < 0x80)                        // one octet
            *(result++) = static_cast<uint8_t>(cp);
        else if (cp < 0x800) 
		{                // two octets
            *(result++) = static_cast<uint8_t>((cp >> 6)            | 0xc0);
            *(result++) = static_cast<uint8_t>((cp & 0x3f)          | 0x80);
        }
        else if (cp < 0x10000) 
		{              // three octets
            *(result++) = static_cast<uint8_t>((cp >> 12)           | 0xe0);
            *(result++) = static_cast<uint8_t>(((cp >> 6) & 0x3f)   | 0x80);
            *(result++) = static_cast<uint8_t>((cp & 0x3f)          | 0x80);
        }
        else 
		{                                // four octets
            *(result++) = static_cast<uint8_t>((cp >> 18)           | 0xf0);
            *(result++) = static_cast<uint8_t>(((cp >> 12) & 0x3f)  | 0x80);
            *(result++) = static_cast<uint8_t>(((cp >> 6) & 0x3f)   | 0x80);
            *(result++) = static_cast<uint8_t>((cp & 0x3f)          | 0x80);
        }
        return result;
    }

    template <typename octet_iterator, typename output_iterator>
    output_iterator replace_invalid(octet_iterator start, octet_iterator end, output_iterator out, uint32_t replacement)
    {
        while (start != end) {
            octet_iterator sequence_start = start;
            internal::utf_error err_code = utf8::internal::validate_next(start, end);
            switch (err_code) 
			{
                case internal::UTF8_OK :
                    for (octet_iterator it = sequence_start; it != start; ++it)
                        *out++ = *it;
                    break;
                case internal::NOT_ENOUGH_ROOM:
                    throw not_enough_room();
                case internal::INVALID_LEAD:
                    out = utf8::append (replacement, out);
                    ++start;
                    break;
                case internal::INCOMPLETE_SEQUENCE:
                case internal::OVERLONG_SEQUENCE:
                case internal::INVALID_CODE_POINT:
                    out = utf8::append (replacement, out);
                    ++start;
                    // just one replacement mark for the sequence
                    while (start != end && utf8::internal::is_trail(*start))
                        ++start;
                    break;
            }
        }
        return out;
    }

    template <typename octet_iterator, typename output_iterator>
    inline output_iterator replace_invalid(octet_iterator start, octet_iterator end, output_iterator out)
    {
        static const uint32_t replacement_marker = utf8::internal::mask16(0xfffd);
        return utf8::replace_invalid(start, end, out, replacement_marker);
    }

    template <typename octet_iterator>
    uint32_t next(octet_iterator& it, octet_iterator end)
    {
        uint32_t cp = 0;
        internal::utf_error err_code = utf8::internal::validate_next(it, end, cp);
        switch (err_code) {
            case internal::UTF8_OK :
                break;
            case internal::NOT_ENOUGH_ROOM :
                throw not_enough_room();
            case internal::INVALID_LEAD :
            case internal::INCOMPLETE_SEQUENCE :
            case internal::OVERLONG_SEQUENCE :
                throw invalid_utf8(*it);
            case internal::INVALID_CODE_POINT :
                throw invalid_code_point(cp);
        }
        return cp;
    }

    template <typename octet_iterator>
    uint32_t peek_next(octet_iterator it, octet_iterator end)
    {
        return utf8::next(it, end);
    }

    template <typename octet_iterator>
    uint32_t prior(octet_iterator& it, octet_iterator start)
    {
        // can't do much if it == start
        if (it == start)
            throw not_enough_room();

        octet_iterator end = it;
        // Go back until we hit either a lead octet or start
        while (utf8::internal::is_trail(*(--it)))
            if (it == start)
                throw invalid_utf8(*it); // error - no lead byte in the sequence
        return utf8::peek_next(it, end);
    }

    /// Deprecated in versions that include "prior"
    template <typename octet_iterator>
    uint32_t previous(octet_iterator& it, octet_iterator pass_start)
    {
        octet_iterator end = it;
        while (utf8::internal::is_trail(*(--it)))
            if (it == pass_start)
                throw invalid_utf8(*it); // error - no lead byte in the sequence
        octet_iterator temp = it;
        return utf8::next(temp, end);
    }

    template <typename octet_iterator, typename distance_type>
    void advance (octet_iterator& it, distance_type n, octet_iterator end)
    {
        for (distance_type i = 0; i < n; ++i)
            utf8::next(it, end);
    }

    template <typename octet_iterator>
    typename std::iterator_traits<octet_iterator>::difference_type
    distance (octet_iterator first, octet_iterator last)
    {
        typename std::iterator_traits<octet_iterator>::difference_type dist;
        for (dist = 0; first < last; ++dist)
            utf8::next(first, last);
        return dist;
    }

    template <typename u16bit_iterator, typename octet_iterator>
    octet_iterator utf16to8 (u16bit_iterator start, u16bit_iterator end, octet_iterator result)
    {
        while (start != end)
		{
            uint32_t cp = utf8::internal::mask16(*start++);
            // Take care of surrogate pairs first
            if (utf8::internal::is_lead_surrogate(cp))
			{
                if (start != end) 
				{
                    uint32_t trail_surrogate = utf8::internal::mask16(*start++);
                    if (utf8::internal::is_trail_surrogate(trail_surrogate))
                        cp = (cp << 10) + trail_surrogate + internal::SURROGATE_OFFSET;
                    else
                        throw invalid_utf16(static_cast<uint16_t>(trail_surrogate));
                }
                else
                    throw invalid_utf16(static_cast<uint16_t>(cp));

            }
            // Lone trail surrogate
            else if (utf8::internal::is_trail_surrogate(cp))
                throw invalid_utf16(static_cast<uint16_t>(cp));

            result = utf8::append(cp, result);
        }
        return result;
    }

    template <typename u16bit_iterator, typename octet_iterator>
    u16bit_iterator utf8to16 (octet_iterator start, octet_iterator end, u16bit_iterator result)
    {
        while (start != end)
		{
            uint32_t cp = utf8::next(start, end);
            if (cp > 0xffff) 
			{
				//make a surrogate pair
                *result++ = static_cast<uint16_t>((cp >> 10)   + internal::LEAD_OFFSET);
                *result++ = static_cast<uint16_t>((cp & 0x3ff) + internal::TRAIL_SURROGATE_MIN);
            }
            else
                *result++ = static_cast<uint16_t>(cp);
        }
        return result;
    }

    template <typename octet_iterator, typename u32bit_iterator>
    octet_iterator utf32to8 (u32bit_iterator start, u32bit_iterator end, octet_iterator result)
    {
        while (start != end)
            result = utf8::append(*(start++), result);

        return result;
    }

    template <typename octet_iterator, typename u32bit_iterator>
    u32bit_iterator utf8to32 (octet_iterator start, octet_iterator end, u32bit_iterator result)
    {
        while (start != end)
            (*result++) = utf8::next(start, end);

        return result;
    }

    // The iterator class
    template <typename octet_iterator>
    class iterator : public std::iterator <std::bidirectional_iterator_tag, uint32_t> {
      octet_iterator it;
      octet_iterator range_start;
      octet_iterator range_end;
      public:
      iterator () {}
      explicit iterator (const octet_iterator& octet_it,
                         const octet_iterator& range_start,
                         const octet_iterator& range_end) :
               it(octet_it), range_start(range_start), range_end(range_end)
      {
          if (it < range_start || it > range_end)
              throw std::out_of_range("Invalid utf-8 iterator position");
      }
      // the default "big three" are OK
      octet_iterator base () const { return it; }
      uint32_t operator * () const
      {
          octet_iterator temp = it;
          return utf8::next(temp, range_end);
      }
      bool operator == (const iterator& rhs) const
      {
          if (range_start != rhs.range_start || range_end != rhs.range_end)
              throw std::logic_error("Comparing utf-8 iterators defined with different ranges");
          return (it == rhs.it);
      }
      bool operator != (const iterator& rhs) const
      {
          return !(operator == (rhs));
      }
      iterator& operator ++ ()
      {
          utf8::next(it, range_end);
          return *this;
      }
      iterator operator ++ (int)
      {
          iterator temp = *this;
          utf8::next(it, range_end);
          return temp;
      }
      iterator& operator -- ()
      {
          utf8::prior(it, range_start);
          return *this;
      }
      iterator operator -- (int)
      {
          iterator temp = *this;
          utf8::prior(it, range_start);
          return temp;
      }
    }; // class iterator 
} // namespace utf8 

/* END OF UNICODE CODE */

#if !defined(SIZEOFWCHART)
	#if __SIZEOF_WCHAR_T__ == 4 || __WCHAR_MAX__ > 0x10000
		#define SIZEOFWCHART 4
	#else
		#define SIZEOFWCHART 2
	#endif
#endif

#endif

ULONGLONG ZipPlatform::GetDeviceFreeSpace(LPCTSTR lpszPath)
{
		struct statfs sStats;
		_ZIP_WIDE_TO_MULTIBYTE(lpszPath, path)		

	#if defined (__SVR4) && defined (__sun)
		if (statvfs(path, &sStats) == -1) // Solaris
	#else
		if (statfs(path, &sStats) == -1)
	#endif
		return 0;

        return sStats.f_bsize * sStats.f_bavail;
}


CZipString ZipPlatform::GetTmpFileName(LPCTSTR lpszPath, ZIP_SIZE_TYPE uSizeNeeded)
{
	TCHAR empty[] = _T(""), prefix [] = _T("zar");
	CZipString tempPath = lpszPath;
	if (tempPath.IsEmpty())
		tempPath = _T("/tmp");
	if (uSizeNeeded > 0 && ZipPlatform::GetDeviceFreeSpace(tempPath) < uSizeNeeded)
		return empty;
	CZipPathComponent::AppendSeparator(tempPath);
	tempPath += prefix;
	tempPath += _T("XXXXXX");
	int handle;
#ifdef _UNICODE
	CZipAutoBuffer tempPathBuffer;	
	WideToMultiByte(tempPath, tempPathBuffer, true, 0);
	handle = mkstemp(tempPathBuffer);
	// copy back to tempPath
	MultiByteToWide(tempPathBuffer, -1, tempPath, 0);	
#else	
	handle = mkstemp(tempPath.GetBuffer(tempPath.GetLength()));
	tempPath.ReleaseBuffer();
#endif	
	if (handle != -1)
	{
		close(handle); // we just create the file and open it later
		return tempPath;
	}
	else
		return empty;		
}

bool ZipPlatform::GetCurrentDirectory(CZipString& sz)
{
	char* pBuf = getcwd(NULL, 0);
	if (!pBuf)
		return false;
#ifdef _UNICODE	
	MultiByteToWide(pBuf, -1, sz, 0);
#else	
	sz = pBuf;
#endif	
	free(pBuf);
	return true;
}

bool ZipPlatform::SetFileAttr(LPCTSTR lpFileName, DWORD uAttr)
{
    _ZIP_WIDE_TO_MULTIBYTE(lpFileName, fileName)
	return chmod(fileName, uAttr) == 0;
}

bool ZipPlatform::GetFileAttr(LPCTSTR lpFileName, DWORD& uAttr)
{	
	_ZIP_WIDE_TO_MULTIBYTE(lpFileName, fileName)
    
	struct stat sStats;
	if (stat(fileName, &sStats) == -1)
		return false;
  	uAttr = (sStats.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_IFMT));
  	return true;
}

bool ZipPlatform::SetExeAttr(LPCTSTR lpFileName)
{	
	DWORD uAttr;
	if (!GetFileAttr(lpFileName, uAttr))
		return false;
	uAttr |= S_IXUSR;
	return ZipPlatform::SetFileAttr(lpFileName, uAttr);
}


bool ZipPlatform::GetFileTimes(LPCTSTR lpFileName, time_t* tModificationTime, time_t* tCreationTime, time_t* tLastAccessTime)
{
	if (tModificationTime != NULL)
	{
		*tModificationTime = 0;
	}
	if (tCreationTime != NULL)
	{
		*tCreationTime = 0;
	}
	if (tLastAccessTime != NULL)
	{
		*tLastAccessTime = 0;
	}
	_ZIP_WIDE_TO_MULTIBYTE(lpFileName, fileName)
    struct stat st;
	bool ret;
	if (stat(fileName, &st) == 0)
	{
		ret = true;
		if (tModificationTime != NULL)
			*tModificationTime = st.st_mtime;
		if (tCreationTime != NULL)
			*tCreationTime = st.st_ctime;
		if (tLastAccessTime != NULL)
			*tLastAccessTime = st.st_atime;
	}
	else 
		ret = false; 

	if (ret == false)
	{
		time_t tNow = time(NULL);
		if (tModificationTime != NULL && *tModificationTime <= 0)
		{
			*tModificationTime = tNow;
		}
		if (tCreationTime != NULL && *tCreationTime <= 0)
		{
			*tCreationTime = tNow;
		}
		if (tLastAccessTime != NULL && *tLastAccessTime <= 0)
		{
			*tLastAccessTime = tNow;
		}
	}
	return ret;
}

bool ZipPlatform::SetFileTimes(LPCTSTR lpFileName, const time_t* tModificationTime, const time_t* tCreationTime, const time_t* tLastAccessTime)
{
	_ZIP_WIDE_TO_MULTIBYTE(lpFileName, fileName)
	struct utimbuf ub;
	// if wrong file time, set it to the current
	ub.actime = tLastAccessTime != NULL && *tLastAccessTime > 0 ? *tLastAccessTime : time(NULL);
	ub.modtime = tModificationTime != NULL && *tModificationTime > 0 ? *tModificationTime : time(NULL);
	// no creation time
	return utime(fileName, &ub) == 0;
}


bool ZipPlatform::ChangeDirectory(LPCTSTR lpDirectory)
{
	_ZIP_WIDE_TO_MULTIBYTE(lpDirectory, directory)
	return chdir(directory) == 0; 
}
int ZipPlatform::FileExists(LPCTSTR lpszName)
{
	_ZIP_WIDE_TO_MULTIBYTE(lpszName, name)
    struct stat st;
	if (stat(name, &st) != 0)
		return 0;
	else
	{
		if (S_ISDIR(st.st_mode))
			return -1;
		else
			return 1;
	}
}

ZIPINLINE  bool ZipPlatform::IsDriveRemovable(LPCTSTR lpszFilePath)
{
	// not implemented
	return true;
}

ZIPINLINE  bool ZipPlatform::SetVolLabel(LPCTSTR lpszPath, LPCTSTR lpszLabel)
{
	// not implemented
        return true;
}

ZIPINLINE void ZipPlatform::AnsiOem(CZipAutoBuffer& buffer, bool bAnsiToOem)
{
	// not implemented
}

ZIPINLINE  bool ZipPlatform::RemoveFile(LPCTSTR lpszFileName, bool bThrow, int iMode)
{
	if ((iMode & ZipPlatform::fomRemoveReadOnly) != 0)
	{
		DWORD attr;
		if (ZipPlatform::GetFileAttr(lpszFileName, attr)
			&& (ZipCompatibility::GetAsInternalAttributes(attr, ZipPlatform::GetSystemID()) & ZipCompatibility::attROnly) != 0)
		{
			ZipPlatform::SetFileAttr(lpszFileName, ZipPlatform::GetDefaultAttributes());
		}
	}
	_ZIP_WIDE_TO_MULTIBYTE(lpszFileName, name)
	if (unlink(name) != 0)
    {
		if (bThrow)
			CZipException::Throw(CZipException::notRemoved, lpszFileName);
		else 
			return false;
	}
    return true;
}

ZIPINLINE  bool ZipPlatform::RenameFile( LPCTSTR lpszOldName, LPCTSTR lpszNewName, bool bThrow)
{	
	_ZIP_WIDE_TO_MULTIBYTE(lpszOldName, oldName)
	_ZIP_WIDE_TO_MULTIBYTE(lpszNewName, newName)
	if (rename(oldName, newName) != 0)
	{
		if (bThrow)
			CZipException::Throw(CZipException::notRenamed, lpszOldName);
		return false;
	}
	return true;
}

ZIPINLINE  bool ZipPlatform::IsDirectory(DWORD uAttr)
{
	return S_ISDIR(uAttr) != 0;
}

ZIPINLINE  bool ZipPlatform::CreateNewDirectory(LPCTSTR lpDirectory)
{	
	_ZIP_WIDE_TO_MULTIBYTE(lpDirectory, directory)
	return mkdir(directory, ZIP_DEFAULT_DIR_ATTRIBUTES) == 0;
}

ZIPINLINE  DWORD ZipPlatform::GetDefaultAttributes()
{
	return S_IFREG | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
}

ZIPINLINE  DWORD ZipPlatform::GetDefaultDirAttributes()
{
	return S_IFDIR | ZIP_DEFAULT_DIR_ATTRIBUTES;
}

ZIPINLINE  int ZipPlatform::GetSystemID()
{
	return ZipCompatibility::zcUnix;
}

ZIPINLINE bool ZipPlatform::GetSystemCaseSensitivity()
{
	return true;
}

bool ZipPlatform::TruncateFile(int iDes, ULONGLONG uSize)
{
#if defined FILE_FUNCTIONS_64B_BY_DEFAULT || defined __ANDROID__
	#ifdef __ANDROID__
		if (uSize > 0x7FFFFFFF)
			CZipException::Throw();
	#endif
	return ftruncate(iDes, uSize) == 0;
#else
	return ftruncate64(iDes, uSize) == 0;
	
#endif

}

int ZipPlatform::OpenFile(LPCTSTR lpszFileName, UINT iMode, int iShareMode)
{
	_ZIP_WIDE_TO_MULTIBYTE(lpszFileName, fileName)
#if defined FILE_FUNCTIONS_64B_BY_DEFAULT || defined __ANDROID__
	return open(fileName, iMode, S_IRUSR | S_IWUSR | S_IRGRP |S_IROTH );	
#else
	return open64(fileName, iMode, S_IRUSR | S_IWUSR | S_IRGRP |S_IROTH );	
#endif
}

bool ZipPlatform::FlushFile(int iDes)
{
	return fsync(iDes) == 0;
}

intptr_t ZipPlatform::GetFileSystemHandle(int iDes)
{
	return iDes;
}


#ifdef _UNICODE

int ZipPlatform::WideToMultiByte(LPCWSTR lpszIn, CZipAutoBuffer &szOut, bool bAddNull, UINT uCodePage)
{
	size_t wideLen = wcslen(lpszIn);
	if (wideLen == 0)
	{
		szOut.Release();
		return 0;
	}	
	std::vector<utf8::uint8_t> result;
	try
	{
#if SIZEOFWCHART == 4
		utf8::utf32to8(lpszIn, lpszIn + wideLen, std::back_inserter(result));
#else
		utf8::utf16to8(lpszIn, lpszIn + wideLen, std::back_inserter(result));
#endif		
	}
	catch(utf8::exception&)
	{
		szOut.Release();
		return -1;
	}

	size_t resultSize = result.size();
	if (resultSize == 0)
	{
		szOut.Release();
		// here it means an error
		return -1;
	}

	szOut.Allocate(bAddNull ? resultSize + 1 : resultSize);
	std::copy(result.begin(), result.end(), (char*)szOut);
	if (bAddNull)
	{
		szOut[resultSize] = 0;
	}
	return szOut.GetSize();
}

// TODO: implement normalization
int ZipPlatform::MultiByteToWide(const char* szIn, int iInSize, CZipString& szOut, UINT uCodePage)
{	
	szOut.Empty();
	if (iInSize < 0)
	{
		iInSize = (int)std::string(szIn).length();
	}	
	if (iInSize == 0)
	{		
		return 0;
	}
	
#if SIZEOFWCHART == 4
	std::vector<utf8::uint32_t> result;
#else
	std::vector<utf8::uint16_t> result;
#endif
	try
	{
		const char* input = szIn;
#if SIZEOFWCHART == 4
		utf8::utf8to32(input, input + iInSize, std::back_inserter(result));
#else
		utf8::utf8to16(input, input + iInSize, std::back_inserter(result));
#endif
		if (result.size() == 0)
		{
			// here it means an error
			return -1;
		}
	}
	catch(utf8::exception&)
	{		
		return -1;
	}
	stdbs& out = szOut;
	out.insert(out.begin(), result.begin(), result.end());
	return szOut.GetLength();
}
#endif

#endif // _ZIP_SYSTEM_LINUX
