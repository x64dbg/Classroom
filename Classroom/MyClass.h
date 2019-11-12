#pragma once
#include "pluginmain.h"
#include <QtCore>

class MyClassMemberVariable
{
public:
    QString label;
    QString vartype;
    QString comment;
};

class MyClass
{
public:
    MyClass();
    QString name;//name of this class.
    QString comment;//description of this class.
    int size;//size of this class. -1 if unknown.
    //Labeling the member function starting with "name::" will make it a member of the class.
    QSet<duint> memberfunction;//VA of known member function.
    //TODO: tracking member function usages in functions;
    QList<std::pair<int,MyClassMemberVariable> > membervariable;//member variable.
    //TODO: tracking member variable usages in functions;
    //QSet<duint> knowninstance;//VA of known instances of this class. Label is stored by x64dbg. Comment stored by x64dbg is description.
};
