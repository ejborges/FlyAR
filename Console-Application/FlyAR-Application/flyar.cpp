#include "flyar.h"

FlyAR::FlyAR(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    scribbling = false;
    myPenWidth = 3;
    myPenColor = Qt::blue;
    outFileName = "C:/test1/Data.txt";
    objHeight = 1;
    objType = 1;
    xRadius = 20;
    yRadius = 20;
}

bool FlyAR::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}

bool FlyAR::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}

void FlyAR::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}

void FlyAR::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}

void FlyAR::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}

void FlyAR::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        bool ok, shape, xrad, yrad = false;
        lastPoint = event->pos();

        objType = QInputDialog::getInt(this, tr("FlyAR"),
                                            tr("1=Circle\n2=Square"),
                                            objType,
                                            1, 2, 1, &shape);
        if (shape)
        {
            objHeight = QInputDialog::getInt(this, tr("FlyAR"),
                                                tr("Give the object a height:"),
                                                objHeight,
                                                1, 15, 1, &ok);
            if (ok)
            {
                xRadius = QInputDialog::getInt(this, tr("FlyAR"),
                                                    tr("X-Radius (10-50): "),
                                                    xRadius,
                                                    10, 50, 2, &xrad);
                if (xrad)
                {
                    yRadius = QInputDialog::getInt(this, tr("FlyAR"),
                                                        tr("Y-Radius (10-50): "),
                                                        yRadius,
                                                        10, 50, 2, &yrad);
                    if (yrad)
                    {
                        drawLineTo(event->pos());
                        output += QString::number(objType) + ", "                           //type
                                + QString::number(myPenColor.rgb() & 0x00FFFFFF) + ", "     //color int -> hex (#00RRGGBB)
                                + QString::number((float)(250-lastPoint.x())/50.0f) + ", "  //x-coordinate
                                + QString::number((float)(500-lastPoint.y())/50.0f) + ", "  //y-coordinate
                                + QString::number((float)objHeight) + ", "                  //z-coordinate
                                + QString::number((float)xRadius) + ", "                    //x radius
                                + QString::number((float)yRadius)                           //y radius
                                + "\n";
                        writeToFile(output);
                    }
                }
            }
        }


        scribbling = true;
    }
}

void FlyAR::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        scribbling = false;
    }
}

void FlyAR::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void FlyAR::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void FlyAR::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&image);
        painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));

        if (objType == 1)
        {
            painter.drawEllipse(QRect(endPoint.x(), endPoint.y(), xRadius, yRadius));
        } else if (objType == 2)
        {
            painter.drawRect(QRect(endPoint.x(), endPoint.y(), xRadius, yRadius));
        }

        modified = true;

        int rad = (myPenWidth / 2) + 2;
        update(QRect(endPoint.x(), endPoint.y(), 25, 25).normalized()
                                         .adjusted(-rad, -rad, +rad, +rad));


}

void FlyAR::writeToFile(QString output)
{

    QFile file(outFileName);
    if (!file.isOpen())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Text);
    }
    QTextStream out(&file);

    out << output;
    file.close();
}

void FlyAR::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
