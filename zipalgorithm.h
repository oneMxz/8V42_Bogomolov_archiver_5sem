#ifndef ZIPALGORITHM_H
#define ZIPALGORITHM_H

#include "compressionalgorithm.h"

class DeflateAlgorithm;

class ZipAlgorithm : public CompressionAlgorithm {
public:
    ZipAlgorithm();
    ~ZipAlgorithm() override;

    QByteArray compress(const QByteArray &data) override;
    QByteArray decompress(const QByteArray &data, quint32 originalSize) override;
    QString getName() const override;

private:
    DeflateAlgorithm* m_deflate;
};

#endif // ZIPALGORITHM_H