#include "functions.h"

using std::cout;

bool validWord(std::string word)
{
    int size = word.size();
    for (int i = 0; i < size; ++i)
    {
        if (word[i] != '0' && word[i] != '1' && word[i] != '-')
            return false;
    }
    return true;
}

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
            while(!validWord(word))
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

Product * equalProduct(Product * product)
{
    if (product == NULL)
    {
        return NULL;
    }
    Product * result = new Product;
    int size = product->namesVariables.size();
    for (int i = 0; i < size; ++i)
    {
        result->namesVariables.push_back(new QString(*(product->namesVariables[i])));
        result->zeroOrOne = result->zeroOrOne + product->zeroOrOne[i];
    }
    return result;
}

DNF * equalDNF(DNF * dnf)
{
    if (dnf == NULL)
    {
        return NULL;
    }
    DNF * result = new DNF;
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        if (dnf->products[i] != NULL)
        result->products.push_back(equalProduct(dnf->products[i]));
    }
    return result;
}

Product * ProductAndProduct(Product * product1, Product * product2)
{
    if (product1 == NULL || product2 == NULL)
        return NULL;
    if (product1->namesVariables.size() == 0)
    {
        return equalProduct(product2);
    }
    if (product2->namesVariables.size() == 0)
    {
        return equalProduct(product1);
    }
    int sizeProduct1 = product1->namesVariables.size();
    int sizeProduct2 = product2->namesVariables.size();
    bool inv = false;
    for (int i = 0; i < sizeProduct1; ++i)
    {
        for (int j = 0; j < sizeProduct2; ++j)
        {
            if (*(product1->namesVariables[i]) == *(product2->namesVariables[j]))
            {
                if (product1->zeroOrOne[i] != product2->zeroOrOne[j])
                {
                    inv = true;
                }
            }
        }
    }
    if (inv)
        return NULL;
    Product * result = equalProduct(product1);
    bool cont = false;
    for (int i = 0; i < sizeProduct2; ++i)
    {
        for (int j = 0; j < sizeProduct1; ++j)
        {
            if (*(result->namesVariables[j]) == *(product2->namesVariables[i]))
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
        result->namesVariables.push_back(new QString(*(product2->namesVariables[i])));
        result->zeroOrOne = result->zeroOrOne + product2->zeroOrOne[i];
    }
    return result;
}

DNF * ProductAndDNF(Product * product, DNF * dnf)
{
    if (dnf == NULL || product == NULL)
        return NULL;
    else
    {
        if (product->namesVariables.size() == 0)
        {
            return equalDNF(dnf);
        }
        if (dnf->products.size() == 0)
        {
            Product * result = equalProduct(product);
            DNF * newDNF = new DNF;
            newDNF->products.push_back(result);
            return newDNF;
        }
    }
    DNF * result = NULL;
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        Product * newProduct = ProductAndProduct(product, dnf->products[i]);
        if (newProduct != NULL)
        {
            if (result != NULL)
            {
                result->products.push_back(newProduct);
            }
            else
            {
                result = new DNF;
                result->products.push_back(newProduct);
            }
        }
    }
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

bool CompareProducts(Product * product1, Product * product2)
{
    if (product1 == NULL && product2 == NULL)
        return true;
    else
    {
        if ((product1 == NULL && product2 != NULL) || (product1 != NULL && product2 == NULL))
            return false;
        else
        {
            if (product1->namesVariables.size() == 0 && product2->namesVariables.size() == 0)
                return true;
        }
    }
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
    if (dnf1 == NULL && dnf2 == NULL)
        return NULL;
    if (dnf1 == NULL && dnf2 != NULL)
        return equalDNF(dnf2);
    if (dnf1 != NULL && dnf2 == NULL)
        return equalDNF(dnf1);
    DNF * result = equalDNF(dnf1);
    int size = dnf2->products.size();
    for (int i = 0; i < size; ++i)
    {
        Product * newProduct = equalProduct(dnf2->products[i]);
        result->products.push_back(newProduct);
    }
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

DNF * DNF_AND_DNF(DNF * dnf1, DNF * dnf2)
{
    if (dnf1 == NULL || dnf2 == NULL)
        return NULL;
    if (dnf1->products.size() == 0)
    {
        return equalDNF(dnf2);
    }
    if (dnf2->products.size() == 0)
    {
        return equalDNF(dnf1);
    }
    DNF * result = NULL;
    int sizeDnf2 = dnf2->products.size();
    for (int i = 0; i < sizeDnf2; ++i)
    {
        DNF * partDNF = ProductAndDNF(dnf2->products[i], dnf1);
        DNF * forResult = DNF_OR_DNF(partDNF, result);
        removeDNF(result);
        result = forResult;
    }
    if (result && result->products.size() == 0)
    {
        removeDNF(result);
        result = NULL;
    }
    reduceDNF(result);
    absorbDNF(result);
    return result;
}

void removeProduct(Product * product)
{
    if (product == NULL)
        return;
    int size = product->namesVariables.size();
    for (int i = 0; i < size; ++i)
    {
        delete product->namesVariables[i];
    }
    delete product;
}

void removeDNF(DNF * dnf)
{
    if (dnf == NULL)
        return;
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        removeProduct(dnf->products[i]);
    }
    delete dnf;
}

void showDNF(DNF * dnf)
{
    if (dnf == NULL)
    {
        std::cout << "0";
        return;
    }
    int size = dnf->products.size();
    if (size == 0)
    {
        std::cout << "1";
        return;
    }
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
    if (dnf == NULL)
        return;
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (i != j && CompareProducts(dnf->products[i], dnf->products[j]))
            {
                if (j < i)
                {
                    i--;
                }
                dnf->products.remove(j);
                size--;
            }
        }
    }
}

