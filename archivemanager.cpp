#include "archivemanager.h"
#include "lz77algorithm.h"
#include "huffmanalgorithm.h"
#include "zipalgorithm.h"
#include <QFile>
#include <QFileInfo>
#include <QDataStream>

ArchiveManager::ArchiveManager() : in(stdin), out(stdout) {
    currentArchive = new Archive();
    currentArchive->algorithm = new Lz77Algorithm(); // Default
}

ArchiveManager::~ArchiveManager() {
    delete currentArchive;
}

void ArchiveManager::runMenu() {
    out << "ARCHIVER (Qt/C++)\n";
    while (true) {
        out << "\nCurrent algorithm: " << currentArchive->algorithm->getName() << "\n";
        out << "1. Compress file\n";
        out << "2. Decompress file\n";
        out << "3. Select algorithm\n";
        out << "0. Exit\n";
        out << "Choose action: ";
        out.flush();

        QString choice = in.readLine();
        if (choice.isNull() || in.atEnd()) {
            out << "\nExiting...\n";
            break;
        }

        choice = choice.trimmed();
        if (choice == "0" || choice == "quit") break;
        else if (choice == "1") compressFile();
        else if (choice == "2") decompressFile();
        else if (choice == "3") selectAlgorithm();
        else out << "Invalid choice!\n";
    }
}

void ArchiveManager::selectAlgorithm() {
    out << "Available algorithms:\n";
    out << "1. LZ77\n";
    out << "2. Huffman\n";
    out << "3. ZIP (Deflate = LZ77+Huffman)\n";
    out << "Select: ";
    out.flush();

    QString algo = in.readLine().trimmed();

    if (currentArchive->algorithm) {
        delete currentArchive->algorithm;
        currentArchive->algorithm = nullptr;
    }

    if (algo == "1") currentArchive->algorithm = new Lz77Algorithm();
    else if (algo == "2") currentArchive->algorithm = new HuffmanAlgorithm();
    else if (algo == "3") currentArchive->algorithm = new ZipAlgorithm();
    else {
        out << "Invalid choice. Previous algorithm kept (reset to LZ77).\n";
        currentArchive->algorithm = new Lz77Algorithm(); // Fallback
        return;
    }

    out << "Algorithm changed to: " << currentArchive->algorithm->getName() << "\n";
}

quint32 ArchiveManager::calculateChecksum(const QByteArray &data) const {
    quint32 checksum = 0;
    for (char byte : data) {
        checksum = (checksum + static_cast<quint8>(byte)) & 0xFFFFFFFF;
    }
    return checksum;
}

void ArchiveManager::compressFile() {
    out << "Enter file path to compress: ";
    out.flush();
    QString filePath = in.readLine().trimmed();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        out << "Error: File not found or cannot be opened!\n";
        return;
    }
    QByteArray originalData = file.readAll();
    file.close();

    out << "Compressing (" << originalData.size() << " bytes)...\n";

    QString algoName = currentArchive->algorithm->getName();
    if (algoName == "LZ77") {
        int recommendedSize = getWindowSizeRecommendation(filePath);
        out << "Smart window size selected: " << recommendedSize << " bytes\n";
        delete currentArchive->algorithm;
        currentArchive->algorithm = new Lz77Algorithm(recommendedSize);
    }

    QByteArray compressedData = currentArchive->algorithm->compress(originalData);

    QString outPath = filePath + ".arc";

    QFile outFile(outPath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        out << "Error: Cannot create output file!\n";
        return;
    }

    QDataStream stream(&outFile);
    stream.setVersion(QDataStream::Qt_5_15);

    stream.writeRawData("ARC2", 4);
    stream << (quint32)originalData.size();

    quint32 checksum = calculateChecksum(originalData);
    stream << checksum;

    stream.writeRawData(compressedData.constData(), compressedData.size());
    outFile.close();

    currentArchive->files.append(new ArchiveFile(QFileInfo(filePath).fileName(),
                                                 originalData.size(),
                                                 compressedData.size()));

    double ratio = (1.0 - (double)compressedData.size() / originalData.size()) * 100.0;
    out << "Success! Compression ratio: " << QString::number(ratio, 'f', 2) << "%\n";
    out << "Saved as: " << outPath << "\n";
}

void ArchiveManager::decompressFile() {
    out << "Enter archive path (.arc): ";
    out.flush();
    QString filePath = in.readLine().trimmed();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        out << "Error: Archive not found or cannot be opened!\n";
        return;
    }

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_15);
    char sig[5] = {0};
    stream.readRawData(sig, 4);
    if (QString(sig) != "ARC2") {
        out << "Error: Invalid archive format or old version!\n";
        return;
    }

    quint32 originalSize, expectedChecksum;
    stream >> originalSize >> expectedChecksum;

    QByteArray compressedData = file.readAll();
    file.close();

    out << "Decompressing...\n";

    QByteArray decompressedData = currentArchive->algorithm->decompress(compressedData, originalSize);

    // Проверка контрольной суммы
    quint32 actualChecksum = calculateChecksum(decompressedData);
    if (actualChecksum != expectedChecksum) {
        out << "\n[WARNING] Checksum mismatch! The archive may be corrupted.\n";
    } else {
        out << "Checksum verified: OK\n";
    }

    QString outPath = filePath;
    if (outPath.endsWith(".arc")) {
        outPath.chop(4);
    }

    QFileInfo originalInfo(outPath);
    QString baseName = originalInfo.baseName(); // для test1.txt вернет test1
    QString extension = originalInfo.suffix();  // для test1.txt вернет txt
    QString dir = originalInfo.absolutePath();
    if (extension.isEmpty()) {
        outPath = dir + "/" + baseName + "_restored";
    } else {
        outPath = dir + "/" + baseName + "_restored." + extension;
    }
    // --------------------------------------

    QFile outFile(outPath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        out << "Error: Cannot create output file!\n";
        return;
    }
    outFile.write(decompressedData);
    outFile.close();

    out << "Successfully decompressed! Saved as: " << outPath << "\n";
    out << "Data size: " << decompressedData.size() << " bytes\n";
}

int ArchiveManager::getWindowSizeRecommendation(const QString &filePath) const {
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    qint64 fileSize = fileInfo.size();

    // Базовые рекомендации (перенос из Python)
    QMap<QString, int> baseRecommendations = {
        {"txt", 8192}, {"log", 16384}, {"csv", 12288}, {"xml", 10240},
        {"json", 10240}, {"html", 12288}, {"py", 8192}, {"cpp", 8192},
        {"c", 8192}, {"java", 8192}, {"js", 8192}, {"bmp", 4096},
        {"tiff", 6144}, {"jpg", 1024}, {"jpeg", 1024}, {"png", 1024},
        {"gif", 1024}, {"zip", 512}, {"rar", 512}, {"7z", 512},
        {"exe", 6144}, {"dll", 6144}, {"bin", 4096}, {"pdf", 2048},
        {"doc", 3072}, {"docx", 2048}
    };

    int baseSize = baseRecommendations.value(ext, 4096);

    // Адаптация по размеру файла
    if (fileSize < 1024) {
        return qMin(256, qMax(64, (int)fileSize * 2));
    } else if (fileSize < 10240) {
        return qMin(baseSize, qMax(512, (int)fileSize / 2));
    } else if (fileSize < 102400) {
        return qMin(baseSize * 2, qMax(1024, (int)fileSize / 4));
    } else if (fileSize < 1048576) {
        return qMin(baseSize * 4, qMax(2048, (int)fileSize / 8));
    } else {
        return qMin(65535, qMax(4096, baseSize * 4));
    }
}