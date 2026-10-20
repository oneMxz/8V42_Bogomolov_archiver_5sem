#include "archive.h"
#include <QDebug>

Archive::~Archive() {
    qDeleteAll(files);
    if (algorithm) {
        delete algorithm;
    }
}