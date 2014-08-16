#include "HostMainWidget.h"
#include "AudienceMainWidget.h"
#include <QApplication>
#include <QDateTime>

#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/LyricParse/LyricParse.h"
#include "Modules/HttpDownload/HttpDownload.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/LogHelper/LogHelperWidget.h"
#include "Modules/HttpUpload/HttpUpLoad.h"

#include "DataType/Player/Player.h"

#include "UI/FeedBackWidget/FeedBackWidget.h"
#include "Modules/log4qt/propertyconfigurator.h"
#include "Modules/log4qt/logmanager.h"
#include "Modules/StateController/StateController.h"
#include "Modules/ExceptionController/ExceptionController.h"
#define MAIN_DEBUG  1

#if MAIN_DEBUG
#include <QDebug>
#define MainDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define MainDebug(format,...)
#endif

#pragma once
#include <windows.h>
#include <DbgHelp.h>
#include <stdlib.h>
#pragma comment(lib, "dbghelp.lib")

#ifndef _M_IX86
#error "The following code only works for x86!"
#endif
char pluginPath[1024] = {0};
static QString coreStr;

inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
{
    if(pModuleName == 0)
    {
        return FALSE;
    }

    WCHAR szFileName[_MAX_FNAME] = L"";
    _wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);

    if(wcsicmp(szFileName, L"ntdll") == 0)
        return TRUE;

    return FALSE;
}

inline BOOL CALLBACK MiniDumpCallback(PVOID                            pParam,
                                      const PMINIDUMP_CALLBACK_INPUT   pInput,
                                      PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
    if(pInput == 0 || pOutput == 0)
        return FALSE;

    switch(pInput->CallbackType)
    {
    case ModuleCallback:
        if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
            if(!IsDataSectionNeeded(pInput->Module.FullPath))
                pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
    case IncludeModuleCallback:
    case IncludeThreadCallback:
    case ThreadCallback:
    case ThreadExCallback:
        return TRUE;
    default:;
    }

    return FALSE;
}

inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
{
    HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
    {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId           = GetCurrentThreadId();
        mdei.ExceptionPointers  = pep;
        mdei.ClientPointers     = NULL;

        MINIDUMP_CALLBACK_INFORMATION mci;
        mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
        mci.CallbackParam       = 0;

        ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pep != 0) ? &mdei : 0, NULL, &mci);

        CloseHandle(hFile);
    }
}

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    strcat(pluginPath , coreStr.toUtf8().data());
    MainDebug("%s",pluginPath);
    wchar_t name[2048] = {0};
    QString(pluginPath).toWCharArray(name);
    // CreateMiniDump(pExceptionInfo, (LPCTSTR)(LPCSTR)(pluginPath));
    CreateMiniDump(pExceptionInfo, (name));

    return EXCEPTION_EXECUTE_HANDLER;
}

// 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效
void DisableSetUnhandledExceptionFilter()
{
    void* addr = (void*)GetProcAddress(LoadLibrary(TEXT("kernel32.dll")),
                                       "SetUnhandledExceptionFilter");

    if (addr)
    {
        unsigned char code[16];
        int size = 0;

        code[size++] = 0x33;
        code[size++] = 0xC0;
        code[size++] = 0xC2;
        code[size++] = 0x04;
        code[size++] = 0x00;

        DWORD dwOldFlag, dwTempFlag;
        VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
        WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
        VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
    }


}

void InitMinDump()
{
    //注册异常处理函数
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    //使SetUnhandledExceptionFilter
    DisableSetUnhandledExceptionFilter();
}

QStringList DirFileListXml(QString xmlName,QString Path) //文件list
{	//xmlName 文件类型 Path 路径

    QStringList fileList;
    QDir dir(Path);
    if (!dir.exists()) return fileList;
    dir.setFilter(QDir::Dirs|QDir::Files);
    //dir.setSorting(QDir::DirsFirst);
    dir.setSorting(QDir::Time |QDir::Reversed);
    //排序方式 修改时间从小到大
    QFileInfoList list = dir.entryInfoList();
    int i=0,filecont=0;
    do{
        QFileInfo fileInfo = list.at(i);

        if(fileInfo.fileName() == "." || fileInfo.fileName()== "..")
        {
            qDebug()<<"filedir="<<fileInfo.fileName();
            i++;
            continue;
        }

        bool bisDir=fileInfo.isDir();
        if(bisDir)
        { }
        else
        {

            QString currentFileName=fileInfo.fileName();
            bool Reght=currentFileName.endsWith(xmlName, Qt::CaseInsensitive);
            if(Reght)
            {
                fileList<<currentFileName;
                qDebug()<<"filelist sort="<<currentFileName;
                filecont++;
            }
        }


        i++;
    }while(i<list.size());
    return fileList;
}

