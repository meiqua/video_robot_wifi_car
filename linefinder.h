#ifndef LINEFINDER_H
#define LINEFINDER_H
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <testparamset.h>

class lineFinder
{
public:
    lineFinder();

    cv::Mat findGraMagni(cv::Mat& src);

    cv::Mat rmEdge(cv::Mat& src,cv::Mat& edge);

    double findThreshAtEdge( cv::Mat const& src, cv::Mat const& edge);

    cv::Mat findEdgeByBinary(cv::Mat& src,double Thresh);

    cv::Mat edgeCombine(cv::Mat& cannyEdge,cv::Mat& binaryEdge);

    cv::Mat getEdge(cv::Mat const& src);

    std::vector< std::vector<cv::Vec4i> > findLines(cv::Mat& src);

    std::vector<cv::Vec4i> predictLines(std::vector< std::vector<cv::Vec4i> >& clusteredLines,int cols);

    std::vector< std::vector<cv::Vec4i> > lineCluster(std::vector<cv::Vec4i>& lines,int rows,int cols);

    bool findCombinedLine(cv::Vec4i& line1,cv::Vec4i& line2,int cols);

    cv::Mat drawLinesOn(cv::Mat& backg,std::vector<cv::Vec4i>& lines);

    cv::Mat testAction(cv::Mat& src);

     TestParamSet* testParam;
private:

};

#endif // LINEFINDER_H
