#include "Classroom.h"
#include "PluginDialog.h"
#include "PluginTabWidget.h"
#include "pluginmain.h"
#include "AddClass.h"
#include "AddMemberVariable.h"
#include "MyClass.h"
#include <QFile>

static PluginDialog* pluginDialog;
static PluginTabWidget* pluginTabWidget;
static HANDLE hSetupEvent;
static HANDLE hStopEvent;
/*
static QByteArray getResourceBytes(const char* path)
{
    QByteArray b;
    QFile s(path);
    if(s.open(QFile::ReadOnly))
        b = s.readAll();
    return b;
}
*/
static QWidget* getParent()
{
    return QWidget::find((WId)Plugin::hwndDlg);
}

void QtPlugin::Init()
{
    hSetupEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
}

void QtPlugin::Setup()
{
    QWidget* parent = getParent();
    pluginDialog = new PluginDialog(parent);
    pluginDialog->show();
    pluginTabWidget = new PluginTabWidget(parent);
    GuiAddQWidgetTab(pluginTabWidget);

    SetEvent(hSetupEvent);
}

void QtPlugin::Show()
{
    pluginDialog->show();
}

void QtPlugin::WaitForSetup()
{
    WaitForSingleObject(hSetupEvent, INFINITE);
    CloseHandle(hSetupEvent);
    hSetupEvent = NULL;
}

void QtPlugin::Stop()
{
    GuiCloseQWidgetTab(pluginTabWidget);
    pluginTabWidget->close();
    pluginDialog->close();
    delete pluginDialog;
    delete pluginTabWidget;

    SetEvent(hStopEvent);
}

void QtPlugin::WaitForStop()
{
    WaitForSingleObject(hStopEvent, INFINITE);
    CloseHandle(hStopEvent);
    hStopEvent = NULL;
}

void QtPlugin::ShowTab()
{
    GuiShowQWidgetTab(pluginTabWidget);
}

void QtPlugin::Refresh()
{
    if(pluginTabWidget) pluginTabWidget->refresh();
}

void QtPlugin::RefreshClasses()
{
    if(pluginTabWidget) pluginTabWidget->refreshClasses();
}

void QtPlugin::cbInit()
{
    for(auto & i : Plugin::classroom)
        delete i;
    Plugin::classroom.clear();
}

void QtPlugin::cbStop()
{
    for(auto & i : Plugin::classroom)
        delete i;
    Plugin::classroom.clear();
    Refresh();
}

void QtPlugin::cbClassroom(char* userdata)
{
    AddClass dialog;
    if(userdata != nullptr)
    {
        dialog.setClassName(QString::fromUtf8((char*)userdata));
        free(userdata);//userdata was allocated with _strdup()
    }
    dialog.exec();
}

void QtPlugin::cbClassMemberVar(std::pair<MyClass*, int>* userdata)
{
    AddMemberVariable dialog;
    dialog.Init(userdata->first, userdata->second);
    delete userdata;
    dialog.exec();
}
