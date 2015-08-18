#pragma once

#define FOR_ADSP_RELEASE

typedef signed char         s8;
typedef signed short        s16;
typedef signed int          s32;
typedef signed __int64      s64;
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned __int64    u64;

typedef struct
{
    u32 svn;
    u8 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
} VERSION_STRUCT_DEF;



#ifdef FOR_ADSP_RELEASE
const char EXECDOSCMD[] = "svn log --incremental --limit 1 >version.txt";
const char SVN_LOG_FILE[] = "version.txt";
const char CREATE_HEADER_FILE[] = "version.h";
#else
const char EXECDOSCMD[] = "cd ../ && svn log --incremental --limit 1 >version.txt";
const char SVN_LOG_FILE[] = "../version.txt";
const char CREATE_HEADER_FILE[] = "../version.h";
#endif
