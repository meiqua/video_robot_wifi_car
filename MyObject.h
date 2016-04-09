#ifndef MYOBJECT_H
#define MYOBJECT_H
#include<QObject>
#include<TcpClient.h>

//#include <opencv2/video/background_segm.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
class MyObject: public QObject
{
    Q_OBJECT
public:
    MyObject();
    TcpClient *tcpClient;
//    cv::VideoCapture videoCapture;
public slots:
    void objectSlot(void);
signals:
    void objectSignal(void);
protected:
};
#endif // MYOBJECT_H
