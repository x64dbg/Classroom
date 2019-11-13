#include "PluginDialog.h"
#include "ui_PluginDialog.h"
#include <QMessageBox>
#include <QDir>
#include <shlwapi.h>
#include "Classroom.h"
#include "pluginmain.h"

PluginDialog::PluginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::PluginDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(size());

    _plugin_logprintf("[QtPlugin] Current directory: \"%s\"\n", QDir::currentPath().toUtf8().constData());
}

PluginDialog::~PluginDialog()
{
    delete ui;
}
