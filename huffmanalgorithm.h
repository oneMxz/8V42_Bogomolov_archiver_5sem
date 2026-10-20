#ifndef HUFFMANALGORITHM_H
#define HUFFMANALGORITHM_H

#include "compressionalgorithm.h"
#include "structs.h"
#include <QMap>

class HuffmanAlgorithm : public CompressionAlgorithm
{
private:
    HuffmanNode* buildTree(const QByteArray &data);
    void generateCodes(HuffmanNode* node, QString currentCode, QMap<QChar, QString> &codes);
public:
    QString getName() const override;
    QByteArray compress(const QByteArray &data) override;
    QByteArray decompress(const QByteArray &data, quint32 originalSize) override;
};

#endif // HUFFMANALGORITHM_H
