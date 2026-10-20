#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <QString>
#include <QList>
#include "compressionalgorithm.h"

class ArchiveFile {
public:
    QString name;
    qint64 originalSize;
    qint64 compressedSize;
    ArchiveFile(const QString &n, qint64 oSize, qint64 cSize = 0)
        : name(n), originalSize(oSize), compressedSize(cSize) {}
};

class Archive {
public:
    QString path;
    QList<ArchiveFile*> files;
    CompressionAlgorithm* algorithm;
    Archive() : algorithm(nullptr) {}
    ~Archive();
};



#endif // ARCHIVE_H
