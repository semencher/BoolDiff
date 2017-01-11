#include "functions.h"

using std::cout;

void readSourceSystem(SourceSystem * sourceSystem, QString blif)
{
    std::ifstream file;
    std::string stdBlif = blif.toStdString();
    file.open(stdBlif, std::ios::in);

    if (!file)
    {
        Exception e;
        e.setTypeError("Unable to open file!");
        throw e;
    }

    Exception e;
    e.setTypeError("Error of file!");

    std::string word;
    if (file.eof())
    {
        throw e;
    }

    file >> word;
    while (word[0] == '.')
    {
        if (word == ".model")
        {
            file >> word;
            sourceSystem->modal = QString::fromStdString(word);
            file >> word;
        }
        if (word == ".inputs")
        {
            file >> word;
            while (word[0] != '.')
            {
                sourceSystem->inputs.push_back(
                            new QString(QString::fromStdString(word)));
                file >> word;
            }
        }
        if (word == ".outputs")
        {
            file >> word;
            while (word[0] != '.')
            {
                sourceSystem->outputs.push_back(
                            new QString(QString::fromStdString(word)));
                file >> word;
            }
        }
        if (word == ".names")
        {
            Element * element = new Element;
            std::string input;
            file >> input;
            file >> word;
            while(word[0] != '0' && word[0] != '1' && word[0] != '-')
            {
                element->namesInputsElement.push_back(
                            new QString(QString::fromStdString(input)));
                input = word;
                file >> word;
            }
            sourceSystem->blocks[QString::fromStdString(input)] = element;
            while(word[0] != '.')
            {
                element->inputsOnElement.push_back(
                            new QString(QString::fromStdString(word)));
                int value;
                file >> value;
                element->value.push_back(value);
                file >> word;
            }
        }
        if (word == ".end")
        {
            break;
        }
    }
}

void showSourceSystem(SourceSystem * sourceSystem)
{
    std::cout << sourceSystem->modal.toStdString() << "\n";
    int sizeInputs = sourceSystem->inputs.size();
    int sizeOutputs = sourceSystem->outputs.size();
    std::cout << "inputs:   ";
    for (int i = 0; i < sizeInputs; ++i)
    {
        std::cout << sourceSystem->inputs[i]->toStdString() << " ";
    }
    std::cout << "\n";
    std::cout << "outputs:  ";
    for (int j = 0; j < sizeOutputs; ++j)
    {
        std::cout << sourceSystem->outputs[j]->toStdString() << " ";
    }
    std::cout << "\n";

    QMap <QString, Element *>::iterator iter = sourceSystem->blocks.begin();
    QMap <QString, Element *>::iterator end = sourceSystem->blocks.end();
    for (; iter != end; ++iter)
    {
        std::cout << iter.key().toStdString() << "\n";
        Element * element = iter.value();
        int sizeNamesInputsElement = element->namesInputsElement.size();
        int sizeInputsElements = element->inputsOnElement.size();
        for (int i = 0; i < sizeNamesInputsElement; ++i)
        {
            std::cout << element->namesInputsElement[i]->toStdString() << " ";
        }
        std::cout << "\n";
        for (int j = 0; j < sizeInputsElements; ++j)
        {
            std::cout << element->inputsOnElement[j]->toStdString() <<
                         " " << element->value[j] << "\n";
        }
    }
}
