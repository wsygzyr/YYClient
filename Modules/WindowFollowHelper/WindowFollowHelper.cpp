#include "WindowFollowHelper.h"
#include <TlHelp32.h>
#include <process.h>
#include <Windows.h>
QList<DWORD> FindProcessByName(QString processName)
{
    QList<DWORD> processIdList;
    PROCESSENTRY32 pe32;
    //在使用这个结构前，先设置它的大小
    pe32.dwSize = sizeof(pe32);
    //给系统内所有的进程拍个快照
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        WindowFollowHelperDebug("CreateToolhelp32Snapshot 调用失败.\n");
        return processIdList;
    }
    //遍历进程快照，轮流显示每个进程的信息
    BOOL bMore = ::Process32First(hProcessSnap,&pe32);
    while (bMore)
    {
        //        WindowFollowHelperDebug("%s", QString::fromWCharArray(pe32.szExeFile).toUtf8().data());
        if(QString::fromWCharArray(pe32.szExeFile).toLower() == processName.toLower() )
        {
            WindowFollowHelperDebug("%s", QString::fromWCharArray(pe32.szExeFile).toUtf8().data());
            processIdList.append(pe32.th32ProcessID);
        }
        bMore = ::Process32Next(hProcessSnap,&pe32);
    }
    //不要忘记清除掉snapshot对象
    ::CloseHandle(hProcessSnap);
    return processIdList;
}
typedef struct EnumFunArg
{
    HWND parentHwnd;
    HWND hWnd;
    QList<DWORD> dwProcessIdList;

}EnumFunArg;

BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
    EnumFunArg *pArg = reinterpret_cast<EnumFunArg *> (lParam);
    DWORD  dwProcessId;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    foreach (DWORD id, pArg->dwProcessIdList) {
        if( dwProcessId == id  )
        {
            WindowFollowHelperDebug("id is %x , hwnd is %x" , id , hwnd);
            HWND child = ::FindWindowExA(hwnd,NULL,NULL,("Music_pannel_parent"));
            if(child)
            {
                WindowFollowHelperDebug("find child, id is %x", child);
                pArg->parentHwnd = hwnd;
                pArg->hWnd = child;
                return FALSE;
            }
            // 注意：当查找到了，应该返回FALSE中止枚举下去
        }
    }
    return TRUE;//继续枚举下去直到所有顶层窗口枚举完为止
}

EnumFunArg myGetWindowByProcessId( QList<DWORD> dwProcessIdList )
{
    EnumFunArg arg;
    arg.dwProcessIdList = dwProcessIdList;
    arg.parentHwnd = 0;
    arg.hWnd = 0;
    EnumWindows(lpEnumFunc,reinterpret_cast<LPARAM>(&arg));
    return arg;
}


WindowFollowHelper::WindowFollowHelper(QObject *parent) :
    QObject(parent)
{
    YYParentWindow = NULL;
    PluginWindow = NULL;
    cfg = ConfigHelper::getInstance();
}

void WindowFollowHelper::setFollowWidget(QWidget *widget)
{
    this->widget = widget;

    widget->setMouseTracking(true);
    HWND yyhwand = cfg->getYYHwnd();
    if(yyhwand)
    {
        PluginWindow = (HWND)this->widget->winId();
        YYParentWindow = yyhwand;
        WindowFollowHelperDebug("parentWinId:%lu, curWinId:%lu", YYParentWindow, PluginWindow);
    }

    // ::SetWindowPos(YYWindow, YYParentWindow, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
    //::ShowWindow(YYWindow, SW_MINIMIZE);
}

void WindowFollowHelper::startFollow()
{
    if(YYParentWindow)
    {
        WINDOWPLACEMENT placeMent;
        ::SetParent(PluginWindow , YYParentWindow);
//        ::GetWindowPlacement(YYParentWindow,&placeMent);
//        lastShowCmd = placeMent.showCmd;
//        if(placeMent.showCmd != SW_SHOWMINIMIZED)
//        {
//            RECT rect;
//            ::GetWindowRect(YYParentWindow , &rect);

//            widget->move(rect.left,(rect.bottom - widget->height() - 40 ));
//        }
//        this->startTimer(10);
    }

}

void WindowFollowHelper::stopFollow()
{
    QPoint point = widget->mapToGlobal(QPoint(0 ,0));
    ::SetParent((HWND)widget->winId() , 0);
    widget->move(point);
}

QPoint WindowFollowHelper::getYYParentPos()
{
    if(YYParentWindow)
    {
        RECT rect;
        ::GetWindowRect(YYParentWindow , &rect);
        return QPoint(rect.left , rect.top);
    }
    else
    {
        return QPoint(0,0);
    }
}

QPoint WindowFollowHelper::getSelfPos()
{
    if(YYParentWindow)
    {
        RECT rect;
        ::GetWindowRect((HWND)widget->winId() , &rect);
        return QPoint(rect.left , rect.top);
    }
    else
    {
        return widget->pos();
    }
}

QRect WindowFollowHelper::getYYParentRect()
{
    if(YYParentWindow)
    {
        RECT rect;
        ::GetWindowRect((HWND)widget->winId() , &rect);
        return QRect(rect.left , rect.top , rect.right - rect.left, rect.bottom - rect.top );
    }
}

void WindowFollowHelper::raiseUpYYWindow()
{

}


void WindowFollowHelper::timerEvent(QTimerEvent *)
{
    if(widget->isHidden())
    {
        return;
    }
    HWND hd = ::GetNextWindow(YYParentWindow, GW_HWNDPREV);
    if(hd != PluginWindow)
    {
        ::SetWindowPos(PluginWindow, hd, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
    }

    //::SetWindowPos(YYWindow, YYParentWindow, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
#if 0
    WINDOWPLACEMENT placeMent;
    BOOL ret = ::GetWindowPlacement(YYParentWindow,&placeMent);
    if(ret == TRUE)
    {
        ::SetWindowPos(YYWindow, YYParentWindow, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
        if( (placeMent.showCmd == SW_SHOWMAXIMIZED || placeMent.showCmd == SW_SHOWDEFAULT || placeMent.showCmd == SW_SHOWNORMAL) && placeMent.showCmd != lastShowCmd)
        {
            WindowFollowHelperDebug("cmd is %s",(placeMent.showCmd == SW_SHOWMAXIMIZED) ? "SW_SHOWMAXIMIZED" : ((placeMent.showCmd == SW_SHOWDEFAULT) ? "SW_SHOWDEFAULT" : "SW_SHOWNORMAL"));
            //::BringWindowToTop((HWND)widget->winId());
            widget->move(getSelfPos());
            lastShowCmd = placeMent.showCmd;
            return;
        }
    }


#endif
}

bool WindowFollowHelper::checkYYIsUnderPlugin()
{
    HWND hwnd =  ::GetNextWindow(PluginWindow , GW_HWNDNEXT);
    if(GetParent(hwnd) == YYParentWindow)
        return true;
    else
        return false;
}
