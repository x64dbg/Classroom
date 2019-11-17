#include "PluginMainWindow.h"
#include "ui_PluginMainWindow.h"
#include "pluginmain.h"
#include "Classroom.h"
#include "MyClass.h"
#include <QTextBrowser>
#include <QMessageBox>

PluginMainWindow::PluginMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::PluginMainWindow)
{
    ui->setupUi(this);
    connect(ui->textBrowser,SIGNAL(anchorClicked(QUrl)),this,SLOT(anchorClicked(QUrl)));
    connect(ui->showDialog,SIGNAL(clicked()),this,SLOT(showDialogClicked()));
    connect(ui->classes,SIGNAL(itemSelectionChanged()),this,SLOT(refresh()));
    refresh();
}

PluginMainWindow::~PluginMainWindow()
{
    delete ui;
}

void PluginMainWindow::refresh()
{
    if(!GuiIsUpdateDisabled())
        ui->textBrowser->setHtml(currentHtml());
}

void PluginMainWindow::refreshClasses()
{
    QString currentSelected;
    if(ui->classes->currentRow()>=0)
        currentSelected=ui->classes->currentItem()->text();
    ui->classes->clear();
    if(Plugin::classroom.size() > 0)
    {
        QStringList classes;
        for(auto & i : Plugin::classroom){
            classes.append(i->name);
        }
        classes.sort();
        ui->classes->addItems(classes);
        if(currentSelected.size()>0)
        {
            for(int i=0;i<classes.size();i++){
                if(classes[i]==currentSelected)
                {
                    ui->classes->setCurrentRow(i);
                    break;
                }
            }
        }
    }
    refresh();
}

void PluginMainWindow::showDialogClicked()
{
    QtPlugin::Show();
}

void PluginMainWindow::anchorClicked(const QUrl& link)
{
    char textbuffer[MAX_COMMENT_SIZE];
    if(link.scheme()=="x64dbg")
    {
        if(link.path()=="/command")
        {
            DbgCmdExec(link.fragment().toUtf8().constData());
        }
        else if(link.path() == "/address")
        {
            if(DbgIsDebugging())
            {
                bool ok = false;
                auto address = duint(link.fragment(QUrl::DecodeReserved).toULongLong(&ok, 16));
                if(ok && DbgMemIsValidReadPtr(address))
                {
                    if(DbgFunctions()->MemIsCodePage(address, true))
                        DbgCmdExec(QString("disasm %1").arg(link.fragment()).toUtf8().constData());
                    else
                    {
                        DbgCmdExec(QString("dump %1").arg(link.fragment()).toUtf8().constData());
                    }
                }
                else
                    SimpleErrorBox(this, tr("Invalid address!"), tr("The address %1 is not a valid memory location...").arg(ToPtrString(address)));
            }
        }
        else if(link.path() == "/labelat")
        {
            QString label;
            bool ok = false;
            auto address = duint(link.fragment(QUrl::DecodeReserved).toULongLong(&ok, 16));
            if(ok)
            {
                if(!DbgGetLabelAt(address, SEG_DEFAULT, textbuffer))
                    textbuffer[0] = 0;
                label = QString::fromUtf8(textbuffer);
                if(SimpleInputBox(this, tr("Set Label at %1").arg(link.fragment()), label, label, ""))
                {
                    DbgSetLabelAt(address, label.toUtf8().constData());
                }
            }
            refresh();
        }
        else if(link.path() == "/commentat")
        {
            QString comment;
            bool ok = false;
            auto address = duint(link.fragment(QUrl::DecodeReserved).toULongLong(&ok, 16));
            if(ok)
            {
                if(!DbgGetCommentAt(address, textbuffer))
                    textbuffer[0] = 0;
                comment = QString::fromUtf8(textbuffer);
                if(SimpleInputBox(this, tr("Set Comment at %1").arg(link.fragment()), comment, comment, ""))
                {
                    DbgSetCommentAt(address, comment.toUtf8().constData());
                }
            }
            refresh();
        }
        else if(link.path() == "/thisdocument")
            ui->textBrowser->scrollToAnchor(link.fragment());
        else
            SimpleErrorBox(this, tr("Url is not valid!"), tr("The Url %1 is not supported").arg(link.toString()));
    }
    else
        SimpleErrorBox(this, tr("Url is not valid!"), tr("The Url %1 is not supported").arg(link.toString()));
}

//HTML generation
QString PluginMainWindow::currentHtml()
{
    QString myhtml;
    myhtml=classHtml();
    return "<html><head><title>Classroom</title></head><body>"+myhtml+"</body></html>";
}

QString PluginMainWindow::placeholderHtml()
{
    return  tr("<h2>There is no known classes yet!</h2><p><a href=\"x64dbg://localhost/command#classroom\"><img src=\"qrc://icons/images/addclass.png\"/>Create a class!</a></p>");
}

QString PluginMainWindow::aboutHtml()
{
    return tr("<h2>Classroom</h2><p>Classroom is a plugin that helps you analyze an object-oriented application.</p>");
}

