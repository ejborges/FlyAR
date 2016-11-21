#ifndef OBJTABLE_H
#define OBJTABLE_H

#include <QWidget>

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

signals:

public slots:

};

#endif // OBJTABLE_H
