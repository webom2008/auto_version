#include "CreateHeaderFile.h"
#include "includes.h"
#include "typedefs.h"

static const char comment[] = "/* This is auto create by version_sh.exe. Do not modify by yourself! */\r\n";
static const char SVN_VERSION_NAME[]="const char SVN_VERSION[] = ";
static CFile gVersionHeaderFile;
static bool gbIsFileOpen = false;

CCreateHeaderFile::CCreateHeaderFile(void)
{
}


CCreateHeaderFile::~CCreateHeaderFile(void)
{
}


int CCreateHeaderFile::createFile(void *info)
{
    char buf[200];
    char svn[50];

    memset(buf, 0x00, sizeof(buf));
    memset(svn, 0x00, sizeof(svn));

    VERSION_STRUCT_DEF *pVersionInfo = (VERSION_STRUCT_DEF *)info;
    openVersionHeaderFile();
    writeHeaderCheckStartFlag();

    sprintf_s(svn, "\"%06d_%02d%02d%02d%02d%02d%02d\"",
        pVersionInfo->svn,
        pVersionInfo->year,
        pVersionInfo->month,
        pVersionInfo->day,
        pVersionInfo->hour,
        pVersionInfo->min,
        pVersionInfo->sec);

    memcpy(buf, SVN_VERSION_NAME, strlen(SVN_VERSION_NAME));
    strcat(buf, svn);
    strcat(buf, ";\r\n");
    writeOneLine(buf);

    writeHeaderCheckEndFlag();
    closeVersionHeaderFile();
    return 0;
}

int CCreateHeaderFile::openVersionHeaderFile(void)
{
    CFileException e;
    if (!gVersionHeaderFile.Open(CREATE_HEADER_FILE,\
        CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, &e))
    {
        TRACE(_T("File could not be opened %d\n"), e.m_cause);
        return -1;
    }

    gVersionHeaderFile.Write(comment, sizeof(comment));
    gVersionHeaderFile.Flush();
    gVersionHeaderFile.Write("\r\n", 2);
    gVersionHeaderFile.Flush();
    gbIsFileOpen = true;
    return 0;
}

int CCreateHeaderFile::closeVersionHeaderFile(void)
{
    if (!gbIsFileOpen) 
    {
        TRACE(_T("File did't open yet\n"));
        return -1;
    }

    gVersionHeaderFile.Flush();
    gVersionHeaderFile.Close();
    gbIsFileOpen = false;
    return 0;
}


int CCreateHeaderFile::writeOneLine(const char *line)
{
    if (!gbIsFileOpen) 
    {
        TRACE(_T("File did't open yet\n"));
        return -1;
    }

    gVersionHeaderFile.Write(line, strlen(line));
    gVersionHeaderFile.Flush();

    return 0;
}

int CCreateHeaderFile::writeHeaderCheckStartFlag(void)
{
    int i;
    const char HeadFlag[10][50] = 
    {
        "#ifndef __VERSION_H__\r\n",
        "#define __VERSION_H__\r\n",
        "\r\n",
        "#ifdef __cplusplus\r\n",
        "#if __cplusplus\r\n",
        "extern \"C\"{\r\n",
        "#endif\r\n",
        "#endif /* __cplusplus */\r\n",
        "\r\n",
        "\r\n"
    };
    if (!gbIsFileOpen) 
    {
        TRACE(_T("File did't open yet\n"));
        return -1;
    }

    for (i = 0; i < 10; i++)
    {
        gVersionHeaderFile.Write(HeadFlag[i], strlen(HeadFlag[i]));
        gVersionHeaderFile.Flush();
    }
    return 0;
}

int CCreateHeaderFile::writeHeaderCheckEndFlag(void)
{
    int i;
    const char EndFlag[10][50] = 
    {
        "\r\n",
        "#ifdef __cplusplus\r\n",
        "#if __cplusplus\r\n",
        "}\r\n",
        "#endif\r\n",
        "#endif /* __cplusplus */\r\n",
        "\r\n",
        "\r\n",
        "#endif /* __VERSION_H__ */\r\n",
        "\r\n"
    };
    if (!gbIsFileOpen) 
    {
        TRACE(_T("File did't open yet\n"));
        return -1;
    }

    for (i = 0; i < 10; i++)
    {
        gVersionHeaderFile.Write(EndFlag[i], strlen(EndFlag[i]));
        gVersionHeaderFile.Flush();
    }
    return 0;

}