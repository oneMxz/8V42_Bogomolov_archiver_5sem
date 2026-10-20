#ifndef DEFLATEALGORITHM_H
#define DEFLATEALGORITHM_H

#include "compressionalgorithm.h"

class Lz77Algorithm;
class HuffmanAlgorithm;

class DeflateAlgorithm : public CompressionAlgorithm {
public:
    DeflateAlgorithm();
    ~DeflateAlgorithm() override;

    QByteArray compress(const QByteArray &data) override;
    QByteArray decompress(const QByteArray &data, quint32 originalSize) override;
    QString getName() const override;

private:
    Lz77Algorithm* m_lz77;
    HuffmanAlgorithm* m_huffman;
};

#endif // DEFLATEALGORITHM_H