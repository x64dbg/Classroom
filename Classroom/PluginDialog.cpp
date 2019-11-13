#include "PluginDialog.h"
#include "ui_PluginDialog.h"
#include "Classroom.h"
#include "pluginmain.h"
#include "MyClass.h"

PluginDialog::PluginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::PluginDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(size());
    int w;
    w = fontMetrics().maxWidth();
    connect(ui->classes, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChaged(int)));
    ui->memberVarList->setColumnWidth(0, 4 * w); //offset
    ui->memberVarList->setColumnWidth(1, 10 * w); //name
    ui->memberVarList->setColumnWidth(2, 6 * w); //type
#ifdef _WIN64
    ui->memberVarList->setColumnWidth(3, 16 * w); //value
#else //x86
    ui->memberVarList->setColumnWidth(3, 8 * w); //value
#endif //_WIN64
    ui->memberVarList->setColumnWidth(4, 30 * w); //comment
}

PluginDialog::~PluginDialog()
{
    delete ui;
}

void PluginDialog::refreshClasses()
{
    QStringList items;
    ui->classes->clear();
    for(auto & i : Plugin::classroom)
        items.append(i->name);
    ui->classes->addItems(items);
}

void PluginDialog::refresh()
{
    ui->memberVarList->clear();
    const MyClass* currentClass;
    currentClass = Plugin::getClassByName(ui->classes->currentText());
    if(currentClass)
    {
        for(auto & i : currentClass->membervariable)
        {
            QTreeWidgetItem* item;
            QStringList columns;
            columns << QString::number(i.first, 16);
            columns << i.second.label;
            columns << i.second.vartype;
            columns << "Value";//dummy value
            columns << i.second.comment;
            item = new QTreeWidgetItem(ui->memberVarList, columns);
            ui->memberVarList->addTopLevelItem(item);
        }
    }
}

void PluginDialog::indexChaged(int index)
{
    refresh();
}

void PluginDialog::selChanged(duint VA)
{
    if(!DbgIsDebugging())
        return;
    char txtbuffer[MAX_COMMENT_SIZE];
    duint start, end;
    if(DbgFunctionGet(VA, &start, &end))
    {
        DbgGetLabelAt(start, SEG_DEFAULT, txtbuffer);
        ui->memberFunc->setText(QString::fromUtf8(txtbuffer));
        DbgGetCommentAt(start, txtbuffer);
        ui->comment->setText(QString::fromUtf8(txtbuffer));
    }
    //Use Zydis to diassemble current instruction to find out what is the most relevant item.
}
