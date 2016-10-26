#include "mainwindow.h"
#include <windows.h>
#include <QApplication>
#include <QMouseEvent>
#include <QObject>
#include <iostream>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QPushButton>

struct object {
    int id, pos_x, pos_y, height, ringNum;
    QPushButton *button;
    QString title;
};

//--TODO-- Increment this every time someone makes an object and set that object
// to the count of it.
object *shapes[100];// = new object[100];
int ringCount = 0;
int idCount = 0;



static void handleClick()
{
    std::cout << "x-pos: " << shapes[idCount]->pos_x << std::endl << "y-pos: " << shapes[idCount]->pos_y << std::endl;
    shapes[idCount]->button->setText("Ring #: " + QString::number(++ringCount));
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //MainWindow w;
    //ui.show();
    //QGraphicsScene scene;
    //QLabel i_label("Start", window);

    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    window->setEnabled(true);
    QEvent *event;
//    window->event(event);
    window->setMinimumSize(600, 500);

    QPushButton *quitButton = new QPushButton;
    quitButton->setParent(window);
    quitButton->setGeometry(window->width()-100,window->height()-40, 40, 30);
    quitButton->setText("CLOSE");
    quitButton->setFixedSize(50, 30);
    QObject::connect(quitButton, SIGNAL(clicked()),window,SLOT(close()));

    //QObject::connect(quitButton, SIGNAL(pressed()),window, SLOT(repaint()));

//    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//    window->mousePressEvent(mouseEvent);
//    QGridLayout *layout = new QGridLayout;

//    if (event->type() == QEvent::MouseButtonPress)
//    {
//        std::cout << "Hey";
//    }


    object *currentShape = new object;
    currentShape->id = idCount;
    currentShape->pos_x = 5;
    currentShape->pos_y = 5;
    QString s = QString::number(currentShape->pos_x) + ", " + QString::number(10-currentShape->pos_y);
    currentShape->title = "(" + s + ")";
    currentShape->button = new QPushButton(currentShape->title);
    currentShape->ringNum = 0;
    QObject::connect(currentShape->button, &QPushButton::clicked, handleClick);
//            layout->addWidget(currentShape->button,currentShape->pos_y,currentShape->pos_x+5);
    currentShape->button->setGeometry(50,50,50,50);
    layout->addWidget(currentShape->button);
    shapes[idCount] = currentShape;






//    for (int i = -5; i <= 5; ++i) //x position
//    {
//        for (int j = 10; j >= 0; --j) //y position
//        {
//            object *currentShape = new object;
//            currentShape->id = idCount;
//            currentShape->pos_x = i;
//            currentShape->pos_y = j;
//            QString s = QString::number(currentShape->pos_x) + ", " + QString::number(10-currentShape->pos_y);
//            currentShape->title = "(" + s + ")";
//            currentShape->button = new QPushButton(currentShape->title);
//            currentShape->ringNum = 0;
//            QObject::connect(currentShape->button, &QPushButton::clicked, handleClick);
////            layout->addWidget(currentShape->button,currentShape->pos_y,currentShape->pos_x+5);
//            layout->addWidget(currentShape->button);
//            shapes[idCount] = currentShape;
//        }
//    }

    window->setLayout(layout);
    //window->setStyleSheet(" background-image: url(C:/grid.jpg); background-repeat: no-repeat; background-position: center; ");

    window->setWindowTitle("FlyAR");
    window->show();



//    while (true)
    {
        if (window->isWindowModified())
        {
            quitButton->setGeometry(window->width()-60,window->height()-40, 40, 30);
        }
//        if (window->event(event) == QEvent::MouseButtonPress)
        {
            std::cout << "hey you clicked the button" << std::endl;
        }
    }



//    for (qreal i = 0; i < 20; ++i)
//    {
//        for (qreal j = 0; j < 20; ++j)
//        {
//            scene.addRect(QRectF((-j*20), (-i*20), 20,20));
//        }
//    }
//    scene.addRect(QRectF(-40,-40,20,20));
//    scene.addRect(QRectF(-20,-20,20,20));
//    scene.addRect(QRectF(0,0,20,20));
//    QGraphicsView view(&scene);
//    view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
//    view.show();
    return a.exec();
}
