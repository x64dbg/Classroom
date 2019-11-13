#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>
#include <windows.h>
#include "pluginmain.h"

namespace Ui
{
class PluginDialog;
}

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginDialog(QWidget* parent);
    ~PluginDialog();

public slots:
    void indexChaged(int index);
    void selChanged(duint VA);
    void refresh();
    void refreshClasses();

private:
    Ui::PluginDialog *ui;
};

#endif // PLUGINDIALOG_H
