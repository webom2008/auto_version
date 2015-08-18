#pragma once

class CCreateHeaderFile
{
public:
    CCreateHeaderFile(void);
    virtual ~CCreateHeaderFile(void);

    static int createFile(void *info);
protected:
    static int openVersionHeaderFile(void);
    static int closeVersionHeaderFile(void);
    static int writeOneLine(const char *line);
    static int writeHeaderCheckStartFlag(void);
    static int writeHeaderCheckEndFlag(void);
};

