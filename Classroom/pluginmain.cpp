#include "pluginmain.h"
#include "Classroom.h"
#include "MyClass.h"

#define plugin_name "Classroom"
#define plugin_version 1

int Plugin::handle;
HWND Plugin::hwndDlg;
int Plugin::hMenu;
int Plugin::hMenuDisasm;
int Plugin::hMenuDump;
int Plugin::hMenuStack;
int Plugin::hMenuGraph;
int Plugin::hMenuMemmap;
int Plugin::hMenuSymmod;
QList<MyClass*> Plugin::classroom;
//Callback
void cbPauseDebug(CBTYPE cbType, void* reserved);
void cbStopDebug(CBTYPE cbType, void* reserved);
void cbInitDebug(CBTYPE cbType, void* arg);
void cbSelChanged(CBTYPE cbType, PLUG_CB_SELCHANGED* arg);
void cbLoadDb(CBTYPE cbType, PLUG_CB_LOADSAVEDB* arg);//see Database.cpp
void cbSaveDb(CBTYPE cbType, PLUG_CB_LOADSAVEDB* arg);//see Database.cpp
//Command
bool cmdClassroom(int argc, char** argv);
bool cmdClassMemberVar(int argc, char** argv);
bool cmdDelClass(int argc, char** argv);
bool cmdDelClassMemberVar(int argc, char** argv);

extern "C" __declspec(dllexport) bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    initStruct->pluginVersion = plugin_version;
    initStruct->sdkVersion = PLUG_SDKVERSION;
    strcpy_s(initStruct->pluginName, plugin_name);

    Plugin::handle = initStruct->pluginHandle;
    QtPlugin::Init();
    return true;
}

extern "C" __declspec(dllexport) void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    Plugin::hwndDlg = setupStruct->hwndDlg;
    Plugin::hMenu = setupStruct->hMenu;
    Plugin::hMenuDisasm = setupStruct->hMenuDisasm;
    Plugin::hMenuDump = setupStruct->hMenuDump;
    Plugin::hMenuStack = setupStruct->hMenuStack;
    Plugin::hMenuGraph = setupStruct->hMenuGraph;
    Plugin::hMenuMemmap = setupStruct->hMenuMemmap;
    Plugin::hMenuSymmod = setupStruct->hMenuSymmod;
    GuiExecuteOnGuiThread(QtPlugin::Setup);
    _plugin_registercallback(Plugin::handle, CB_INITDEBUG, cbInitDebug);
    _plugin_registercallback(Plugin::handle, CB_PAUSEDEBUG, cbPauseDebug);
    _plugin_registercallback(Plugin::handle, CB_STOPDEBUG, cbStopDebug);
    _plugin_registercallback(Plugin::handle, CB_SELCHANGED, (CBPLUGIN)cbSelChanged);
    _plugin_registercallback(Plugin::handle, CB_LOADDB, (CBPLUGIN)cbLoadDb);
    _plugin_registercallback(Plugin::handle, CB_SAVEDB, (CBPLUGIN)cbSaveDb);
    _plugin_registercommand(Plugin::handle, "classroom", cmdClassroom, true);
    _plugin_registercommand(Plugin::handle, "classmembervar", cmdClassMemberVar, true);
    _plugin_registercommand(Plugin::handle, "delclass", cmdDelClass, true);
    _plugin_registercommand(Plugin::handle, "delclassmembervar", cmdDelClassMemberVar, true);
    QtPlugin::WaitForSetup();
}

extern "C" __declspec(dllexport) bool plugstop()
{
    GuiExecuteOnGuiThread(QtPlugin::Stop);
    QtPlugin::WaitForStop();
    _plugin_unregistercallback(Plugin::handle, CB_INITDEBUG);
    _plugin_unregistercallback(Plugin::handle, CB_PAUSEDEBUG);
    _plugin_unregistercallback(Plugin::handle, CB_STOPDEBUG);
    _plugin_unregistercallback(Plugin::handle, CB_SELCHANGED);
    _plugin_unregistercallback(Plugin::handle, CB_LOADDB);
    _plugin_unregistercallback(Plugin::handle, CB_SAVEDB);
    _plugin_unregistercommand(Plugin::handle, "classroom");
    _plugin_unregistercommand(Plugin::handle, "classmembervar");
    _plugin_unregistercommand(Plugin::handle, "delclass");
    _plugin_unregistercommand(Plugin::handle, "delclassmembervar");
    return true;
}
//Functions
QString Plugin::getCurrentClassName()
{
    SELECTIONDATA sel;
    if(GuiSelectionGet(GUI_DISASSEMBLY, &sel))
    {
        duint start, end;
        char label[MAX_LABEL_SIZE];
        const char* substr;
        DbgGetLabelAt(DbgFunctionGet(sel.start,&start,&end) ? start : sel.start, SEG_DEFAULT, label);
        substr = strstr(label, "::");
        if(substr != nullptr)
            return QString::fromUtf8(label, substr - label);
        else
            return QString();
    }
    else
        return QString();
}

