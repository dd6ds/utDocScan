// Page image processing, see pageprocessor.h.

#include "pageprocessor.h"

#include <QImageReader>
#include <QLineF>
#include <QPainter>
#include <QTransform>

#include <algorithm>
#include <vector>

namespace {

// Share of the darkest and brightest pixels ignored when stretching the
// contrast, so a few specks do not decide the range.
const double kClipShare = 0.01;
// A range narrower than this is a blank page; stretching it only amplifies noise.
const int kMinContrastRange = 32;
// Black-and-white: a pixel is ink when it is this much darker than the
// average of its neighbourhood (Bradley's adaptive threshold).
const double kInkThreshold = 0.15;
// Neighbourhood size as a fraction of the longer page side.
const int kNeighbourhoodDivisor = 16;
// Pixels of the photo cut off around the straightened page.
const double kEdgeBleed = 2.0;

QImage stretchContrast(const QImage &gray)
{
    std::vector<int> histogram(256, 0);
    for (int y = 0; y < gray.height(); ++y) {
        const uchar *line = gray.constScanLine(y);
        for (int x = 0; x < gray.width(); ++x)
            ++histogram[line[x]];
    }
    const int clip = int(gray.width() * gray.height() * kClipShare);
    int low = 0, high = 255, sum = 0;
    while (low < 255 && (sum += histogram[low]) < clip)
        ++low;
    sum = 0;
    while (high > 0 && (sum += histogram[high]) < clip)
        --high;
    if (high - low < kMinContrastRange)
        return gray;

    uchar table[256];
    for (int v = 0; v < 256; ++v)
        table[v] = uchar(std::clamp((v - low) * 255 / (high - low), 0, 255));
    QImage result = gray;
    for (int y = 0; y < result.height(); ++y) {
        uchar *line = result.scanLine(y);
        for (int x = 0; x < result.width(); ++x)
            line[x] = table[line[x]];
    }
    return result;
}

// Summed-area table: entry (x, y) holds the sum of all pixels above and left.
std::vector<quint32> integralImage(const QImage &gray)
{
    const int w = gray.width(), h = gray.height();
    std::vector<quint32> sums(size_t(w + 1) * size_t(h + 1), 0);
    for (int y = 0; y < h; ++y) {
        const uchar *line = gray.constScanLine(y);
        quint32 rowSum = 0;
        for (int x = 0; x < w; ++x) {
            rowSum += line[x];
            sums[size_t(y + 1) * (w + 1) + x + 1] = sums[size_t(y) * (w + 1) + x + 1] + rowSum;
        }
    }
    return sums;
}

QImage blackAndWhite(const QImage &gray)
{
    const int w = gray.width(), h = gray.height();
    const std::vector<quint32> sums = integralImage(gray);
    const int radius = std::max(1, std::max(w, h) / kNeighbourhoodDivisor / 2);
    QImage result(w, h, QImage::Format_Grayscale8);
    for (int y = 0; y < h; ++y) {
        const int y0 = std::max(0, y - radius), y1 = std::min(h, y + radius + 1);
        const uchar *in = gray.constScanLine(y);
        uchar *out = result.scanLine(y);
        for (int x = 0; x < w; ++x) {
            const int x0 = std::max(0, x - radius), x1 = std::min(w, x + radius + 1);
            const quint64 area = quint64(x1 - x0) * quint64(y1 - y0);
            const quint64 total = quint64(sums[size_t(y1) * (w + 1) + x1])
                                  - sums[size_t(y0) * (w + 1) + x1]
                                  - sums[size_t(y1) * (w + 1) + x0]
                                  + sums[size_t(y0) * (w + 1) + x0];
            out[x] = (in[x] * area < total * (1.0 - kInkThreshold)) ? 0 : 255;
        }
    }
    return result;
}

} // namespace

namespace PageProcessor {

QImage loadImage(const QString &path)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);
    return reader.read();
}

QImage warp(const QImage &source, const QPolygonF &normalizedCorners, int maxSide)
{
    QPolygonF corners;
    for (const QPointF &p : normalizedCorners)
        corners << QPointF(p.x() * source.width(), p.y() * source.height());

    // The page is as wide as its longer horizontal edge and as tall as its
    // longer vertical edge; the camera sees the far edges shortened.
    const double width = std::max(QLineF(corners[0], corners[1]).length(),
                                  QLineF(corners[3], corners[2]).length());
    const double height = std::max(QLineF(corners[0], corners[3]).length(),
                                   QLineF(corners[1], corners[2]).length());
    const double scale = std::min(1.0, maxSide / std::max(width, height));
    const int outWidth = std::max(1, qRound(width * scale));
    const int outHeight = std::max(1, qRound(height * scale));

    // The corners map slightly outside the result, so the anti-aliased edge
    // of the photo (half table, half paper) is cut off instead of turning
    // into a dotted frame in black and white.
    const QPolygonF target({ QPointF(-kEdgeBleed, -kEdgeBleed),
                             QPointF(outWidth + kEdgeBleed, -kEdgeBleed),
                             QPointF(outWidth + kEdgeBleed, outHeight + kEdgeBleed),
                             QPointF(-kEdgeBleed, outHeight + kEdgeBleed) });
    QTransform transform;
    if (!QTransform::quadToQuad(corners, target, transform))
        return source.scaled(outWidth, outHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QImage result(outWidth, outHeight, QImage::Format_RGB32);
    result.fill(Qt::white);
    QPainter painter(&result);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setTransform(transform);
    painter.drawImage(0, 0, source.convertToFormat(QImage::Format_RGB32));
    return result;
}

QImage applyFilter(const QImage &image, Filter filter)
{
    switch (filter) {
    case FilterGray:
        return stretchContrast(image.convertToFormat(QImage::Format_Grayscale8));
    case FilterBlackWhite:
        return blackAndWhite(stretchContrast(image.convertToFormat(QImage::Format_Grayscale8)));
    case FilterColor:
        break;
    }
    return image;
}

QImage rotateClockwise(const QImage &image)
{
    return image.transformed(QTransform().rotate(90));
}

} // namespace PageProcessor
