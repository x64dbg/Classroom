#include <QMessageBox>
#include <QIcon>
#include "LineEditDialog.h"
#include "MiscUtil.h"

void SimpleErrorBox(QWidget* parent, const QString & title, const QString & text)
{
    QMessageBox msg(QMessageBox::Critical, title, text, QMessageBox::NoButton, parent);
    msg.setParent(parent, Qt::Dialog);
    msg.setWindowFlags(msg.windowFlags() & (~Qt::WindowContextHelpButtonHint));
    msg.exec();
}

bool SimpleInputBox(QWidget* parent, const QString & title, QString defaultValue, QString & output, const QString & placeholderText, QIcon* icon)
{
    LineEditDialog mEdit(parent);
    mEdit.setWindowIcon(icon ? *icon : parent->windowIcon());
    mEdit.setText(defaultValue);
    mEdit.setPlaceholderText(placeholderText);
    mEdit.setWindowTitle(title);
    mEdit.setCheckBox(false);
    if(mEdit.exec() == QDialog::Accepted)
    {
        output = mEdit.editText;
        return true;
    }
    else
        return false;
}
