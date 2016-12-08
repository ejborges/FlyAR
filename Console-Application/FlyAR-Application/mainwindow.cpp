#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    flyAR = new FlyAR;
    objTable = new QTableWidget(0, 8);
    objTable->setHorizontalHeaderLabels(QString("Type,Red,Green,Blue,X-Pos,Y-Pos,Z-Pos,Radius").split(","));
    createActions();
    createMenus();
    QWidget *central = new QWidget(0); // a central widget.
    QVBoxLayout *layout = new QVBoxLayout(central);

    QSizePolicy winPolicy = flyAR->sizePolicy();
    winPolicy.setVerticalStretch(2);
    flyAR->setSizePolicy(winPolicy);

    QSizePolicy tablePolicy = objTable->sizePolicy();
    tablePolicy.setVerticalStretch(1);
    objTable->setSizePolicy(tablePolicy);

    layout->addWidget(flyAR);
    layout->addWidget(objTable);
    central->setLayout(layout);

    setCentralWidget(central);

    setWindowTitle(tr("FlyAR"));
    setFixedSize(820, 800);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();//actually close the application
    } else {
        event->ignore();
    }

}

void MainWindow::open()
{
    if (flyAR->isModified()) {

       QMessageBox::StandardButton ret;
       ret = QMessageBox::question(this, tr("FlyAR"),
                          tr("This will overwrite any existing elements.\n"
                             "Do you want to continue?"),
                          QMessageBox::Yes | QMessageBox::Cancel);
       if (ret == QMessageBox::Yes)
       {
           clearImage();
       }
       else return; //Otherwise leave the function.
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty())
    {        
        flyAR->openImage(fileName);
        flyAR->initializeScreen();
    }
}

void MainWindow::save()
{
    saveFile();
}

void MainWindow::undoFunc()
{
    flyAR->removeLastItem(-1);
    updateTable();
}

void MainWindow::removeFunc()
{
    flyAR->removeElement();
    updateTable();
}

void MainWindow::updateTable()
{
    objTable->clearContents();
    int objCounter = 0;
    objTable->setRowCount(objCounter);

    for (vector<FlyAR::obj>::iterator it = flyAR->objVec.begin(); it != flyAR->objVec.end(); ++it)
    {
        if (objCounter >= objTable->rowCount()) objTable->insertRow(objCounter); //only insert new rows if needed
        objTable->setItem(objCounter, 0, new QTableWidgetItem(QString::number(it->type)));
        objTable->setItem(objCounter, 1, new QTableWidgetItem(QString::number(it->r)));
        objTable->setItem(objCounter, 2, new QTableWidgetItem(QString::number(it->g)));
        objTable->setItem(objCounter, 3, new QTableWidgetItem(QString::number(it->b)));
        objTable->setItem(objCounter, 4, new QTableWidgetItem(QString::number(it->x)));
        objTable->setItem(objCounter, 5, new QTableWidgetItem(QString::number(it->y)));
        objTable->setItem(objCounter, 6, new QTableWidgetItem(QString::number(it->z)));
        objTable->setItem(objCounter, 7, new QTableWidgetItem(QString::number(it->radius)));
        objCounter++;
    }
}

void MainWindow::editElement()
{
    if (flyAR->objCount > 0)
    {
        bool ok = false;
        int elementToEdit = 0;
        elementToEdit = QInputDialog::getInt(this, tr("FlyAR"),
                                            tr("Select which element to edit:"),
                                            elementToEdit,
                                            1, flyAR->objCount, 1, &ok);
        if (ok)
        {
            flyAR->editElement(elementToEdit);
        }
    }
}

void MainWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(flyAR->penColor());
    if (newColor.isValid())
        flyAR->setPenColor(newColor);
}

void MainWindow::penWidth()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("FlyAR"),
                                        tr("Select pen width:"),
                                        flyAR->penWidth(),
                                        1, 10, 1, &ok);

    if (ok) flyAR->setPenWidth(newWidth);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About FlyAR GUI"),
            tr("<p>This GUI application gives you a basic view of a field that you might "
               "be flying in.  It is assuming that your vehicle is facing outwards and "
               "is placed at the bottom center of the screen. </p>"
               "<p> To help you visualize the field. You can click the File option "
               "in the menu bar and open an image file (Google Earth) to show where the obstacles  "
               "would go from a top down sort of view.  When you click on the screen "
               "you will have to specify first what type of object you would like to show, "
               "and then add a value between 1-10 which represents the height of the object "
               "off the ground.  You will also have to specify the radius of the object "
               "that you selected. "
               "The data of each position will be stored in a comma separated file "
               "who's location is specified by you, the user. </p>"
               "<p>There are many options that are available from the application. "
               "Most of them are self explanatory. Undo last entry (removes the last entry on the board). "
               "Remove Element allows you to splice out single elements anywhere on the table. "
               "Edit element allows you to change the everything but the position of an element. "
               "You can change pen color at any time, which will be applied to any element placed after the change. </p>"));
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    undoAct = new QAction(tr("&Undo Last Entry"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undoFunc()));

    removeAct = new QAction(tr("&Remove Element"), this);
    removeAct->setShortcut(tr("Ctrl+D"));
    connect(removeAct, SIGNAL(triggered()), this, SLOT(removeFunc()));

    editElementAct = new QAction(tr("&Edit Element's Values"), this);
    editElementAct->setShortcut(tr("Ctrl+U"));
    connect(editElementAct, SIGNAL(triggered()), this, SLOT(editElement()));

    penColorAct = new QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    clearScreenAct = new QAction(tr("&Clear Screen"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
            this, SLOT(clearImage()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(undoAct);
    optionMenu->addAction(removeAct);
    optionMenu->addAction(editElementAct);
    optionMenu->addAction(penColorAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

bool MainWindow::maybeSave()
{
    if (flyAR->isModified()) {
       QMessageBox::StandardButton ret;
       ret = QMessageBox::warning(this, tr("FlyAR"),
                          tr("The data points have been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return saveFile();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile()
{
    QString initialPath = QDir::currentPath() + "/config.txt";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("Files (*.txt)"));
    if (fileName.isEmpty()) {
        return false;
    } else {

        return flyAR->saveImage(fileName);
    }
}

void MainWindow::clearImage()
{
    flyAR->clearImage();
    updateTable();
}
