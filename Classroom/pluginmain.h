#pragma once

#include <windows.h>
#include "pluginsdk\_plugins.h"
#include <qlist.h>
#include "MiscUtil.h"
class MyClass;

namespace Plugin
{
    extern int handle;
    extern HWND hwndDlg;
    extern int hMenu;
    extern int hMenuDisasm;
    extern int hMenuDump;
    extern int hMenuStack;
    extern int hMenuGraph;
    extern int hMenuMemmap;
    extern int hMenuSymmod;
    extern QList<MyClass*> classroom;

    QString getCurrentClassName();
    MyClass* getCurrentClass();
    MyClass* getClassByName(const QString & className);
} //Plugin
