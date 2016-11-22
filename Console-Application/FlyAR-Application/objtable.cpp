#include "objtable.h"

ObjTable::ObjTable(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
}

void ObjTable::fillTable(vector<obj> objVec)
{
    for (vector<obj>::iterator it = objVec.begin(); it != objVec.end(); ++it)
    {

    }
}
