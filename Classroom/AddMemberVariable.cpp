#include "AddMemberVariable.h"
#include "ui_AddMemberVariable.h"
#include "pluginmain.h"
#include "MyClass.h"
#include "Classroom.h"

AddMemberVariable::AddMemberVariable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMemberVariable)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->ok, SIGNAL(clicked()), this, SLOT(ok()));

    ui->vartype->addItem("BYTE");
    ui->vartype->addItem("WORD");
    ui->vartype->addItem("DWORD");
    ui->vartype->addItem("QWORD");
#ifdef _WIN64
    ui->vartype->setEditText("QWORD");
#else //x86
    ui->vartype->setEditText("DWORD");
#endif //_WIN64
}

AddMemberVariable::~AddMemberVariable()
{
    delete ui;
}

void AddMemberVariable::Init(MyClass* currentClass, int offset)
{
    className = currentClass->name;
    setWindowTitle(tr("Edit member variable of class %1").arg(className));
    if(offset >= 0)
    {
        //TODO: this list is always sorted so we can use binary search
        for(int i = 0; i < currentClass->membervariable.size(); i++)
        {
            if(currentClass->membervariable.at(i).first == offset)
            {
                const MyClassMemberVariable& variable = currentClass->membervariable.at(i).second;
                ui->label->setText(variable.label);
                ui->descriptions->setText(variable.comment);
                ui->vartype->setCurrentText(variable.vartype);
                break;
            }
        }
        ui->offset->setText(QString::number(offset, 16));
        ui->offset->setEnabled(false);//The offset is the unique identifier of the member variable so it cannot be modified.
    }
    else
        ui->offset->setText(QString());

}

void AddMemberVariable::ok()
{
    MyClass* currentClass;
    int offset;
    bool ok;
    currentClass = Plugin::getClassByName(className);
    if(currentClass == nullptr)
    {
        close();
        return;
    }
    offset = ui->offset->text().toInt(&ok, 16);
    if(!ok || offset < 0)
    {
        SimpleErrorBox(this, tr("Error"), tr("The offset must be set and must be positive."));
        ui->offset->setFocus();
        return;
    }
    //TODO: this list is always sorted so we can use binary search
    int before = 0;
    for(int i = 0; i < currentClass->membervariable.size(); i++)
    {
        if(currentClass->membervariable.at(i).first == offset)
        {
            MyClassMemberVariable& variable = currentClass->membervariable[i].second;
            variable.label = ui->label->text();
            variable.comment = ui->descriptions->text();
            variable.vartype = ui->vartype->currentText();
            accept();
            QtPlugin::Refresh();
            return;
        }
        else if(currentClass->membervariable.at(i).first < offset)
            before = i;
    }
    MyClassMemberVariable newvariable;
    newvariable.label = ui->label->text();
    newvariable.comment = ui->descriptions->text();
    newvariable.vartype = ui->vartype->currentText();
    if(currentClass->membervariable.size() > 0 && offset > currentClass->membervariable.last().first)
        currentClass->membervariable.append(std::make_pair(offset, newvariable));
    else
        currentClass->membervariable.insert(before, std::make_pair(offset, newvariable));
    accept();
    QtPlugin::Refresh();
}
