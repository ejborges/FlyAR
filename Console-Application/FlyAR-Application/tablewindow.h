#ifndef TABLEWINDOW_H
#define TABLEWINDOW_H

#include <QTableWidget>
#include <QMainWindow>

class ObjTable;

class TableWindow : public QMainWindow
{
    Q_OBJECT

public:
    TableWindow();

private:
    ObjTable *objTable;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

};

#endif // TABLEWINDOW_H
