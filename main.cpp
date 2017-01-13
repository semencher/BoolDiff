#include <QCoreApplication>

#include "functions.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    try
    {
        SourceSystem * sourceSystem = new SourceSystem;

        // В дальнейшем это будет argv[1].
        QString blif = "cm42a.blif";

        readSourceSystem(sourceSystem, blif);

        // В дальнейшем уберем.
//        showSourceSystem(sourceSystem);

        // Массив ДНФ соответствует порядку входных переменных сперва 0 потом 1 и так для
        // каждой.
        QVector <DNF *> arrayDNF = algorithm(sourceSystem);
    }
    catch(Exception e)
    {
        std::cout << "\n" << e.message() << "\n";
    }


    return 0;
}