void DeleteYesterdayLog(QStringList &fileList, QString currentTime)
{
    QString YMDHTime = currentTime.left(10);
    double currentTimeDouble = YMDHTime.toDouble();

    for(int i=0; i<fileList.size(); i++)
    {
        MainDebug("FileList i=%d file name:%s", i, fileList[i].toUtf8().data());
        if(fileList[i].indexOf(".log") != -1 || fileList[i].indexOf("log.txt") != -1 || fileList[i].indexOf(".zip") != -1)
        {
            QString commonStr = fileList[i].left(10);
            double fileTimeDouble = commonStr.toDouble();
            if(currentTimeDouble -fileTimeDouble > 100)
            {
                QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/"+fileList[i]);
            }

        }
    }
}

bool checkSingleExe();

bool setConfigFromParam(char *strJSON);

int main(int argc, char *argv[])
{

    bool firstRun = true;
    bool isHost = false;
    while(!isHost)
    {

        QApplication *app = new QApplication(argc, argv);
        InitMinDump();

        /*
     *  step 1: set default codec as GBK
     */
        QTextCodec *codec = QTextCodec::codecForLocale();
        QTextCodec::setCodecForTr(codec);
        QTextCodec::setCodecForCStrings(codec);


        /*
     *  step 2: init singletons
     */

        QThread::currentThread()->setObjectName(QString("mainThread-") + QString::number((int)(QThread::currentThreadId())));

        ConfigHelper *cfg       = ConfigHelper::getInstance();
        LyricParse *lrc         = LyricParse::getInstance();
        HttpDownload *hd        = HttpDownload::getInstance();
        PkInfoManager *info     = PkInfoManager::getInstance();
        YYNotificationCenter *yync = YYNotificationCenter::getInstance();
        NodeNotificationCenter *nodeNC = NodeNotificationCenter::getInstance();
        StateController *stateManager =  StateController::getInstance();

        /*
     *  step 3: parse YY command line parameters
     */
        if(firstRun)
        {
            QByteArray param = "eyJjaGFubmVsSUQiOiIxNDI4MTY4NiIsImhvc3QiOnsiSUQiOjEwMDY3OTI0MDYsIm5hbWUiOiJoYndhbmcyIiwicGljIjoiIn0sInVzZXIiOnsiSUQiOjEwMDY3OTI0MDYsIm5hbWUiOiJoYndhbmcyIiwiYklzSG9zdCI6dHJ1ZSwicGljIjoiIn0sImNvbmZpZyI6eyJwb3J0Ijo2MzU1NywicGx1Z2luUGF0aCI6IkM6XFxVc2Vyc1xcd2hiXFxBcHBEYXRhXFxSb2FtaW5nXC9kdW93YW5cL3l5XC9idXNpbmVzc1wveXltdXNpY2NvbmZpZ1wvNF81XC8yMjIxXC85MDhcL3BsdWdpblwvaUhvdVBsdWdpbi5leGUiLCJ3aW5JRCI6IjE5ODg5NCJ9LCJhdXRoIjp7InBsYWluIjoiMjAxNC0wOC0xNCAxNToxMjoyNSIsImNpcGhlciI6IlwvZHA5WTRMYm83YVI4V2JHN2JNcEdHS2s5TW1lcWxiWnViU09Qak5tR0pRPSIsImd1aWQiOiJ7NDJBRkRFRDItRTdENS00MzZELThERDUtMTQ2Q0I3ODg5MTI4fSJ9fQ==";

            MainDebug("param is: %s",QString::fromUtf8(QByteArray::fromBase64(argv[1]).data()).toUtf8().data());
//            if (!setConfigFromParam(QByteArray::fromBase64(param).data()))
            if (!setConfigFromParam(QByteArray::fromBase64(argv[1]).data()))
            {
                MainDebug("parse command line parameters failed!");
                //return -1;
            }

            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString appExeBeginTime = currentDateTime.toString("yyyyMMddhhmmsszzz")+QString("_%1_%2_").arg(PkInfoManager::getInstance()->getMe().getID()).arg(PkInfoManager::getInstance()->getChannelID());

            /*
     * log4qt start on
     */
            QFile versionFile;
            versionFile.setFileName(ConfigHelper::getInstance()->getPluginPath() + "version.config");
            QString mainVersion = "iHouPlugin_1.0.";
            QString versionStr = "Default";
            if(versionFile.open(QFile::ReadOnly))
            {
                MainDebug("version file exist~~");
                QByteArray versionArray = versionFile.readAll();
                versionStr = QString::fromUtf8(versionArray);
                versionFile.close();
            }
            QString logFileName = appExeBeginTime + mainVersion + versionStr + ".log";
            if(!Log4Qt::PropertyConfigurator::configure(ConfigHelper::getInstance()->getPluginPath() , logFileName))
            {
                //      printf("configure false!!!\n");
            }
            else
            {
                //      printf("configure true!!!\n");
            }

            Log4Qt::LogManager::setHandleQtMessages(true);
            MainDebug("YY winId is 0x%x",(unsigned long)cfg->getYYHwnd());
            /*
     *  step 4: upload dumpfile
     */
            cfg->setLogFileName(appExeBeginTime + mainVersion + versionStr + ".log");
            MainDebug("appExeBeginTime %s", appExeBeginTime.toUtf8().data());

            coreStr = "log/"+appExeBeginTime + mainVersion + versionStr +"core.dmp";
            MainDebug("coreStr %s", coreStr.toUtf8().data());

            //get yy log
            //    QProcess *process = new QProcess;
            //    QStringList logArgv;
            //    logArgv.append(ConfigHelper::getInstance()->getPluginPath() + "\\log");
            //    logArgv.append(QString::number(PkInfoManager::getInstance()->getMe().getID()));
            //    process->start(ConfigHelper::getInstance()->getPluginPath() + "\\AnalyzeLog.exe", logArgv);

            HttpUpLoad *httpUpload = new HttpUpLoad();
            QStringList fileList = DirFileListXml("", ConfigHelper::getInstance()->getPluginPath()+"log/");

            for(int i=0; i<fileList.size(); i++)
            {
                if(fileList[i].indexOf("core.dmp") != -1)
                {
                    QString commonStr = fileList[i].left(37);
                    //           commonStr += "iHouPlugin1.0.5.0.log";
                    for(int j=0; j<fileList.size(); j++)
                    {
                        if(fileList[j].indexOf(commonStr) != -1)
                        {
                            MainDebug("has dump and log file");
                            httpUpload->zipDumpFile(ConfigHelper::getInstance()->getPluginPath()+"log/", fileList[i], fileList[j]);
                            MainDebug("start setDumpHeaderPart");
                            httpUpload->setDumpHeaderPart();
                            break;
                        }
                    }
                }
            }


            DeleteYesterdayLog(fileList, appExeBeginTime);


            strcpy(pluginPath , ConfigHelper::getInstance()->getPluginPath().toUtf8().data());

            MainDebug("param is: %s",argv[1]);

            //    /*
            //     *  step 5: connect to YY
            //     */
            if (!yync->start(cfg->getYYClientPort()))
            {
                MainDebug("connot connect to YY!");
                return -1;
            }


            // send GUID to yync
            yync->sendCheckConnection(cfg->getYYCheckGUID());
        }

        /*
     * init plugin state
     */
        stateManager->startStateMachine();

        /*
     * init ExceptionController
     */
        ExceptionController *exceptionController = ExceptionController::getInstance();

        /*
     *  step 6: init UI
     */
        HostMainWidget *hostWidget;
        AudienceMainWidget *audienceWidget;


        if (info->getMe().getIsHost())
        {
            MainDebug("Host created");
            isHost = true;
            hostWidget = new HostMainWidget;
            //change by zhaoli 2014.5.18
            hostWidget->showHostMainWidget();
            exceptionController->setMainWidget(hostWidget);
            if(firstRun)
                yync->sendHostCreateGame();
        }
        else
        {
            audienceWidget = new AudienceMainWidget;
            if(firstRun)
                yync->sendAudienceJoinGame();
            exceptionController->setMainWidget(audienceWidget);
        }

        /*
     *  step 7: connect to NodeServer
     */
        if (info->getMe().getIsHost())
        {
            if (info->getHostPlayer().getID() == 0)
            {
                MainDebug("quit cause me is Host but Host ID is 0");
                return -1;
            }
            if (!nodeNC->start(cfg->getNodeServerIP(),cfg->getNodeServerPort(),
                               cfg->getStatusServerIP(),cfg->getStatusServerPort(),
                               info->getChannelID(),info->getMe()))
            {
                MainDebug("cannot connect to node!");
                return -1;
            }
        }
        else
        {
            if (!nodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                               cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                               info->getChannelID(), info->getMe()))
            {
                MainDebug("cannot connect to node!");
                return -1;
            }
        }

        /*
     *  step 8: run app event loop
     */
        app->exec();
        app->deleteLater();
        app = NULL;
        /*
     *  step 9: clean up
     */
        hd->destroy();
        lrc->destory();
        cfg->destroy();
        info->destory();

        yync->stop();
        yync->destory();
        nodeNC->stop();
        nodeNC->destory();

        stateManager->stopStateMachine();
        stateManager->destory();

        if(firstRun)
            firstRun = false;
    }

}
#include <TlHelp32.h>
#include <process.h>
bool checkSingleExe()
{
    PROCESSENTRY32 pe32;
    //在使用这个结构前，先设置它的大小
    pe32.dwSize = sizeof(pe32);
    //给系统内所有的进程拍个快照
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        MainDebug("CreateToolhelp32Snapshot 调用失败.\n");
        return -1;
    }
    //遍历进程快照，轮流显示每个进程的信息
    BOOL bMore = ::Process32First(hProcessSnap,&pe32);
    bool bSingleExe = true;
    while (bMore)
    {
        if(QString::fromWCharArray(pe32.szExeFile) == "iHouPlugin.exe" && (int)getpid() != pe32.th32ProcessID)
        {
            bSingleExe = false;
            break;
        }
        bMore = ::Process32Next(hProcessSnap,&pe32);
    }
    //不要忘记清除掉snapshot对象
    ::CloseHandle(hProcessSnap);
    return bSingleExe;
}

