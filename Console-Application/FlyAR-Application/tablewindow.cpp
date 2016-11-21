#include <QtWidgets>

#include "tablewindow.h"
#include "objtable.h"

TableWindow::TableWindow()
{
    objTable = new ObjTable;
    setCentralWidget(objTable);

    setWindowTitle(tr("FlyAR Objects"));
    setFixedSize(750, 300);
}

void TableWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
