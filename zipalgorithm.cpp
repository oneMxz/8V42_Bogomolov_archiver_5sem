#include "zipalgorithm.h"
#include "deflatealgorithm.h"

ZipAlgorithm::ZipAlgorithm() {
    m_deflate = new DeflateAlgorithm();
}

ZipAlgorithm::~ZipAlgorithm() {
    delete m_deflate;
}

QString ZipAlgorithm::getName() const {
    return "ZIP";
}

QByteArray ZipAlgorithm::compress(const QByteArray &data) {
    // В учебных целях ZIP использует Deflate как ядро
    return m_deflate->compress(data);
}

QByteArray ZipAlgorithm::decompress(const QByteArray &data, quint32 originalSize) {
    return m_deflate->decompress(data, originalSize);
}