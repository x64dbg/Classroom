#ifndef MISCUTIL_H
#define MISCUTIL_H
#include <QString>
#include "pluginsdk/bridgemain.h"
class QWidget;
class QIcon;

void SimpleErrorBox(QWidget* parent, const QString & title, const QString & text);
bool SimpleInputBox(QWidget* parent, const QString & title, QString defaultValue, QString & output, const QString & placeholderText, QIcon* icon = nullptr);
inline QString ToPtrString(duint Address)
{
    //
    // This function exists because of how QT handles
    // variables in strings.
    //
    // QString::arg():
    // ((int32)0xFFFF0000) == 0xFFFFFFFFFFFF0000 with sign extension
    //

    char temp[33];
#ifdef _WIN64
    sprintf_s(temp, "%016llX", Address);
#else
    sprintf_s(temp, "%08X", Address);
#endif // _WIN64
    return QString::fromUtf8(temp);
}
#endif // MISCUTIL_H
