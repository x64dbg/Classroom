#include "LineEditDialog.h"
#include "ui_LineEditDialog.h"

LineEditDialog::LineEditDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LineEditDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setModal(true); //modal window
    ui->checkBox->hide();
    bChecked = false;
    this->fixed_size = 0;
    ui->label->setVisible(false);
}

LineEditDialog::~LineEditDialog()
{
    delete ui;
}

void LineEditDialog::selectAllText()
{
    ui->textEdit->selectAll();
}

void LineEditDialog::ForceSize(unsigned int size)
{
    this->fixed_size = size;
    if(this->fixed_size)
        ui->label->setVisible(true);
}

void LineEditDialog::setText(const QString & text)
{
    ui->textEdit->setText(text);
    ui->textEdit->selectAll();
}

void LineEditDialog::setPlaceholderText(const QString & text)
{
    ui->textEdit->setPlaceholderText(text);
}

void LineEditDialog::enableCheckBox(bool bEnable)
{
    if(bEnable)
        ui->checkBox->show();
    else
        ui->checkBox->hide();
}

void LineEditDialog::setCheckBox(bool bSet)
{
    ui->checkBox->setChecked(bSet);
    bChecked = bSet;
}

void LineEditDialog::setCheckBoxText(const QString & text)
{
    ui->checkBox->setText(text);
}

void LineEditDialog::on_textEdit_textEdited(const QString & arg1)
{
    this->editText = arg1;
}

void LineEditDialog::on_checkBox_toggled(bool checked)
{
    bChecked = checked;
}

void LineEditDialog::on_buttonOk_clicked()
{
    accept();
}

void LineEditDialog::on_buttonCancel_clicked()
{
    ui->textEdit->setText("");
    close();
}
