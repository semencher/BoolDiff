#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

#include <QVector>

#include <QMap>

#include <QDebug>

#include <iostream>

#include <fstream>

#include <string>

class Exception
{
    std::string type_error;

public:
    Exception()
    {
        type_error = "";
    }

    void setTypeError(std::string tmp)
    {
        type_error = tmp;
    }

    std::string message()
    {
        return type_error;
    }
};  // Exception

struct Element
{
    QVector <QString *> namesInputsElement;
    QVector <QString *> inputsOnElement;
    QVector <int> value;
};

struct SourceSystem
{
    QString modal;
    QVector <QString *> inputs;
    QVector <QString *> outputs;
    QMap <QString, Element *> blocks;
};

#endif // GLOBAL_H
