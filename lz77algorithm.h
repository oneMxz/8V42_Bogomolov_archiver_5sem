#ifndef LZ77ALGORITHM_H
#define LZ77ALGORITHM_H

#include "compressionalgorithm.h"
#include "structs.h"
#include <QList>

class Lz77Algorithm : public CompressionAlgorithm
{
private:
    int m_windowSize;
    QList<LZ77Node> compressData(const QByteArray &data);

public:
    QString getName() const override;
    explicit Lz77Algorithm(int windowSize = 4096);
    QByteArray compress(const QByteArray &data) override;
    QByteArray decompress(const QByteArray &data, quint32 originalSize) override;
};

#endif // LZ77ALGORITHM_H
