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
    if(pluginTabWidget)
        pluginTabWidget->refresh();
    if(pluginDialog)
        pluginDialog->refresh();
}

void QtPlugin::RefreshClasses()
{
    if(pluginTabWidget)
        pluginTabWidget->refreshClasses();
    if(pluginDialog)
        pluginDialog->refreshClasses();
}

void QtPlugin::cbSelChanged(void* VA)
{
    duint start, end;
    MyClass* currentClass;
    if(DbgFunctionGet((duint)VA, &start, &end))
    {
        currentClass = Plugin::getCurrentClass();
        if(currentClass)
        {
            if(!currentClass->memberfunction.contains(start))
            {
                currentClass->memberfunction.insert(start);
                QtPlugin::Refresh();
            }
        }
    }
    if(pluginDialog)
        pluginDialog->selChanged((duint)VA);
}

void QtPlugin::cbInit()
{

}

void QtPlugin::cbStop()
{
    for(auto & i : Plugin::classroom)
        delete i;
    Plugin::classroom.clear();
    RefreshClasses();
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

void QtPlugin::cbDelClass(char* userdata)
{
    if(userdata != nullptr)
    {
        QString className = QString::fromUtf8((char*)userdata);
        free(userdata);//userdata was allocated with _strdup()
        for(int i = 0; i < Plugin::classroom.size(); i++)
        {
            if(Plugin::classroom.at(i)->name == className)
            {
                delete Plugin::classroom[i];
                Plugin::classroom.removeAt(i);
                RefreshClasses();
                return;
            }
        }
    }
}

void QtPlugin::cbDelClassMemberVar(std::pair<MyClass*, int>* userdata)
{
    auto & list = userdata->first->membervariable;
    for(int i = 0; i < list.size(); i++)
    {
        if(list.at(i).first == userdata->second)
        {
            delete userdata;
            list.removeAt(i);
            Refresh();
            return;
        }
    }
    delete userdata;
}
