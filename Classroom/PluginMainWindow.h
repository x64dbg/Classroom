#ifndef PLUGINMAINWINDOW_H
#define PLUGINMAINWINDOW_H

#include <QMainWindow>
class MyClass;

namespace Ui {
class PluginMainWindow;
}

class PluginMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PluginMainWindow(QWidget* parent = nullptr);
    ~PluginMainWindow();

    //HTML generation
    QString currentHtml();
    QString classHtml();
    QString placeholderHtml();
    QString aboutHtml();
    QString functionHtml(MyClass* currentClass);
    QString memberHtml(MyClass* currentClass);

public slots:
    void refresh();
    void refreshClasses();
    void anchorClicked(const QUrl& link);
    void showDialogClicked();

private:
    Ui::PluginMainWindow* ui;
};

#endif // PLUGINMAINWINDOW_H
