#ifndef ADDMEMBERVARIABLE_H
#define ADDMEMBERVARIABLE_H

#include <QDialog>
class MyClass;

namespace Ui {
class AddMemberVariable;
}

class AddMemberVariable : public QDialog
{
    Q_OBJECT

public:
    explicit AddMemberVariable(QWidget *parent = nullptr);
    ~AddMemberVariable();

    void Init(MyClass* currentClass, int offset);

public slots:
    void ok();

private:
    Ui::AddMemberVariable *ui;
    QString className;
};

#endif // ADDMEMBERVARIABLE_H
