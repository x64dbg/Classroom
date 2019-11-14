#include "PluginDialog.h"
#include "ui_PluginDialog.h"
#include "Classroom.h"
#include "pluginmain.h"
#include "MyClass.h"

PluginDialog::PluginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::PluginDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(size());
    int w;
    w = fontMetrics().maxWidth();
    connect(ui->classes, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChaged(int)));
    connect(ui->instance, SIGNAL(returnPressed()), this, SLOT(refresh()));
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
        duint instanceAddr;
        bool instanceValid;
        if(ui->instance->text().size() > 0)
        {
            instanceAddr = DbgEval(ui->instance->text().toUtf8().constData(), &instanceValid);
            if(instanceValid)
            {
                if(DbgMemIsValidReadPtr(instanceAddr))
                    ui->instance->setStyleSheet("background-color:#FFFFFF"); //white
                else
                    ui->instance->setStyleSheet("background-color:#FFCCCC"); //red
            }
            else
                ui->instance->setStyleSheet("background-color:#FFCCCC"); //red
        }
        else
        {
            instanceAddr = 0;
            ui->instance->setStyleSheet("background-color:#FFFFFF"); //white
        }
        for(auto & i : currentClass->membervariable)
        {
            QTreeWidgetItem* item;
            QStringList columns;
            columns << QString::number(i.first, 16);
            columns << i.second.label;
            columns << i.second.vartype;
            if(ui->instance->text().size() > 0)
            {
                duint value;
                if(DbgMemRead(instanceAddr + (duint)i.first, &value, sizeof(duint)))
                    columns << ToPtrString(value);
                else
                    columns << "???";
            }
            else
                columns << "???";
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
    DISASM_INSTR instr;
    char txtbuffer[MAX_COMMENT_SIZE];
    duint start = 0, end = 0;
    const MyClass* currentClass;
    currentClass = Plugin::getClassByName(ui->classes->currentText());
    //Use Zydis to diassemble current instruction to find out what is the most relevant item.
    DbgDisasmAt(VA, &instr);
    if(instr.type == instr_branch)
        start = DbgGetBranchDestination(VA);
    else if(!DbgFunctionGet(VA, &start, &end))
        start = 0;
    if(start != 0)
    {
        DbgGetLabelAt(start, SEG_DEFAULT, txtbuffer);
        ui->memberFunc->setText(QString::fromUtf8(txtbuffer));
        DbgGetCommentAt(start, txtbuffer);
        ui->comment->setText(QString::fromUtf8(txtbuffer));
    }
    else
    {
        ui->memberFunc->clear();
        ui->comment->clear();
    }
    if(currentClass)
    {
        int j = -1;
        for(int i = 0; i < instr.argcount; i++)
        {
            if(instr.arg[i].type == arg_memory)
            {
                for(j = 0; j < currentClass->membervariable.size(); j++)
                {
                    if(currentClass->membervariable.at(j).first == instr.arg[i].constant)
                    {
                        auto list = ui->memberVarList->findItems(QString::number(instr.arg[i].constant, 16), Qt::MatchExactly, 0);
                        if(list.size() > 0)
                        {
                            ui->memberVarList->setCurrentItem(list.first());
                            if(ui->instance->text().size() == 0)
                                list.first()->setText(3, ToPtrString(instr.arg[i].value));
                            return;
                        }
                    }
                }
            }
        }
    }
}
