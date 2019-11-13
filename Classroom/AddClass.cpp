#include "ui_AddClass.h"
#include "AddClass.h"
#include "pluginmain.h"
#include "MyClass.h"
#include "Classroom.h"

AddClass::AddClass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->btnOk,SIGNAL(clicked()),this,SLOT(ok()));
}

AddClass::~AddClass()
{
    delete ui;
}

void AddClass::setClassName(const QString & name)
{
    MyClass* myclass;
    ui->name->setText(name);
    myclass = Plugin::getClassByName(name);
    if(myclass != nullptr)
    {
        ui->description->setText(myclass->comment);
        setWindowTitle(tr("Edit class %1").arg(name));
        ui->editSize->setText(myclass->size >= 0 ? QString::number(myclass->size) : "-");
    }
}

void AddClass::ok()
{
    MyClass* myclass;
    int size;
    bool ok;
    bool isNewClass;
    QString name;
    name = ui->name->text();
    if(name.size() <= 0)
    {
        SimpleErrorBox(this, tr("Class name is empty."), tr("Please enter the class name."));
        ui->name->setFocus();
        return;
    }
    myclass = Plugin::getClassByName(name);
    if(myclass == nullptr)
    {
        myclass = new MyClass();
        isNewClass = true;
    }
    else
        isNewClass = false;
    myclass->name = name;
    size = ui->editSize->text().toInt(&ok);
    if(ok)
        myclass->size = size >= 0 ? size : -1;
    else
        myclass->size = -1; //-1 is Unknown size
    myclass->comment=ui->description->toPlainText();
    if(isNewClass)
        Plugin::classroom.push_back(myclass);
    accept();
    //Try to add the member function selected by the user, if applicable
    SELECTIONDATA dissel;
    if(GuiSelectionGet(GUI_DISASSEMBLY, &dissel))
        QtPlugin::cbSelChanged((void*)dissel.start);
    QtPlugin::RefreshClasses();
}
