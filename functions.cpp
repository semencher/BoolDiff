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
            int index = -1;
            for (int k = 0; k < size; ++k)
            {
                if (*(dnf->products[i]->namesVariables[k]) == *(product->namesVariables[j]))
                    index = k;
            }
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
            newProduct->namesVariables.push_back(new QString(*(product->namesVariables[j])));
            newProduct->zeroOrOne = newProduct->zeroOrOne + product->zeroOrOne[j];
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
        int index = -1;
        int sizeProduct2 = product2->namesVariables.size();
        for (int j = 0; j < sizeProduct2; ++j)
        {
            if (*(product2->namesVariables[j]) == *(product1->namesVariables[i]))
                index = j;
        }        if (index >= 0)
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
            std::cout << dnf->products[i]->namesVariables[j]->toStdString();
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

DNF * inversionProduct(Product * product)
{
    DNF * result = new DNF;
    int size = product->namesVariables.size();
    for (int i = 0; i < size; ++i)
    {
        Product * newProduct = new Product;
        newProduct->namesVariables.push_back(product->namesVariables[i]);
        if (product->zeroOrOne[i] == '1')
        {
            newProduct->zeroOrOne = newProduct->zeroOrOne + '0';
        }
        else
        {
            newProduct->zeroOrOne = newProduct->zeroOrOne + '1';
        }
        result->products.push_back(newProduct);
    }
    return result;
}

DNF * inversionDNF(DNF * dnf)
{
    DNF * result;
    result = inversionProduct(dnf->products[0]);
    int size = dnf->products.size();
    for (int i = 1; i < size; ++i)
    {
        DNF * multResult = inversionProduct(dnf->products[i]);
        DNF * tmpResult = DNF_AND_DNF(result, multResult);
        removeDNF(result);
        result = tmpResult;
        removeDNF(multResult);
        reduceDNF(result);
        absorbDNF(result);
    }
    return result;
}

void absorbDNF(DNF * dnf)
{
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        for (int j = i + 1; j < size; ++j)
        {
            if (absorbProduct(dnf->products[i], dnf->products[j]))
            {
                dnf->products.remove(j);
                size--;
            }
        }
    }
}

bool absorbProduct(Product * product1, Product * product2)
{
    if (product1->namesVariables.size() > product2->namesVariables.size())
    {
        return false;
    }
    int size = product1->namesVariables.size();
    for (int i = 0; i < size; ++i)
    {
        int index = -1;
        int sizeProduct2 = product2->namesVariables.size();
        for (int j = 0; j < sizeProduct2; ++j)
        {
            if (*(product2->namesVariables[j]) == *(product1->namesVariables[i]))
                index = j;
        }
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

DNF * getDNF(Element * element)
{
    DNF * result = new DNF;
    int size = element->inputsOnElement.size();
    int sizeProduct = element->namesInputsElement.size();
    for (int i = 0; i < size; ++i)
    {
        Product * product = new Product;
        for (int j = 0; j < sizeProduct; ++j)
        {
            if ((*(element->inputsOnElement[i]))[j] == '-')
            {
                continue;
            }
            product->namesVariables.push_back(
                        new QString(*(element->namesInputsElement[j])));
            if ((*(element->inputsOnElement[i]))[j] == '0')
            {
                product->zeroOrOne = product->zeroOrOne + '0';
            }
            else
            {
                product->zeroOrOne = product->zeroOrOne + '1';
            }
        }
        result->products.push_back(product);
    }
    if (element->value[0] == 0)
    {
        DNF * newResult = inversionDNF(result);
        removeDNF(result);
        result = newResult;
    }
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

QString correctDNF(DNF * dnf, SourceSystem * sourceSystem)
{
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        int sizeProduct = dnf->products[i]->namesVariables.size();
        for (int j = 0; j < sizeProduct; ++j)
        {
            int sizeInputs = sourceSystem->inputs.size();
            for (int k = 0; k < sizeInputs; ++k)
            {
                if (*(dnf->products[i]->namesVariables[j]) == *(sourceSystem->inputs[k]))
                {
                    break;
                }
                if (k == sizeInputs - 1)
                {
                    return *(dnf->products[i]->namesVariables[j]);
                }
            }
        }
    }
    return "";
}

DNF * substitute(DNF * dnf, QString sym, DNF * substitution)
{
    DNF * invSubstitution = inversionDNF(substitution);
    showDNF(invSubstitution);
    DNF * result = new DNF;
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        int sizeProduct = dnf->products[i]->namesVariables.size();
        for (int j = 0; j < sizeProduct; ++j)
        {
            if (sym == *(dnf->products[i]->namesVariables[j]))
            {
                Product * product = new Product;
                for (int t = 0; t < sizeProduct; ++t)
                {
                    if (t != j)
                    {
                        product->namesVariables.push_back(
                                    new QString(*(dnf->products[i]->namesVariables[t])));
                        product->zeroOrOne = product->zeroOrOne +
                                dnf->products[i]->zeroOrOne[t];
                    }
                }
                if (dnf->products[i]->zeroOrOne[j] == '0')
                {
                    DNF * partDNF = ProductAndDNF(product, invSubstitution);
                    DNF * newResult = DNF_OR_DNF(partDNF, result);
                    removeDNF(result);
                    result = newResult;
                    removeDNF(partDNF);
                    showDNF(result);
                }
                else
                {
                    DNF * partDNF = ProductAndDNF(product, substitution);
                    DNF * newResult = DNF_OR_DNF(partDNF, result);
                    removeDNF(result);
                    result = newResult;
                    removeDNF(partDNF);
                }
                break;
            }
            if (j == sizeProduct - 1)
            {
                Product * product = new Product;
                for (int t = 0; t < sizeProduct; ++t)
                {
                    product->namesVariables.push_back(
                                new QString(*(dnf->products[i]->namesVariables[t])));
                    product->zeroOrOne = product->zeroOrOne +
                            dnf->products[i]->zeroOrOne[t];
                }
                result->products.push_back(product);
            }
        }
    }
    removeDNF(invSubstitution);
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

QVector <DNF *> algorithm(SourceSystem * sourceSystem)
{
    QVector <DNF *> result;
    DNF * dnf = getDNF(sourceSystem->blocks[*sourceSystem->outputs[0]]);
    QString varNotBegin = correctDNF(dnf, sourceSystem);
    while (varNotBegin != "")
    {
        DNF * substitution = getDNF(sourceSystem->blocks[varNotBegin]);
        DNF * newDNF = substitute(dnf, varNotBegin, substitution);
       // showDNF(newDNF);
        removeDNF(dnf);
        dnf = newDNF;
        removeDNF(substitution);
        varNotBegin = correctDNF(dnf, sourceSystem);
        break;
    }
    //showDNF(dnf);

    return result;
}
