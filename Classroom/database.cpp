#include "pluginmain.h"
#include "Classroom.h"
#include "MyClass.h"
#ifdef _WIN64
#else //x86
#endif //_WIN64

void cbLoadDb(CBTYPE cbType, PLUG_CB_LOADSAVEDB* arg)
{
    if(!(arg->loadSaveType == PLUG_DB_LOADSAVE_DATA || arg->loadSaveType == PLUG_DB_LOADSAVE_ALL)) //NONE
        return;
    json_t* classroom;
    classroom = json_object_get(arg->root, "classroom");
    if(!json_is_array(classroom))
        return;
    json_t* classJSON;
    size_t i;
    json_array_foreach(classroom, i, classJSON)
    {
        if(json_is_object(classJSON))
        {
            json_t* value;
            MyClass* currentClass;
            currentClass = new MyClass();
            value = json_object_get(classJSON, "label");
            if(json_is_string(value))
                currentClass->name = QString::fromUtf8(json_string_value(value));
            value = json_object_get(classJSON, "size");
            if(json_is_integer(value))
                currentClass->size = json_integer_value(value);
            value = json_object_get(classJSON, "comment");
            if(json_is_string(value))
                currentClass->comment = QString::fromUtf8(json_string_value(value));
            value = json_object_get(classJSON, "module");
            if(json_is_string(value))
                currentClass->module = QString::fromUtf8(json_string_value(value));
            value = json_object_get(classJSON, "membervariable");
            if(json_is_array(value))
            {
                json_t* varJSON;
                size_t j;
                json_array_foreach(value, j, varJSON)
                {
                    MyClassMemberVariable varobject;
                    json_t* varvalue;
                    int offset = -1;
                    varvalue = json_object_get(varJSON, "offset");
                    if(json_is_integer(varvalue))
                        offset = json_integer_value(varvalue);
                    if(offset >= 0)
                    {
                        varvalue = json_object_get(varJSON, "label");
                        if(json_is_string(varvalue))
                            varobject.label = QString::fromUtf8(json_string_value(varvalue));
                        varvalue = json_object_get(varJSON, "type");
                        if(json_is_string(varvalue))
                            varobject.vartype = QString::fromUtf8(json_string_value(varvalue));
                        varvalue = json_object_get(varJSON, "comment");
                        if(json_is_string(varvalue))
                            varobject.comment = QString::fromUtf8(json_string_value(varvalue));
                        currentClass->membervariable.push_back(std::make_pair(offset, varobject));
                    }
                }
            }
            Plugin::classroom.push_back(currentClass);
        }
    }
    GuiExecuteOnGuiThread(QtPlugin::RefreshClasses);
}

void cbSaveDb(CBTYPE cbType, PLUG_CB_LOADSAVEDB* arg)
{
    json_t* classroom;
    if(Plugin::classroom.size() > 0)
    {
        classroom = json_array();
        for(auto & currentClass : Plugin::classroom)
        {
            json_t* classJSON;
            json_t* membervariable;
            classJSON = json_object();
            json_object_set_new(classJSON, "label", json_string(currentClass->name.toUtf8().constData()));
            json_object_set_new(classJSON, "size", json_integer(currentClass->size));
            json_object_set_new(classJSON, "comment", json_string(currentClass->comment.toUtf8().constData()));
            json_object_set_new(classJSON, "module", json_string(currentClass->module.toUtf8().constData()));
            //TODO: member functions need to save with module name! Using "mod.hash(addr)" expression function.
            membervariable = json_array();
            for(auto & i : currentClass->membervariable)
            {
                json_t* variableObject;
                variableObject = json_object();
                json_object_set_new(variableObject, "offset", json_integer(i.first));
                json_object_set_new(variableObject, "label", json_string(i.second.label.toUtf8().constData()));
                json_object_set_new(variableObject, "type", json_string(i.second.vartype.toUtf8().constData()));
                json_object_set_new(variableObject, "comment", json_string(i.second.comment.toUtf8().constData()));
                json_array_append_new(membervariable, variableObject);
            }
            json_object_set_new(classJSON, "membervariable", membervariable);
            json_array_append_new(classroom, classJSON);
        }
        json_object_set_new(arg->root, "classroom", classroom);
    }
}
