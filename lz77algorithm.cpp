#include "lz77algorithm.h"
#include <QDataStream>
#include <QBuffer>

Lz77Algorithm::Lz77Algorithm(int windowSize) : m_windowSize(windowSize) {}

QString Lz77Algorithm::getName() const {
    return "LZ77";
}

QList<LZ77Node> Lz77Algorithm::compressData(const QByteArray &data) {
    QList<LZ77Node> result;
    int pos = 0, dataLen = data.size();

    while (pos < dataLen) {
        int bestOffset = 0, bestLength = 0;
        int searchStart = qMax(0, pos - m_windowSize);

        for (int i = pos - 1; i >= searchStart; --i) {
            int length = 0;
            while (length < 255 && i + length < pos && pos + length < dataLen &&
                   data[i + length] == data[pos + length]) {
                length++;
            }
            if (length > bestLength) {
                bestLength = length;
                bestOffset = pos - i;
            }
        }

        LZ77Node node;
        if (bestLength >= 3) {
            node.offset = bestOffset;
            node.length = bestLength;
            node.nextByte = (pos + bestLength < dataLen) ? data[pos + bestLength] : 0;
            pos += bestLength + 1;
        } else {
            node.offset = 0;
            node.length = 0;
            node.nextByte = data[pos];
            pos++;
        }
        result.append(node);
    }
    return result;
}

QByteArray Lz77Algorithm::compress(const QByteArray &data) {
    QList<LZ77Node> nodes = compressData(data);
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);

    stream << (quint32)data.size() << (quint32)nodes.size();
    for (const LZ77Node &n : nodes) {
        stream << n.offset << n.length << n.nextByte;
    }
    return result;
}

QByteArray Lz77Algorithm::decompress(const QByteArray &data, quint32 originalSize) {
    QByteArray result;
    result.reserve(originalSize);

    QBuffer buffer;
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream.setVersion(QDataStream::Qt_5_15);

    quint32 origSize, nodesCount;
    stream >> origSize >> nodesCount;

    for (quint32 i = 0; i < nodesCount; ++i) {
        LZ77Node n;
        stream >> n.offset >> n.length >> n.nextByte;
        if (result.size() >= (int)originalSize) break;

        if (n.length == 0) {
            result.append(static_cast<char>(n.nextByte));
        } else {
            int startPos = result.size() - n.offset;
            for (int j = 0; j < n.length; ++j) {
                if (startPos + j >= 0 && startPos + j < result.size()) {
                    result.append(result[startPos + j]);
                }
            }
            if (result.size() < (int)originalSize) {
                result.append(static_cast<char>(n.nextByte));
            }
        }
    }
    return result.left(originalSize);
}