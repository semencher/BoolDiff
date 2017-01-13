#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "global.h"

void readSourceSystem(SourceSystem * sourceSystem, QString blif);

void showSourceSystem(SourceSystem * sourceSystem);

DNF * ProductAndDNF(Product * product, DNF * dnf);

DNF * DNF_OR_DNF(DNF * dnf1, DNF * dnf2);

bool CompareProducts(Product * product1, Product * product2);

DNF * DNF_AND_DNF(DNF * dnf1, DNF * dnf2);

void removeProduct(Product * product);

void removeDNF(DNF * dnf);

void showDNF(DNF * dnf);

void reduceDNF(DNF * dnf);

DNF * inversionProduct(Product * product);

DNF * inversionDNF(DNF * dnf);

void absorbDNF(DNF * dnf);

bool absorbProduct(Product * product1, Product * product2);

DNF * getDNF(Element * element);

QString correctDNF(DNF * dnf, SourceSystem * sourceSystem);

DNF * substitute(DNF * dnf, QString sym, DNF * substitution);

QVector <DNF *> algorithm(SourceSystem * sourceSystem);

#endif // FUNCTIONS_H
