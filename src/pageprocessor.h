// Image processing for one scanned page: perspective correction to the four
// corners the user placed, size limit and the colour / grey / black-and-white
// filters. Pure functions on QImage, safe to run in a worker thread.

#ifndef PAGEPROCESSOR_H
#define PAGEPROCESSOR_H

#include <QImage>
#include <QPolygonF>

namespace PageProcessor {

// Values match the filter choice in QML (CropPage).
enum Filter {
    FilterColor = 0,
    FilterGray = 1,
    FilterBlackWhite = 2
};

// Loads a photo upright (applies the EXIF orientation the camera wrote).
QImage loadImage(const QString &path);

// Cuts out the quadrilateral given by four corners - top left, top right,
// bottom right, bottom left, each normalised to 0..1 of the image - and maps
// it onto an upright rectangle no longer than maxSide pixels.
QImage warp(const QImage &source, const QPolygonF &normalizedCorners, int maxSide);

QImage applyFilter(const QImage &image, Filter filter);

QImage rotateClockwise(const QImage &image);

} // namespace PageProcessor

#endif
