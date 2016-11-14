#include <QtWidgets>

#include "mainwindow.h"
#include "FlyAR.h"

MainWindow::MainWindow()
{
    flyAR = new FlyAR;
    setCentralWidget(flyAR);

    createActions();
    createMenus();

    setWindowTitle(tr("FlyAR"));
    setFixedSize(500, 500);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept(); //actually close the application
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
               "be racing in.  It is assuming that your Quad is facing outwards and "
               "is placed at the bottom center of the screen. </p>"
               "<p> To help you visualize the field. You can click the File option "
               "in the menu bar and open an image file to show where the obstacles  "
               "would go from a top down sort of view.  When you click on the screen "
               "you will have to specify first what type of object you would like to show, "
               "and then add a value between 1-15 which represents the height of the object "
               "off the ground.  You will also have to specify the x-radius and y-radius of the object "
               "that you selected. "
               "The data of each position will be stored in a comma separated file that "
               "is determined in the constructor of the FlyAR source file</p> "));
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }

    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

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
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
//    fileMenu->addMenu(saveAsMenu);
//    fileMenu->addSeparator();
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
//    if (FlyAR->isModified()) {
//       QMessageBox::StandardButton ret;
//       ret = QMessageBox::warning(this, tr("FlyAR"),
//                          tr("The image has been modified.\n"
//                             "Do you want to save your changes?"),
//                          QMessageBox::Save | QMessageBox::Discard
//                          | QMessageBox::Cancel);
//        if (ret == QMessageBox::Save) {
//            return saveFile("png");
//        } else if (ret == QMessageBox::Cancel) {
//            return false;
//        }
//    }
//    return true;
    return false;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
//                               initialPath,
//                               tr("%1 Files (*.%2);;All Files (*)")
//                               .arg(QString::fromLatin1(fileFormat.toUpper()))
//                               .arg(QString::fromLatin1(fileFormat)));
//    if (fileName.isEmpty()) {
//        return false;
//    } else {
//        return flyAR->saveImage(fileName, fileFormat.constData());
//    }
    return false;
}