DNF * inversionProduct(Product * product)
{
    if (product == NULL)
        return new DNF;
    DNF * result = new DNF;
    int size = product->namesVariables.size();
    if (size == 0)
    {
        removeDNF(result);
        return NULL;
    }
    for (int i = 0; i < size; ++i)
    {
        Product * newProduct = new Product;
        newProduct->namesVariables.push_back(new QString(*(product->namesVariables[i])));
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
    if (dnf == NULL)
        return new DNF;
    DNF * result;
    int size = dnf->products.size();
    if (size == 0)
    {
        return NULL;
    }
    result = inversionProduct(dnf->products[0]);
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
    if (dnf == NULL)
        return;
    int size = dnf->products.size();
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (i != j && absorbProduct(dnf->products[i], dnf->products[j]))
            {
                if (j < i)
                {
                    i--;
                }
                dnf->products.remove(j);
                size--;
            }
        }
    }
}

bool absorbProduct(Product * product1, Product * product2)
{
    if (product1 == NULL && product2 == NULL)
        return true;
    if (product1->namesVariables.size() == 0 && product2->namesVariables.size() == 0)
    {
        return true;
    }
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
    if (dnf == NULL)
        return "";
    int size = dnf->products.size();
    if (size == 0)
        return "";
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

DNF * subVal(DNF * dnf, QString sym, char valSym)
{
    if (dnf == NULL)
        return NULL;
    DNF * result = new DNF;
    int size = dnf->products.size();
    if (size == 0)
        return result;
    for (int i = 0; i < size; ++i)
    {
        int sizeProduct = dnf->products[i]->namesVariables.size();
        for (int j = 0; j < sizeProduct; ++j)
        {
            if (*(dnf->products[i]->namesVariables[j]) == sym)
            {
                if (dnf->products[i]->zeroOrOne[j] == valSym)
                {
                    if (sizeProduct == 1)
                    {
                        removeDNF(result);
                        return new DNF;
                    }
                    Product * product = new Product;
                    for (int k = 0; k < sizeProduct; ++k)
                    {
                        if (k != j)
                        {
                            product->namesVariables.push_back(
                                        new QString(*(dnf->products[i]->namesVariables[k])));
                            product->zeroOrOne = product->zeroOrOne +
                                    dnf->products[i]->zeroOrOne[k];
                        }
                    }
                    result->products.push_back(product);
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (j == sizeProduct - 1)
                {
                    Product * product = equalProduct(dnf->products[i]);
                    result->products.push_back(product);
                }
            }
        }
    }
    if (result && result->products.size() == 0)
    {
        return NULL;
    }
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
        removeDNF(dnf);
        dnf = newDNF;
        removeDNF(substitution);
        varNotBegin = correctDNF(dnf, sourceSystem);
    }
    int sizeInputs = sourceSystem->inputs.size();
    Product * invVar = new Product;
    invVar->zeroOrOne = invVar->zeroOrOne + '0';
    invVar->namesVariables.push_back(new QString(""));
    Product * var = new Product;
    var->zeroOrOne = var->zeroOrOne + '1';
    var->namesVariables.push_back(new QString(""));
    for (int i = 0; i < sizeInputs; ++i)
    {
        DNF * x = subVal(dnf, *(sourceSystem->inputs[i]), '0');
        DNF * y = subVal(dnf, *(sourceSystem->inputs[i]), '1');
        DNF * notX = inversionDNF(x);
        DNF * notY = inversionDNF(y);
        DNF * xAndNotY = DNF_AND_DNF(x, notY);
        DNF * notXAndY = DNF_AND_DNF(notX, y);
        DNF * sumModTwo = DNF_OR_DNF(xAndNotY, notXAndY);
        removeDNF(x);
        removeDNF(y);
        removeDNF(notX);
        removeDNF(notY);
        removeDNF(xAndNotY);
        removeDNF(notXAndY);
        reduceDNF(sumModTwo);
        absorbDNF(sumModTwo);
        *(var->namesVariables[0]) = *(sourceSystem->inputs[i]);
        *(invVar->namesVariables[0]) = *(sourceSystem->inputs[i]);
        result.push_back(ProductAndDNF(var, sumModTwo));
        result.push_back(ProductAndDNF(invVar, sumModTwo));
    }
    return result;
}

void showResult(QVector <DNF *> arrayDNF, SourceSystem * sourceSystem)
{
    int size = sourceSystem->inputs.size();
    for (int i = 0; i < size; ++i)
    {
        std::cout << sourceSystem->inputs[i]->toStdString() << "/0 - ";
        DNF * zero = arrayDNF[2*i];
        DNF * one = arrayDNF[2*i + 1];
        int sizeZero = 0;
        if (zero != NULL)
            sizeZero = zero->products.size();
        for (int j = 0; j < sizeZero; ++j)
        {
            std::cout << "(";
            for (int k = 0; k < size; ++k)
            {
                int sizeProduct = zero->products[j]->namesVariables.size();
                for (int t = 0; t < sizeProduct; ++t)
                {
                    if (*(sourceSystem->inputs[k]) == *(zero->products[j]->namesVariables[t]))
                    {
                        std::cout << zero->products[j]->zeroOrOne.toStdString()[t];
                        break;
                    }
                    if (t == sizeProduct - 1)
                    {
                        std::cout << "-";
                    }
                }
            }
            if (j == sizeZero - 1)
            {
                std::cout << ").";
            }
            else
            {
                std::cout << "),";
            }
        }
        std::cout << "\n";
        std::cout << sourceSystem->inputs[i]->toStdString() << "/1 - ";
        int sizeOne = 0;
        if (one != NULL)
            sizeOne = one->products.size();
        for (int j = 0; j < sizeOne; ++j)
        {
            std::cout << "(";
            for (int k = 0; k < size; ++k)
            {
                int sizeProduct = one->products[j]->namesVariables.size();
                for (int t = 0; t < sizeProduct; ++t)
                {
                    if (*(sourceSystem->inputs[k]) == *(one->products[j]->namesVariables[t]))
                    {
                        std::cout << one->products[j]->zeroOrOne.toStdString()[t];
                        break;
                    }
                    if (t == sizeProduct - 1)
                    {
                        std::cout << "-";
                    }
                }
            }
            if (j == sizeOne - 1)
            {
                std::cout << ").";
            }
            else
            {
                std::cout << "),";
            }
        }
        std::cout << "\n";
    }
}