#include "Modules/JsonPrase/cJSON.h"
#include "Modules/ihouAuth/ihouAuth.h"
bool setConfigFromParam(char *strJSON)
{
    /*
    {
        "channelID": "90131313",
        "host":
        {
            "ID": 121132322,
            "name": "主播名"，
            "pic": "c:\\YY\\hostHeadPic.png"
        },
        "user":
        {
            "ID": 1231231123,
            "name": "用户xx",
            "bIsHost": false,
            "pic": "c:\\YY\\headPic.png"
        },
        "config":
        {
            "port": 6418,
            "pluginPath": "c:\\YY\\plugins\\iHou"
        },
        "auth":
        {
            "plain": "2014-05-01 08:00:00",
            "cipher": "slkfsfjsfsfsfsf123131",
            "guid": "123_1231_31213"
        }
    }
    */
    MainDebug("strJSON is: %s", QString::fromUtf8(strJSON).toUtf8().data());

    ConfigHelper *cfg       = ConfigHelper::getInstance();
    PkInfoManager *info     = PkInfoManager::getInstance();

    QString plainText;
    QString cipherText;

    cJSON *jsRoot = cJSON_Parse(strJSON);
    {
        MainDebug("jsRoot is: @%d", jsRoot);

        qint64 channelID = QString::fromUtf8(cJSON_GetObjectItem(jsRoot, "channelID")->valuestring).toLongLong();
        info->setChannelID(channelID);
        MainDebug("channelID is: %d", channelID);

        cJSON *jsHost = cJSON_GetObjectItem(jsRoot, "host");
        {
            Player host;
            host.setID(cJSON_GetObjectItem(jsHost, "ID")->valueint);
            host.setName(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "name")->valuestring));
            host.setPicFileSvPath(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "pic")->valuestring));

            MainDebug("host.getID() is: %d", host.getID());
            MainDebug("host.getName() is: %s", host.getName().toUtf8().data());
            MainDebug("host.getPic() is: %s", host.getPicFileSvPath().toUtf8().data());
            info->setHostPlayer(host);
        }

        cJSON *jsUser = cJSON_GetObjectItem(jsRoot, "user");
        {
            Player me;
            me.setID(cJSON_GetObjectItem(jsUser, "ID")->valueint);
            MainDebug("me.getID() is: %d", me.getID());

            me.setName(QString::fromUtf8(cJSON_GetObjectItem(jsUser, "name")->valuestring));
            MainDebug("me.getName() is: %s", me.getName().toUtf8().data());

            me.setIsHost(cJSON_GetObjectItem(jsUser, "bIsHost")->type == cJSON_True);
            MainDebug("me.getPic() is: %s", me.getPicFileSvPath().toUtf8().data());

            me.setPicFileSvPath(QString::fromUtf8(cJSON_GetObjectItem(jsUser, "pic")->valuestring));
            MainDebug("me.getIsHost() is: %s", me.getIsHost() ? "true" : "false");

            info->setMe(me);
        }

        cJSON *jsConfig = cJSON_GetObjectItem(jsRoot, "config");
        {
            cfg->setYYClientPort(cJSON_GetObjectItem(jsConfig, "port")->valueint);
            MainDebug("port is: %d", cfg->getYYClientPort());
            QString pluginFullPath = QString::fromUtf8(cJSON_GetObjectItem(jsConfig, "pluginPath")->valuestring);
            pluginFullPath.remove("iHouPlugin.exe");
            pluginFullPath.replace("\\","/");
            cfg->setPluginPath(pluginFullPath);
            MainDebug("pluginPath is: %s", cfg->getPluginPath().toUtf8().data());

            QString yyWid = QString::fromUtf8(cJSON_GetObjectItem(jsConfig , "winID")->valuestring);
            cfg->setYYHwnd(::GetParent(::GetParent((HWND)yyWid.toULong())));
            MainDebug("YY winId is 0x%x",(unsigned long)cfg->getYYHwnd());
        }

        cJSON *jsAuth = cJSON_GetObjectItem(jsRoot, "auth");
        {
            plainText = QString::fromUtf8(cJSON_GetObjectItem(jsAuth, "plain")->valuestring);
            MainDebug("plain is: %s", plainText.toUtf8().data());

            cipherText = QString::fromUtf8(cJSON_GetObjectItem(jsAuth, "cipher")->valuestring);
            MainDebug("cipher is: %s", cipherText.toUtf8().data());

            cfg->setYYCheckGUID(QString::fromUtf8(cJSON_GetObjectItem(jsAuth, "guid")->valuestring));
            MainDebug("guid is: %s", cfg->getYYCheckGUID().toUtf8().data());
        }
    }
    cJSON_Delete(jsRoot);

    // cipher -> DES -> plain
    char buf[1024];
    int len = decrypt(cipherText.toUtf8().data(), cipherText.length(), buf);
    QString decryptPlainText = QString::fromUtf8(buf, len);
    MainDebug("decryptPlainText is: %s", decryptPlainText.toUtf8().data());
    if (decryptPlainText != plainText + "ihou plugin")
    {
        MainDebug("auth failed!");
        return false;
    }
    QDateTime dateTimeYY = QDateTime::fromString(decryptPlainText.replace("ihou plugin", ""), "yyyy-MM-dd hh:mm:ss");
    QDateTime dateTimePlugin = QDateTime::currentDateTime();
    MainDebug("dateTimeYY is: %s, dateTimePlugin is: %s",
              dateTimeYY.toString().toUtf8().data(), dateTimePlugin.toString().toUtf8().data());

    qint64 secondGap = dateTimeYY.secsTo(dateTimePlugin);
    //    MainDebug("millionSecondGap is: %lld", secondGap);
    //    if (secondGap < 0 || secondGap > 10)
    //    {
    //        MainDebug("auth failed cause time!");
    //        return false;
    //    }

    MainDebug("auth succeed!");

    return true;
}
