#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "settingsdialog.h"
#include <QDir>
#include <QKeyEvent>
#include <QtNetwork>
#include <qtconcurrentrun.h>
#include <QFileDialog>
#include<QMessageBox>
#include <QSplitter>


//save picture and videocapture should move to thread

using namespace std;

// Main GUI
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

     ui->setupUi(this);

     myLineFinder = new lineFinder;

     settings = new SettingsDialog;
 //    testParam = new TestParamSet;

     intializeCMD();

     videoFrameLabel =ui->labelLeft;
     frameEdgeLabel =ui->labelRight;

     QHBoxLayout *layout = new QHBoxLayout;
     layout->addWidget(videoFrameLabel);
     layout->addWidget(frameEdgeLabel);

    centralWidget()->setLayout(layout);

     openSuccess = false;

     myObject = new MyObject();
     myThreadA = new MyThread();

     timer = new QTimer(this);

    // Connect slots
    connect(timer, SIGNAL(timeout()), this, SLOT(timerLoop()));

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(actionOpen()));
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(actionClose()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionCapture,SIGNAL(triggered()),this,SLOT(savePictures()));

    connect(ui->actionTestParamSet, SIGNAL(triggered()), myLineFinder->testParam, SLOT(show()));

    connect(myObject->tcpClient->tcpSocket,SIGNAL(connected()),this,SLOT(setConnectionSuccess()));
    connect(myObject->tcpClient->tcpSocket,SIGNAL(disconnected()),this,SLOT(setConnectionFail()));
    connect(myObject->tcpClient->tcpSocket,SIGNAL(readyRead()),this,SLOT(readCMD()));

    myObject->moveToThread(myThreadA);
    myThreadA->start();


//	// Set initial screen
 //    SetScreen(":/images/initialScreen.jpg");
        SetScreen(":/images/ceti1.png");
//     qDebug()<<videoFrameLabel->size().width();

}

// Load video to process.
void MainWindow::actionOpen() {

    // Load video and start video
    if (!timer->isActive()) {
        // Open file dialog
   //     QString fileName = QFileDialog::getOpenFileName(this, "Open Video", ".", "Video Files (*.avi *.mpg)");
   //       videoCapture.open(fileName.toStdString().c_str());//fileName.toAscii().data());

      SettingsDialog::Settings p = settings->settings();
      QString address = p.IPaddress;
      QString controlIP = p.controlIP;
      QStringList IPtemp = controlIP.split(':',QString::SkipEmptyParts);
      if(IPtemp.size() == 2) //make sure the format is IP:port
      {
          QString IP = IPtemp.at(0);
          QString port = IPtemp.at(1);
          myObject->tcpClient->newConnection(IP.toStdString().c_str(),port.toInt());
      }


      if(isDigitStr(address))
       videoCapture.open(address.toInt()); //open webcam on laptop when type in 0
      else
      {

           ui->statusBar->showMessage("opening....",2000);


            videoCapture.open((address.toStdString().c_str()));
            //This capture is blocking,unfortunately I have not find a solution yet.
         qDebug()<<"open ip address :" << videoCapture.isOpened() << endl;
      }


      if(videoCapture.isOpened())
      {
          ui->statusBar->showMessage("opening camera success",3000);

          // Get frame per second
          int fps=videoCapture.get(CV_CAP_PROP_FPS);

          //   qDebug("fps: %d", fps);

          // Set timer interval
          if(fps>0)
          timer->setInterval(1000/fps);
          else
           timer->setInterval(1000/33);  //set 33 as default fps

          timer->start();

           ui->actionOpen->setEnabled(false);
           ui->actionClose->setEnabled(true);
      }
      else
      {
          ui->statusBar->showMessage("open camera fail",3000);
          openSuccess = false;
          SetScreen(":/images/failScreen.jpg");
      }


    } else {
        timer->stop();
        ui->statusBar->showMessage("open camera fail",3000);
        openSuccess = false;
        SetScreen(":/images/failScreen.jpg");
    }
}

void MainWindow::actionClose()
{

    ui->actionOpen->setEnabled(true);
    ui->actionClose->setEnabled(false);

    if(videoCapture.isOpened())
        videoCapture.release();

    if(timer->isActive())
        timer->stop();

    SetScreen(":/images/closeScreen.jpg");
    openSuccess = false;
}

// Main loop activated with the timer.
void MainWindow::timerLoop() {

    // Capture frame from video
    videoCapture>>rawFrame;

    rawCopyFrame = rawFrame.clone();  //save as picture
    frameForEdgeDetect = rawFrame.clone();
    edge = myLineFinder->testAction(frameForEdgeDetect);

    // Check if frame was successfully captured.
    if (rawFrame.empty()) {
        timer->stop();
        ui->statusBar->showMessage("the remote camera send back empty frame",3000);
        SetScreen(":/images/failScreen.jpg");
        openSuccess = false;

    } else {
        int w ;
        int h ;
        int w2 ;
        int h2 ;
        w = videoFrameLabel->width();
        h = videoFrameLabel->height();

        w2 = frameEdgeLabel->width();
        h2 = frameEdgeLabel->height();

        cv::resize(rawFrame, rawFrame, cv::Size(rawFrame.size().width,rawFrame.size().height), 0, 0, CV_INTER_LINEAR);
        videoFrameLabel->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(rawFrame)).scaled(w,h,Qt::KeepAspectRatio));

        cv::resize(edge, edge, cv::Size(edge.size().width,edge.size().height), 0, 0, CV_INTER_LINEAR);
        frameEdgeLabel->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(edge)).scaled(w2,h2,Qt::KeepAspectRatio));

        openSuccess = true;
    }

}

