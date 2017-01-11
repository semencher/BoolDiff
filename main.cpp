#include <QCoreApplication>

#include "functions.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    try
    {
        SourceSystem * sourceSystem = new SourceSystem;

        // В дальнейшем это будет argv[1].
        QString blif = "b1.blif";

        readSourceSystem(sourceSystem, blif);

        showSourceSystem(sourceSystem);
    }
    catch(Exception e)
    {
        std::cout << "\n" << e.message() << "\n";
    }


    return 0;
}
