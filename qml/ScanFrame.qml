import QtQuick 2.12

// The quadrilateral between the four corners on CropPage, drawn over the
// photo, with the area outside it dimmed.
Canvas {
    id: frame
    property var app
    property Item photo
    property var corners

    onCornersChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    Connections {
        target: frame.photo
        onPaintedWidthChanged: frame.requestPaint()
        onPaintedHeightChanged: frame.requestPaint()
    }

    function point(i) {
        var offX = photo.x + (photo.width - photo.paintedWidth) / 2
        var offY = photo.y + (photo.height - photo.paintedHeight) / 2
        return { x: offX + corners[2 * i] * photo.paintedWidth,
                 y: offY + corners[2 * i + 1] * photo.paintedHeight }
    }

    onPaint: {
        var ctx = getContext("2d")
        ctx.reset()
        var p = [point(0), point(1), point(2), point(3)]

        // Dim everything, then cut the page out of the dimmed layer.
        ctx.fillStyle = "rgba(0, 0, 0, 0.45)"
        ctx.fillRect(0, 0, width, height)
        ctx.globalCompositeOperation = "destination-out"
        ctx.beginPath()
        ctx.moveTo(p[0].x, p[0].y)
        for (var i = 1; i < 4; ++i)
            ctx.lineTo(p[i].x, p[i].y)
        ctx.closePath()
        ctx.fill()

        ctx.globalCompositeOperation = "source-over"
        ctx.strokeStyle = app.colorScanFrame
        ctx.lineWidth = Math.max(2, app.gu * 0.3)
        ctx.stroke()
    }
}
