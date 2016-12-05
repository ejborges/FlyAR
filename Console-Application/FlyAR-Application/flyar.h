#ifndef FLYAR_H
#define FLYAR_H

#include <QWidget>
#include <QtWidgets>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <vector>
#include <QStaticText>

using namespace std;

class FlyAR : public QWidget
{
    Q_OBJECT
public:

    FlyAR(QWidget *parent = 0);

    struct obj {
        int type;
        int r, g, b;
        float x, y, z;
        float radius;
    };

    vector<obj> objVec;

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setNewHeight(int newHeight);
    void removeLastItem(int objNum);
    void removeElement();
    void initializeScreen();

    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }

public slots:
    void clearImage();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void drawObj(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);
    void writeToFile(QString theFileName);

    bool modified;
    bool scribbling;
    int myPenWidth;
    int objCount;
    int objType;
    float objHeight;
    float objRadius;
    QColor myPenColor;
    QImage image;
    QPoint lastPoint;
    QString outFileName;
    QFile outFile;
    QString output;
};

#endif // FLYAR_H
