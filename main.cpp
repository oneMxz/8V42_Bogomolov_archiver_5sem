#include <QCoreApplication>
#include "archivemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ArchiveManager manager;
    manager.runMenu();
    return 0;
}