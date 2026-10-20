#ifndef COMPRESSIONALGORITHM_H
#define COMPRESSIONALGORITHM_H
#include <QByteArray>
#include <QString>

class CompressionAlgorithm {
public:
    virtual ~CompressionAlgorithm() = default;
    virtual QByteArray compress(const QByteArray &data) = 0;
    virtual QByteArray decompress(const QByteArray &data, quint32 originalSize) = 0;
    virtual QString getName() const = 0;
};

#endif // COMPRESSIONALGORITHM_H
