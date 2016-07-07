#ifndef LINEFINDER_H
#define LINEFINDER_H
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <testparamset.h>

class lineFinder:public QObject
{
    Q_OBJECT
public:
    lineFinder();

    cv::Mat thinImage(const cv::Mat & src, const int maxIterations);

    int findKthMin(cv::Mat& src,int percent);

    cv::Mat getEdge(cv::Mat const& src);

    std::vector<cv::Vec4i> findLines(cv::Mat& edge);

    std::vector<cv::Vec4i>  lineCluster(std::vector<cv::Vec4i> lines,int rows);

    int calIntersection(cv::Vec4i line,int rows,float location);

    cv::Mat rmEdgeByGraDir(cv::Mat& src,cv::Mat& edge,int shift);

    cv::Mat drawLines(cv::Mat& backg, std::vector<cv::Vec4i>& lines,cv::Scalar color);

    cv::Mat testAction(cv::Mat& src);

     testparamset* testParam;

     cv::Vec4i trackedLine;
     void updateTrackedLine();
     int rowsConst,colsConst;
     std::vector<cv::Vec4i> foundLines;
public slots:
     void selectLine();
 signals:
     lineMiss();
};

#endif // LINEFINDER_H
