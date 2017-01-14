#include <QCoreApplication>

#include "functions.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    try
    {
        SourceSystem * sourceSystem = new SourceSystem;

        // В дальнейшем это будет argv[1].
        QString blif = "c432.blif";

        readSourceSystem(sourceSystem, blif);

        // В дальнейшем уберем.
//        showSourceSystem(sourceSystem);

        // Массив ДНФ соответствует порядку входных переменных сперва 0 потом 1 и так для
        // каждой.
        QVector <DNF *> arrayDNF = algorithm(sourceSystem);

        showResult(arrayDNF, sourceSystem);
        // Короче, необходимо добавить обработку случая когда днф константа и ноль.
    }
    catch(Exception e)
    {
        std::cout << "\n" << e.message() << "\n";
    }


    return 0;
}
