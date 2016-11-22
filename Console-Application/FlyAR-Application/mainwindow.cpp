#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    flyAR = new FlyAR;
    objTable = new ObjTable;

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
    setFixedSize(800, 800);
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
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty())
        flyAR->openImage(fileName);
}

void MainWindow::save()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::updateTable()
{
    int objCounter = 0;
    for (vector<FlyAR::obj>::iterator it = flyAR->objVec.begin(); it != flyAR->objVec.end(); ++it)
    {
        objTable->objVec.push_back(ObjTable::obj());

        objTable->objVec[objCounter].type = it->type;
        objTable->objVec[objCounter].r = it->r;
        objTable->objVec[objCounter].g = it->g;
        objTable->objVec[objCounter].b = it->b;
        objTable->objVec[objCounter].x = it->x;
        objTable->objVec[objCounter].y = it->y;
        objTable->objVec[objCounter].z = it->z;
        objTable->objVec[objCounter].radius = it->radius;
    }

    objTable->fillTable(objTable->objVec);
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
    if (ok)
        flyAR->setPenWidth(newWidth);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About FlyAR GUI"),
            tr("<p>This GUI application gives you a basic view of a field that you might "
               "be flying in.  It is assuming that your vehicle is facing outwards and "
               "is placed at the bottom center of the screen. </p>"
               "<p> To help you visualize the field. You can click the File option "
               "in the menu bar and open an image file to show where the obstacles  "
               "would go from a top down sort of view.  When you click on the screen "
               "you will have to specify first what type of object you would like to show, "
               "and then add a value between 1-15 which represents the height of the object "
               "off the ground.  You will also have to specify the radius of the object "
               "that you selected. "
               "The data of each position will be stored in a comma separated file that "
               "is determined in the constructor of the FlyAR source file</p> "));
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

    updateTableAct = new QAction(tr("&Update Table Values"), this);
    connect(updateTableAct, SIGNAL(triggered()), this, SLOT(updateTable()));

    penColorAct = new QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    penWidthAct = new QAction(tr("Pen &Width..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    clearScreenAct = new QAction(tr("&Clear Screen"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
            flyAR, SLOT(clearImage()));

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
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
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
            return saveFile("txt");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/DataPoints." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {

        return flyAR->saveImage(fileName);
    }
}
