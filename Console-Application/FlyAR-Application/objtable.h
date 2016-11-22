#ifndef OBJTABLE_H
#define OBJTABLE_H

#include <QWidget>
#include <QtWidgets>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QFile>
#include <QTextStream>
#include <vector>

using namespace std;

class ObjTable : public QWidget
{
    Q_OBJECT
public:
    ObjTable(QWidget *parent = 0);

    struct obj {
        int type;
        int r, g, b;
        float x, y, z;
        float radius;
    };

    vector<obj> objVec;

    void fillTable(vector<obj> objVec);
};
#endif // OBJTABLE_H
