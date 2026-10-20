#ifndef ARCHIVEMANAGER_H
#define ARCHIVEMANAGER_H

#include <QTextStream>
#include <QString>
#include "archive.h"

class ArchiveManager {
private:
    Archive* currentArchive;
    QTextStream in;
    QTextStream out;

    void selectAlgorithm();
    void compressFile();
    void decompressFile();

    int getWindowSizeRecommendation(const QString &filePath) const;
    quint32 calculateChecksum(const QByteArray &data) const;

public:
    ArchiveManager();
    ~ArchiveManager();

    void runMenu();
};

#endif // ARCHIVEMANAGER_H