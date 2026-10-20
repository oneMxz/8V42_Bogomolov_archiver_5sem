#include "huffmanalgorithm.h"
#include <QDataStream>
#include <QBuffer>
#include <algorithm>

QString HuffmanAlgorithm::getName() const {
    return "Huffman";
}

HuffmanNode* HuffmanAlgorithm::buildTree(const QByteArray &data) {
    QMap<QChar, int> freqs;
    for (char c : data) freqs[QChar(c)]++;

    QList<HuffmanNode*> nodes;
    for (auto it = freqs.begin(); it != freqs.end(); ++it) {
        nodes.append(new HuffmanNode(it.key(), it.value()));
    }

    while (nodes.size() > 1) {
        std::sort(nodes.begin(), nodes.end(), [](HuffmanNode* a, HuffmanNode* b) {
            return a->probability < b->probability;
        });
        HuffmanNode* left = nodes.takeFirst();
        HuffmanNode* right = nodes.takeFirst();
        HuffmanNode* parent = new HuffmanNode(QChar(0), left->probability + right->probability);
        parent->left = left;
        parent->right = right;
        nodes.append(parent);
    }
    return nodes.isEmpty() ? nullptr : nodes.first();
}

void HuffmanAlgorithm::generateCodes(HuffmanNode* node, QString currentCode, QMap<QChar, QString> &codes) {
    if (!node) return;
    if (!node->left && !node->right) codes[node->charValue] = currentCode;
    generateCodes(node->left, currentCode + "0", codes);
    generateCodes(node->right, currentCode + "1", codes);
}

QByteArray HuffmanAlgorithm::compress(const QByteArray &data) {
    if (data.isEmpty()) return {};

    HuffmanNode* root = buildTree(data);
    QMap<QChar, QString> codes;
    generateCodes(root, "", codes);
    delete root;

    QString bitString;
    for (char c : data) bitString += codes[QChar(c)];

    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);

    stream << (quint32)data.size();
    stream << (quint32)codes.size();
    for (auto it = codes.begin(); it != codes.end(); ++it) {
        stream << it.key() << it.value();
    }

    char currentByte = 0;
    int bitCount = 0;
    for (QChar bit : bitString) {
        currentByte <<= 1;
        if (bit == '1') currentByte |= 1;
        bitCount++;
        if (bitCount == 8) {
            result.append(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }
    if (bitCount > 0) {
        currentByte <<= (8 - bitCount);
        result.append(currentByte);
    }
    return result;
}

QByteArray HuffmanAlgorithm::decompress(const QByteArray &data, quint32) {
    QBuffer buffer;
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream.setVersion(QDataStream::Qt_5_15);

    quint32 origSize, dictSize;
    stream >> origSize >> dictSize;

    QMap<QChar, QString> codes;
    for (quint32 i = 0; i < dictSize; ++i) {
        QChar ch;
        QString code;
        stream >> ch >> code;
        codes[ch] = code;
    }

    QByteArray bitData = buffer.readAll();

    QMap<QString, QChar> reverseCodes;
    for (auto it = codes.begin(); it != codes.end(); ++it) {
        reverseCodes[it.value()] = it.key();
    }

    QByteArray result;
    QString currentCode;

    for (char byte : bitData) {
        for (int i = 7; i >= 0; --i) {
            currentCode += (byte & (1 << i)) ? '1' : '0';
            if (reverseCodes.contains(currentCode)) {
                result.append(reverseCodes[currentCode].toLatin1());
                currentCode.clear();
                if (result.size() >= (int)origSize) return result;
            }
        }
    }
    return result;
}