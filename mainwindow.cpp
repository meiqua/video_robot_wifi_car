#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "settingsdialog.h"
#include <QDir>
#include <QKeyEvent>
#include <QtNetwork>

using namespace std;

// Main GUI
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

     ui->setupUi(this);

     settings = new SettingsDialog;

     char forward[] = {0xff,0x00,0x01,0x00,0xff};
     char back[] = {0xff,0x00,0x02,0x00,0xff};
     char left[] = {0xff,0x00,0x03,0x00,0xff};
     char right[] = {0xff,0x00,0x04,0x00,0xff};
     char stop[] = {0xff,0x00,0x00,0x00,0xff};
     CMDmap["forward"] = QString::fromLatin1(forward);
     CMDmap["back"] = QString::fromLatin1(back);
     CMDmap["left"] = QString::fromLatin1(left);
     CMDmap["right"] = QString::fromLatin1(right);
     CMDmap["stop"] = QString::fromLatin1(stop);

     videoFrameLabel = new QLabel;
     videoFrameLabel->setMinimumSize(1, 1);
     videoFrameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
     setCentralWidget(videoFrameLabel);
     openSuccess = false;

     myObject = new MyObject();
     myThreadA = new MyThread();

     timer = new QTimer(this);

    // Connect slots
    connect(timer, SIGNAL(timeout()), this, SLOT(timerLoop()));

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(actionOpen()));
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(actionClose()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));

    connect(myObject->tcpClient->tcpSocket,SIGNAL(connected()),this,SLOT(setConnectionSuccess()));
    connect(myObject->tcpClient->tcpSocket,SIGNAL(disconnected()),this,SLOT(setConnectionFail()));
    connect(myObject->tcpClient->tcpSocket,SIGNAL(readyRead()),this,SLOT(readCMD()));

    myObject->moveToThread(myThreadA);
    myThreadA->start();

//	// Set initial screen
     SetScreen(":/images/initialScreen.jpg");
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
      videoCapture.open(address.toStdString().c_str());

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

    // Check if frame was successfully captured.
    if (rawFrame.empty()) {
        timer->stop();
        ui->statusBar->showMessage("open camera fail",3000);
        SetScreen(":/images/failScreen.jpg");
        openSuccess = false;

    } else {
        cv::resize(rawFrame, rawFrame, cv::Size(rawFrame.size().width,rawFrame.size().height), 0, 0, CV_INTER_LINEAR);
        frameImage = ImageFormat::Mat2QImage(rawFrame);

        // Set label pixmap to video frame
        int w = videoFrameLabel->width();
        int h = videoFrameLabel->height();
        videoFrameLabel->setPixmap(QPixmap::fromImage(frameImage).scaled(w,h,Qt::KeepAspectRatio));
        openSuccess = true;

    }

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
    if(event->key()==Qt::Key_W ||event->key()==Qt::Key_I || event->key()==Qt::Key_Up)
    {
         myObject->tcpClient->sendCMD(CMDmap.value("forward"));
       //  qDebug() << "w";
    }
     if(event->key()==Qt::Key_S ||event->key()==Qt::Key_K || event->key()==Qt::Key_Down)
    {
         myObject->tcpClient->sendCMD(CMDmap.value("back"));

    }
     if(event->key()==Qt::Key_A ||event->key()==Qt::Key_J || event->key()==Qt::Key_Left)
    {
        myObject->tcpClient->sendCMD(CMDmap.value("left"));
      //  qDebug() << "a";
    }
     if(event->key()==Qt::Key_D ||event->key()==Qt::Key_L || event->key()==Qt::Key_Right)
    {
        myObject->tcpClient->sendCMD(CMDmap.value("right"));
    }
     if(event->key()==Qt::Key_Space)
     {
         myObject->tcpClient->sendCMD(CMDmap.value("stop"));
     }
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
    ui->statusBar->showMessage("connect control IP success",3000);
}

void MainWindow::setConnectionFail()
{
    ui->statusBar->showMessage("connect control IP fail",3000);
}

void MainWindow::readCMD()
{

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

void MainWindow::SetScreen(QString qrc)
{
    cv::Mat src = loadFromQrc(qrc);
    QImage tempImage = ImageFormat::Mat2QImage(src);
    int w = videoFrameLabel->width();
    int h = videoFrameLabel->height();
    videoFrameLabel->setPixmap(QPixmap::fromImage(tempImage).scaled(w,h,Qt::KeepAspectRatio));
    imageOnScreen = qrc;
}