void MainWindow::timeToDistribute()
{
    QCoreApplication::processEvents() ;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(!openSuccess)
   SetScreen(imageOnScreen);

}

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{


    if((event->key()==Qt::Key_W ||event->key()==Qt::Key_I || event->key()==Qt::Key_Up)
            /*&& !event->isAutoRepeat()*/)
    {
         myObject->tcpClient->sendCMD(CMDmap.value("forward"));
    }
     if((event->key()==Qt::Key_S ||event->key()==Qt::Key_K || event->key()==Qt::Key_Down)
             /*&& !event->isAutoRepeat()*/)
    {
         myObject->tcpClient->sendCMD(CMDmap.value("back"));

    }
     if((event->key()==Qt::Key_A ||event->key()==Qt::Key_J || event->key()==Qt::Key_Left)
            /* && !event->isAutoRepeat()*/)
    {
        myObject->tcpClient->sendCMD(CMDmap.value("left"));
      //  qDebug() << "a";
    }
     if((event->key()==Qt::Key_D ||event->key()==Qt::Key_L || event->key()==Qt::Key_Right)
             /*&& !event->isAutoRepeat()*/)
    {
        myObject->tcpClient->sendCMD(CMDmap.value("right"));
    }
     if(event->key()==Qt::Key_Space)
     {
         myObject->tcpClient->sendCMD(CMDmap.value("stop"));
     }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    myObject->tcpClient->sendCMD(CMDmap.value("stop"));
}

bool MainWindow::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();// convert QString to char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //not a number
        return false;
    }
    else
    { //number
        return true;
    }
}



void MainWindow::setConnectionSuccess()
{
  //  ui->statusBar->showMessage("connect control IP success",3000);
}

void MainWindow::setConnectionFail()
{
    ui->statusBar->showMessage("connect control IP fail",3000);
}

void MainWindow::readCMD()
{

}

void MainWindow::savePictures()
{
      if (!rawCopyFrame.empty())
      {
          QImage img = ImageFormat::Mat2QImage(rawCopyFrame);
          QString Path;
          if(settings->settings().picturePath == "")
          {
              Path = QFileDialog::getExistingDirectory(this, tr("Save images"),
                                                              settings->settings().picturePath,
                                                              QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
               settings->updatePath(Path);
          }

             Path = settings->settings().picturePath;

             QDir dir(Path);
             dir.setFilter(QDir::Files | QDir::NoSymLinks);
             dir.setSorting(QDir::Name | QDir::Reversed);
             QFileInfoList list = dir.entryInfoList();

             int fileNum = list.size()+1;
             Path = Path+QDir::separator()+QString::number(fileNum,10)+".png";
           //  qDebug() << "path is: " << list.size() <<endl;

             if(Path.isEmpty())
             {
                 return;
             }
             else
             {

                if(! (img.save(Path) ) ) //save
                  {
                      QMessageBox::information(this, tr("Failed to save the image"),
                                               tr("Failed to save the image!"));
                       return;
                  }
             }
      }

}

cv::Mat MainWindow::loadFromQrc(QString qrc)
{
    //double tic = double(getTickCount());

    QFile file(qrc);
    cv::Mat m;
    if(file.open(QIODevice::ReadOnly))
    {
        qint64 sz = file.size();
        std::vector<uchar> buf(sz);
        file.read((char*)buf.data(), sz);
        m = cv::imdecode(buf, cv::IMREAD_COLOR); //
    }
    //double toc = (double(getTickCount()) - tic) * 1000.0 / getTickFrequency();
    //qDebug() << "OpenCV loading time: " << toc;

    return m;
}

void MainWindow::intializeCMD()
{
    qint8 back[] = {0xff,0x00,0x01,0x00,0xff};  //must be qint8 to cast to char*
    qint8 forward[] = {0xff,0x00,0x02,0x00,0xff};
    qint8 right[] = {0xff,0x00,0x03,0x00,0xff};
    qint8 left[] = {0xff,0x00,0x04,0x00,0xff};
    qint8 stop[] = {0xff,0x00,0x00,0x00,0xff};
    CMDmap["forward"] = QString::fromLatin1((char*)forward,sizeof(forward)/sizeof(forward[0]));
//    qDebug() << "forward is： "<< CMDmap["forward"].size() <<endl;
//    qDebug() << CMDmap["forward"].toLatin1().toHex();
    CMDmap["back"] = QString::fromLatin1((char*)back,sizeof(back)/sizeof(back[0]));
    CMDmap["left"] = QString::fromLatin1((char*)left,sizeof(left)/sizeof(left[0]));
    CMDmap["right"] = QString::fromLatin1((char*)right,sizeof(right)/sizeof(right[0]));
    CMDmap["stop"] = QString::fromLatin1((char*)stop,sizeof(stop)/sizeof(stop[0]));
}


void MainWindow::SetScreen(QString qrc)
{
    cv::Mat src = loadFromQrc(qrc);
    cv::Mat edge = src.clone();

    QImage tempImage = ImageFormat::Mat2QImage(src);
    int w = videoFrameLabel->width();
    int h = videoFrameLabel->height();
    videoFrameLabel->setPixmap(QPixmap::fromImage(tempImage).scaled(w,h,Qt::KeepAspectRatio));

    w = frameEdgeLabel->width();
    h = frameEdgeLabel->height();
    frameEdgeLabel->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(myLineFinder->testAction(edge))).scaled(w,h,Qt::KeepAspectRatio));

    imageOnScreen = qrc;

//    qDebug()<<videoFrameLabel->size().width();
}