MyClass* Plugin::getClassByName(const QString & className)
{
    for(auto & i : Plugin::classroom)
    {
        if(i->name == className)
        {
            return i;
        }
    }
    return nullptr;
}

MyClass* Plugin::getCurrentClass()
{
    QString currentClass = getCurrentClassName();
    if(currentClass.size()>0)
        return getClassByName(currentClass);
    else
        return nullptr;
}
//Callbacks
void cbPauseDebug(CBTYPE cbType, void* reserved)
{
    GuiExecuteOnGuiThread(QtPlugin::Refresh);
}

void cbStopDebug(CBTYPE cbType, void* reserved)
{
    GuiExecuteOnGuiThread(QtPlugin::cbStop);
}

void cbInitDebug(CBTYPE cbType, void* arg)
{
    GuiExecuteOnGuiThread(QtPlugin::cbInit);
}

void cbSelChanged(CBTYPE cbType, PLUG_CB_SELCHANGED* arg)
{
    //cbSelChanged should execute on GUI thread.
    if(arg->hWindow == GUI_DISASSEMBLY) //TODO: references view (list all labels and go through them to update member functions)
    {
        QtPlugin::cbSelChanged((void*)arg->VA); //Should execute on GUI thread
    }
}
//Add a class or edit an existing class
//arg1: name of the class to edit
bool cmdClassroom(int argc, char** argv)
{
    char* newbuffer; //We duplicate the buffer because argv[1] is no longer valid when used by QtPlugin::cbClassroom
    newbuffer = _strdup(argc > 1 ? argv[1] : Plugin::getCurrentClassName().toUtf8().constData());
    GuiExecuteOnGuiThreadEx((GUICALLBACKEX)QtPlugin::cbClassroom, newbuffer);
    return true;
}
//Add or edit a class member variable
//arg1: name of the class
//[arg2]: offset of member variable(optional)
bool cmdClassMemberVar(int argc, char** argv)
{
    std::pair<void*, int>* newbuffer;
    if(argc < 2)
    {
        _plugin_logputs("Usage: classmembervar classname,offset. \"classname\" is the name of class. \"offset\" is optional offset of member variable to edit.");
        return false;
    }
    newbuffer = new std::pair<void*, int>;
    newbuffer->second = -1;
    if(argc > 2)
        sscanf(argv[2], "%X", &newbuffer->second);
    newbuffer->first = Plugin::getClassByName(QString::fromUtf8(argv[1]));
    if(newbuffer->first == nullptr)
    {
        delete newbuffer;
        _plugin_logputs("Cannot find the class specified.");
        return false;
    }
    else
    {
        GuiExecuteOnGuiThreadEx((GUICALLBACKEX)QtPlugin::cbClassMemberVar, newbuffer);
        return true;
    }
}

bool cmdDelClass(int argc, char** argv)
{
    char* newbuffer; //We duplicate the buffer because argv[1] is no longer valid when used by QtPlugin::cbClassroom
    if(argc < 2)
    {
        _plugin_logputs("Usage: delclass classname. \"classname\" is the name of class to delete.");
        return false;
    }
    newbuffer = _strdup(argc > 1 ? argv[1] : Plugin::getCurrentClassName().toUtf8().constData());
    GuiExecuteOnGuiThreadEx((GUICALLBACKEX)QtPlugin::cbDelClass, newbuffer);
    return true;
}

bool cmdDelClassMemberVar(int argc, char** argv)
{
    std::pair<void*, int>* newbuffer;
    if(argc < 3)
    {
        _plugin_logputs("Usage: delclassmembervar classname,offset. \"classname\" is the name of class. \"offset\" is offset of member variable to delete.");
        return false;
    }
    newbuffer = new std::pair<void*, int>;
    newbuffer->second = -1;
    sscanf(argv[2], "%X", &newbuffer->second);
    if(newbuffer->second == -1)
    {
        delete newbuffer;
        _plugin_logputs("offset is not valid.");
        return false;
    }
    newbuffer->first = Plugin::getClassByName(QString::fromUtf8(argv[1]));
    if(newbuffer->first == nullptr)
    {
        delete newbuffer;
        _plugin_logputs("Cannot find the class specified.");
        return false;
    }
    else
    {
        GuiExecuteOnGuiThreadEx((GUICALLBACKEX)QtPlugin::cbDelClassMemberVar, newbuffer);
        return true;
    }
}
