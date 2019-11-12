#ifndef QTPLUGIN_H
#define QTPLUGIN_H
#include <utility>
class MyClass;

namespace QtPlugin
{
    void Init();
    void Setup();
    void Show();
    void WaitForSetup();
    void Stop();
    void WaitForStop();
    void ShowTab();
    void Refresh();
    void RefreshClasses();
    //Callback
    void cbInit();
    void cbStop();
    void cbClassroom(char* userdata);
    void cbClassMemberVar(std::pair<MyClass*, int>* userdata);
} //QtPlugin

#endif // QTPLUGIN_H
