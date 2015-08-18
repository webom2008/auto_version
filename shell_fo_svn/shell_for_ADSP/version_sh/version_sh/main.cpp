#include <iostream>
#include "typedefs.h"
#include "includes.h"
#include "CreateHeaderFile.h"

static VERSION_STRUCT_DEF gVersionInfo;
static VERSION_STRUCT_DEF *gpVersionInfo = &gVersionInfo;

#pragma comment(lib, "shell32.lib")
#pragma comment(linker,   "/subsystem:\"windows\"   /entry:\"mainCRTStartup\""   ) 




static int doReadSvnLog(void)
{
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead,hWrite;
        
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
        
    if (!CreatePipe(&hRead,&hWrite,&sa,0))
    {
        return FALSE;   
    }
        
    char command[1024]; //长达1K的命令行，够用了吧
        
    strcpy(command,"Cmd.exe /C ");
    strcat(command,EXECDOSCMD);
        
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
        
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
        
    si.hStdError = hWrite; //把创建进程的标准错误输出重定向到管道输入
    si.hStdOutput = hWrite; //把创建进程的标准输出重定向到管道输入
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        
    //关键步骤，CreateProcess函数参数意义请查阅MSDN
    if (!CreateProcess(NULL, command,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))        
    {       
        CloseHandle(hWrite);    
        CloseHandle(hRead);     
        return FALSE;   
    }
    CloseHandle(hWrite);
        
    char buffer[4096] = {0}; //用4K的空间来存储输出的内容，只要不是显示文件内容，一般情况下是够用了。
    DWORD bytesRead;
        
    while (true)    
    {       
        if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)    
                break;
    }
    std::cout<<buffer<<std::endl;
    CloseHandle(hRead);
    return 0;
}

static int readFileInfo(CString &strLine)
{
    CStdioFile file;
    TCHAR* pFileName = (TCHAR*)SVN_LOG_FILE;

    if(!file.Open(pFileName,CFile::modeRead | CFile::typeText)) 
    {
       TRACE(_T("Unable to open file\n"));
       return -1;
    }
    
    file.ReadString(strLine);//MS这里得跳一行
    if(!file.ReadString(strLine))
    {
       TRACE(_T("ReadString Error\n"));
       return -2;
    }
    file.Close();

    TRACE(_T("readFileInfo OK\n"));
    return 0;
}

static void string2date(CString &str)
{
    int offset_start = 0;
    int offset_end = 0;
    int count = 0;
    int tmp = 0;
    CString tmpStr;
    
    offset_start = 0;
    offset_end = str.Find('-', offset_start);
    count = offset_end - offset_start;
    tmpStr = str.Mid(offset_start, count);
    tmp =  _ttoi(tmpStr);
    gpVersionInfo->year = (BYTE)(tmp - 2000);

    offset_start = offset_end+1;
    offset_end = str.Find('-',offset_start);
    count = offset_end - offset_start;
    tmpStr = str.Mid(offset_start, count);
    tmp =  _ttoi(tmpStr);
    gpVersionInfo->month = (BYTE)tmp;
    
    count = 2;
    tmpStr = str.Right(count);
    tmp =  _ttoi(tmpStr);
    gpVersionInfo->day = (BYTE)tmp;
}

static void string2time(CString &str)
{
    int offset_start = 0;
    int offset_end = 0;
    int count = 0;
    int tmp = 0;
    CString tmpStr;
    
    offset_start = 0;
    offset_end = str.Find(':', offset_start);
    count = offset_end - offset_start;
    tmpStr = str.Mid(offset_start, count);
    tmp =  _ttoi(tmpStr);
    gpVersionInfo->hour = (BYTE)tmp;

    offset_start = offset_end+1;
    offset_end = str.Find(':',offset_start);
    count = offset_end - offset_start;
    tmpStr = str.Mid(offset_start, count);
    tmp =  _ttoi(tmpStr);
    gpVersionInfo->min = (BYTE)tmp;
    
    count = 2;
    tmpStr = str.Right(count);
    tmp =  _ttoi(tmpStr);
    gpVersionInfo->sec = (BYTE)tmp;
}

static int praseVersionInfo(const CString &info)
{
    int offset_start = 0;
    int offset_end = 0;
    int count = 0;
    CString version;
    CString datetime;
    CString date;
    CString time;
    memset(gpVersionInfo, 0x00, sizeof(VERSION_STRUCT_DEF));

    offset_start = info.Find('r');
    offset_end = info.Find(' ');
    offset_start++;
    count = offset_end - offset_start;
    version = info.Mid(offset_start, count);
    gpVersionInfo->svn = _ttoi(version);
    
    offset_start = info.Find('|');
    offset_start++;
    offset_start = info.Find('|', offset_start);
    offset_start++;
    offset_end = info.Find('+', offset_start);
    count = offset_end - offset_start;
    datetime = info.Mid(offset_start, count);

    
    offset_start = datetime.Find(' ');
    offset_start++;
    offset_end = datetime.Find(' ', offset_start);
    count = offset_end - offset_start;
    date = datetime.Mid(offset_start, count);
    string2date(date);

    offset_start = offset_end+1;
    offset_end = datetime.Find(' ', offset_start);
    count = offset_end - offset_start;
    time = datetime.Mid(offset_start, count);
    string2time(time);

    return 0;
}

static int deleteTempFile(void)
{
    CFile TempFile;
    TempFile.Remove(SVN_LOG_FILE);
    return 0;
}

int main (int argc, char *argv[])
{
    int ret = 0;
    CString info;
    
#ifdef FOR_ADSP_RELEASE
    doReadSvnLog();
#endif
    
    ret =  readFileInfo(info);
    if (0 != ret)
    {
        TRACE(_T("readFileInfo exit -1\n"));
        return -1;
    }

    praseVersionInfo(info);

    CCreateHeaderFile::createFile(gpVersionInfo);

    deleteTempFile();
    return 0;
}




