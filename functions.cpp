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

DNF * ProductAndDNF(Product * product, DNF * dnf)
{
    DNF * result = new DNF;
    int size = dnf->products.size();
    int sizeProduct = product->namesVariables.size();
    bool cont = false;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < sizeProduct; ++j)
        {
            int index = dnf->products[i]->namesVariables.indexOf(
                        product->namesVariables[j]);
            if (index >= 0)
            {
                if (dnf->products[i]->zeroOrOne[index] != product->zeroOrOne[j])
                {
                    cont = true;
                }
            }
        }
        if (cont)
        {
            cont = false;
            continue;
        }
        Product * newProduct = new Product;
        for (int j = 0; j < sizeProduct; ++j)
        {
            newProduct->namesVariables.push_back(product->namesVariables[j]);
            newProduct->zeroOrOne + product->zeroOrOne[j];
        }
        int sizeIProduct = dnf->products[i]->namesVariables.size();
        for (int j = 0; j < sizeIProduct; ++j)
        {
            if (!newProduct->namesVariables.contains(dnf->products[i]->namesVariables[j]))
            {
                newProduct->namesVariables.push_back(dnf->products[i]->namesVariables[j]);
                newProduct->zeroOrOne + dnf->products[i]->zeroOrOne[j];
            }
        }
        result->products.push_back(newProduct);
    }
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

bool CompareProducts(Product * product1, Product * product2)
{
    if (product1->namesVariables.size() != product2->namesVariables.size())
    {
        return false;
    }
    int size = product1->namesVariables.size();
    for (int i = 0; i < size; ++i)
    {
        int index = product2->namesVariables.indexOf(product1->namesVariables[i]);
        if (index >= 0)
        {
            if (product2->zeroOrOne[index] != product1->zeroOrOne[i])
                return false;
            else
                continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

DNF * DNF_OR_DNF(DNF * dnf1, DNF * dnf2)
{
    DNF * result = new DNF;
    int sizeDNF1 = dnf1->products.size();
    for (int i = 0; i < sizeDNF1; ++i)
    {
        Product * newProduct = new Product;
        int sizeProduct = dnf1->products[i]->namesVariables.size();
        for (int j = 0; j < sizeProduct; ++j)
        {
            newProduct->namesVariables.push_back(dnf1->products[i]->namesVariables[j]);
            newProduct->zeroOrOne + dnf1->products[i]->zeroOrOne[j];
        }
        result->products.push_back(newProduct);
    }
    int sizeDNF2 = dnf2->products.size();
    bool cont = false;
    for (int i = 0; i < sizeDNF2; ++i)
    {
        for (int j = 0; j < sizeDNF1; ++j)
        {
            if (CompareProducts(dnf1->products[j], dnf2->products[i]))
            {
                cont = true;
                break;
            }
        }
        if (cont)
        {
            cont = false;
            continue;
        }
        Product * newProduct = new Product;
        int sizeProduct = dnf2->products[i]->namesVariables.size();
        for (int j = 0; j < sizeProduct; ++j)
        {
            newProduct->namesVariables.push_back(dnf2->products[i]->namesVariables[j]);
            newProduct->zeroOrOne + dnf2->products[i]->zeroOrOne[j];
        }
        result->products.push_back(newProduct);
    }
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

DNF * DNF_AND_DNF(DNF * dnf1, DNF * dnf2)
{
    DNF * result = new DNF;
    int sizeDnf2 = dnf2->products.size();
    for (int i = 0; i < sizeDnf2; ++i)
    {
        DNF * partDNF = ProductAndDNF(dnf2->products[i], dnf1);
        DNF * forResult = DNF_OR_DNF(partDNF, result);
        removeDNF(result);
        result = forResult;
    }
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

void removeProduct(Product * product)
{
    int size = product->namesVariables.size();
    for (int i = 0; i < size; ++i)
    {
        delete product->namesVariables[i];
    }
}

void removeDNF(DNF * dnf)
{
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        removeProduct(dnf->products[i]);
    }
    dnf->products.clear();
}

void showDNF(DNF * dnf)
{
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        int sizeProduct = dnf->products[i]->namesVariables.size();
        for (int j = 0; j < sizeProduct; ++j)
        {
            if (dnf->products[i]->zeroOrOne[j] == '0')
            {
                std::cout << "!";
            }
            std::cout << dnf->products[i]->namesVariables[j];
            if (j != sizeProduct - 1)
            {
                std::cout << "*";
            }
        }
        if (i != size - 1)
        {
            std::cout << "+";
        }
    }
}

void reduceDNF(DNF * dnf)
{
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        for (int j = i + 1; j < size; ++j)
        {
            if (CompareProducts(dnf->products[i], dnf->products[j]))
            {
                dnf->products.remove(j);
                size--;
            }
        }
    }
}

DNF * inversionDNF(DNF * dnf)
{
    DNF * result = new DNF;



    return result;
}

void absorbDNF(DNF * dnf)
{

}

bool absorbProduct(Product * product1, Product * product2)
{

}
