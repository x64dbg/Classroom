#ifndef ADDCLASS_H
#define ADDCLASS_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class AddClass : public QDialog
{
    Q_OBJECT

public:
    explicit AddClass(QWidget *parent = nullptr);
    void setClassName(const QString & name);
    ~AddClass();

public slots:
    void ok();

private:
    Ui::Dialog *ui;
};

#endif // ADDCLASS_H
