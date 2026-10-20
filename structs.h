#ifndef STRUCTS_H
#define STRUCTS_H
#include <QtTypes>
#include <QChar>

// Узел для LZ77
struct LZ77Node {
    quint16 offset;
    quint16 length;
    quint8 nextByte;
};

// Узел для дерева Хаффмана
struct HuffmanNode {
    QChar charValue;
    int probability;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(QChar c, int p) : charValue(c), probability(p), left(nullptr), right(nullptr) {}
    ~HuffmanNode() { delete left; delete right; }
};


#endif // STRUCTS_H
