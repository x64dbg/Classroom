#ifndef PLUGINTABWIDGET_H
#define PLUGINTABWIDGET_H

#include <QWidget>
#include "PluginMainWindow.h"

class PluginTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginTabWidget(QWidget* parent = 0);

public slots:
    void refresh();
    void refreshClasses();

protected:
    void closeEvent(QCloseEvent* event);

private:
    PluginMainWindow* mPluginMainWindow;
};

#endif // PLUGINTABWIDGET_H
