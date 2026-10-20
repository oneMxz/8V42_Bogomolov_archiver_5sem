#include "deflatealgorithm.h"
#include "lz77algorithm.h"
#include "huffmanalgorithm.h"

DeflateAlgorithm::DeflateAlgorithm() {
    m_lz77 = new Lz77Algorithm();
    m_huffman = new HuffmanAlgorithm();
}

DeflateAlgorithm::~DeflateAlgorithm() {
    delete m_lz77;
    delete m_huffman;
}

QString DeflateAlgorithm::getName() const {
    return "Deflate (LZ77 + Huffman)";
}

QByteArray DeflateAlgorithm::compress(const QByteArray &data) {
    QByteArray lzCompressed = m_lz77->compress(data);
    return m_huffman->compress(lzCompressed);
}

QByteArray DeflateAlgorithm::decompress(const QByteArray &data, quint32 originalSize) {
    // Для распаковки Хаффмана размер не так важен, он распакует до конца
    QByteArray huffmanDecompressed = m_huffman->decompress(data, 0);
    return m_lz77->decompress(huffmanDecompressed, originalSize);
}