QString PluginMainWindow::classHtml()
{
    MyClass* currentClass;
    QStringList html;
    currentClass = nullptr;
    if(!DbgIsDebugging())
        return aboutHtml();
    if(ui->classes->currentItem() == nullptr)
        return (Plugin::classroom.size()>0) ? aboutHtml() : placeholderHtml();
    html.append(ui->classes->currentItem()->text());
    currentClass=Plugin::getClassByName(html[0]);
    if(currentClass == nullptr)
        return placeholderHtml();
    //Header
    html[0]="<h2>"+html[0].toHtmlEscaped()+tr("</h2><p>Module: ");
    if(currentClass->module.isEmpty())
        html.append(tr("<i>Module not defined (Please add a member function)</i>"));
    else
        html.append(currentClass->module.toHtmlEscaped());
    html.append(tr(" Size:"));
    html.append(currentClass->size == -1 ? tr("<i>Unknown size</i>") : QString::number(currentClass->size));
    html.append(tr(" <a href=\"x64dbg://localhost/command#classroom %1\">Edit</a> <a href=\"x64dbg://localhost/command#delclass %1\">Delete</a>").arg(currentClass->name.toHtmlEscaped()));
    html.append(tr("</p><h3>Descriptions:</h3><p>")+currentClass->comment.toHtmlEscaped().replace("\n","<br/>")+"</p>");
    //Member functions
    html.append(functionHtml(currentClass));
    //Member variables
    html.append(memberHtml(currentClass));
    //Footer
    html.append(tr("<p><a href=\"x64dbg://localhost/command#classroom\"><img src=\"qrc://icons/images/addclass.png\"/>Create a class</a></p>"));
    return html.join("");
}

QString PluginMainWindow::functionHtml(MyClass* currentClass)
{
    QStringList html;
    using FuncPair = std::pair<QString, duint>;
    QList<FuncPair> funcs;
    QSet<duint> stalefunction;
    QByteArray currentClassPrefix;
    if(currentClass->memberfunction.size() == 0)
    {
        return tr("<h3>Member functions:</h3><p>No member functions known for this class.</p>")
                +tr("<p>To define a function as a member of this class, add a label to the function that starts with \"%1::\". Make sure to also define the function boundary with Add Function or Analyze.</p>").arg(currentClass->name.toHtmlEscaped())
                +tr("<p>If you already defined some member functions but they don't show here, <a href=\"x64dbg://localhost/command#refreshmembervar\">Refresh the list</a>. Note: The module must be loaded before member functions can be displayed.</p>");
    }
    currentClassPrefix = currentClass->name.toUtf8();
    currentClassPrefix.append("::");
    for(auto & i : currentClass->memberfunction)
    {
        char label[MAX_LABEL_SIZE];
        if(DbgGetLabelAt(i, SEG_DEFAULT, label))
        {
            if(memcmp(label, currentClassPrefix, currentClassPrefix.size()) == 0)
                funcs.push_back(std::make_pair(QString::fromUtf8(label + currentClassPrefix.size()).toHtmlEscaped(), i));
            else
                stalefunction.insert(i);
        }
        else
            stalefunction.insert(i);
    }
    currentClass->memberfunction.subtract(stalefunction);
    stalefunction.clear();
    currentClassPrefix.clear();
    std::sort(funcs.begin(), funcs.end(), [](const FuncPair & x, const FuncPair & y){
        return x.first < y.first;
    });
    html.append(tr("<h3>Member functions:</h3><ul>"));
    for(auto & i : funcs)
        html.append(QString("<li><a href=\"x64dbg://localhost/thisdocument#%1\">%1</a> @ <a href=\"x64dbg://localhost/address#%2\">%2</a></li>").arg(i.first.toHtmlEscaped(), ToPtrString(i.second)));
    html.append("</ul><hr/>");
    for(auto & i : funcs)
    {
        QString ptrstr = ToPtrString(i.second);
        char comment[MAX_COMMENT_SIZE];
        html.append(QString("<p><b>%2</b> @ <a href=\"x64dbg://localhost/address#%1\" name=\"%2\">%1</a>").arg(ptrstr, i.first));
        html.append(QString(" <a href=\"x64dbg://localhost/labelat#") + ptrstr + tr("\">Edit</a></p>"));
        html.append("<p>");
        if(DbgGetCommentAt(i.second, comment))
            html.append(QString::fromUtf8(comment).toHtmlEscaped());
        else
            html.append(tr("<i>No comment for this function</i>"));
        html.append(QString("&nbsp;&nbsp;<a href=\"x64dbg://localhost/commentat#") + ptrstr + tr("\">Edit</a></p>"));
        html.append("<hr/>");
    }
    html.append("<p><a href=\"x64dbg://localhost/command#refreshmembervar\">Refresh the list.</a></p>");
    html.append(tr("<p>To define a function as a member of this class, add a label to the function that starts with \"%1::\". Make sure to also define the function boundary with Add Function or Analyze.</p>").arg(currentClass->name.toHtmlEscaped()));
    return html.join("");
}

QString PluginMainWindow::memberHtml(MyClass* currentClass)
{
    QStringList html;
    if(currentClass->membervariable.size() == 0)
    {
        return tr("<h3>Member variables:</h3><p>No member variable known for this class. <a href=\"x64dbg://localhost/command#classmembervar %1\">Add</a></p>").arg(currentClass->name.toHtmlEscaped());
    }
    html.append(tr("<h3>Member variables:</h3>"));
    for(auto & i : currentClass->membervariable)
    {
        QString ptrstr = QString::number(i.first, 16);
        html.append("<p>");
        html.append(ptrstr);
        html.append("&nbsp;&nbsp;");
        html.append(i.second.vartype.toHtmlEscaped());
        html.append("&nbsp;<b>");
        html.append(i.second.label.toHtmlEscaped());
        html.append("</b>;&nbsp;");
        html.append(i.second.comment.toHtmlEscaped());
        html.append(QString("&nbsp;<a href=\"x64dbg://localhost/command#classmembervar %1,%2\">Edit</a>&nbsp;<a href=\"x64dbg://localhost/command#delclassmembervar %1,%2\">Delete</a></p>").arg(currentClass->name.toHtmlEscaped(), ptrstr));
    }
    html.append(tr("<p><a href=\"x64dbg://localhost/command#classmembervar %1\">Add</a></p><hr/>").arg(currentClass->name.toHtmlEscaped()));
    return html.join("");
}
