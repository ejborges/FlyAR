#include "flyar.h"

FlyAR::FlyAR(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    scribbling = false;
    objCount = 0;
    objHeight = 1.0f;
    objType = 1;
    objRadius = 20.0f;
    initializeScreen();
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

bool FlyAR::saveImage(const QString &fileName)
{
    modified = false;
    writeToFile(fileName); //write to the file when saves
    return true;
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
    objCount = 0; //reset the counter
    objVec.clear(); //empty the vector
    initializeScreen();
    modified = true;
    update();
}

void FlyAR::mousePressEvent(QMouseEvent *event)
{    
    if (event->button() == Qt::LeftButton) {
        bool ok, shape, rad = false;
        lastPoint = event->pos();
        float x_pos = (lastPoint.x()-410+(objRadius/2.0f))/40.0f;
        float y_pos = (500-lastPoint.y()+(objRadius/2.0f))/50.0f;

        if ((abs(x_pos) < 1.30f && y_pos < 1.0f))
        {
            QMessageBox::warning(this, tr("Warning"),
                    tr("Can't place here. You are trying to place \n"
                       "an object that will overlay the starting location\n"
                       " of the vehicle."), QMessageBox::Ok);
            return;
        }

        for (vector<obj>::iterator it = objVec.begin(); it != objVec.end(); ++it)
        {
            if (abs(it->x - x_pos) < 1.0f && abs(it->y - y_pos) < 1.0f)
            {
                QMessageBox::warning(this, tr("Warning"),
                        tr("Can't place here. You are trying to place \n"
                           "an object that will overlay another object.\n"
                           "Try spreading your objects out more."), QMessageBox::Ok);
                return;
            }
        }

        if ((abs(x_pos) > 9.5f || y_pos < 0.55f || y_pos > 9.85f))
        {
            QMessageBox::warning(this, tr("Warning"),
                    tr("Can't place here. You are trying to place \n"
                       "an object that will be displayed off the screen.\n"), QMessageBox::Ok);
            return;
        }

        objType = QInputDialog::getInt(this, tr("FlyAR"),
                                            tr("1=Circle\n2=Square"),
                                            objType,
                                            1, 2, 1, &shape);
        if (shape) //if the user cancels at anytime, cancel the drawing of the object
        {
            objHeight = QInputDialog::getInt(this, tr("FlyAR"),
                                                tr("Give the object a height:"),
                                                objHeight,
                                                1.0f, 15.0f, 1.0f, &ok);
            if (ok) //if the user cancels at anytime, cancel the drawing of the object
            {
                objRadius = QInputDialog::getInt(this, tr("FlyAR"),
                                                    tr("Radius (10-30): "),
                                                    objRadius,
                                                    10, 30, 2, &rad);
                if (rad) //if the user cancels at anytime, cancel the drawing of the object
                {
                    drawObj(lastPoint);

                    objVec.push_back(obj());

                    objVec[objCount].type = objType;
                    objVec[objCount].r = (myPenColor.rgb() >> 16) & 0xFF;
                    objVec[objCount].g = (myPenColor.rgb() >> 8) & 0xFF;
                    objVec[objCount].b = (myPenColor.rgb()) & 0xFF;
                    objVec[objCount].x = x_pos;
                    objVec[objCount].y = y_pos;
                    objVec[objCount].z = objHeight;
                    objVec[objCount].radius = (objRadius/50.0f);
                    objCount++; //Increment the count of total objects
                    modified = true;
                }
            }
        }

        scribbling = true;
    }
}

void FlyAR::removeLastItem()
{
    if (objCount > 0)
    {
        QColor currentPenColor = penColor();
        setPenColor(Qt::white);
        setPenWidth(20);
        objRadius = objVec[objCount-1].radius*50.0f;
        objType = objVec[objCount-1].type;
        lastPoint.setX((objVec[objCount-1].x*40)+410-(objRadius/2));
        lastPoint.setY(500+(objRadius/2)-(objVec[objCount-1].y*50));
        drawObj(lastPoint);
        setPenColor(currentPenColor);
        setPenWidth(3);
        objVec.pop_back();
        --objCount;
    }
}

void FlyAR::removeElement()
{
    bool ok;
    int objElement = QInputDialog::getInt(this, tr("FlyAR"),
                                        tr("Which element do you want to remove:"),
                                        objCount,
                                        1, objCount, 1, &ok);
    if (ok && (objElement-1 < objCount)) //if the user cancels at anytime, cancel the drawing of the object
    {
        QColor currentPenColor = penColor();
        setPenColor(Qt::white);
        setPenWidth(20);
        objRadius = objVec[objElement-1].radius*50.0f;
        objType = objVec[objElement-1].type;
        lastPoint.setX((objVec[objElement-1].x*40)+410-(objRadius/2));
        lastPoint.setY(500+(objRadius/2)-(objVec[objElement-1].y*50));
        drawObj(lastPoint);
        setPenColor(currentPenColor);
        setPenWidth(3);
        objVec.erase(objVec.begin()+(objElement-1));
        --objCount;
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

void FlyAR::drawObj(const QPoint &endPoint)
{
    QPainter painter(&image);
        painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));

        if (objType == 1)
        {
            painter.drawEllipse(QRect(endPoint.x()-(objRadius/2), endPoint.y()-(objRadius/2), objRadius, objRadius));
        } else if (objType == 2)
        {
            painter.drawRect(QRect(endPoint.x()-(objRadius/2), endPoint.y()-(objRadius/2), objRadius, objRadius));
        }

        modified = true;

        int rad = myPenWidth;
        update(QRect(endPoint.x(), endPoint.y(), objRadius, objRadius).normalized()
                                         .adjusted(-rad, -rad, +rad, +rad));


}

void FlyAR::writeToFile(QString theFileName)
{

    QFile file(theFileName);
    if (!file.isOpen())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Text);
    }
    QTextStream out(&file);

    for (vector<obj>::iterator it = objVec.begin(); it != objVec.end(); ++it)
    {
        out << QString::number(it->type) + ", ";
        out << QString::number(it->r) + ", ";
        out << QString::number(it->g) + ", ";
        out << QString::number(it->b) + ", ";
        out << QString::number(it->x) + ", ";
        out << QString::number(it->y) + ", ";
        out << QString::number(it->z) + ", ";
        out << QString::number(it->radius) + "\n";
    }

    file.close();
}

void FlyAR::initializeScreen()
{
    if (image.size().isEmpty())
    {
        QImage newImage(QSize(820,800), QImage::Format_RGB32);
        newImage.fill(qRgb(255, 255, 255));
        image = newImage;
    }

    QPainter painter(&image);
    myPenColor = Qt::black;
    myPenWidth = 3;
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));

    painter.drawText(QRectF(386, 484, 60, 20),"QUAD");
    QRectF rectangle(370.0, 480.0, 60.0, 80.0);
    int startAngle = 30 * 16;
    int spanAngle = 120 * 16;
    painter.drawChord(rectangle, startAngle, spanAngle);
    update();

    myPenColor = Qt::blue;
    myPenWidth = 3;
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
