#include "linefinder.h"
#include <QDebug>
#include <vector>
#include <cmath>
#include <limits>

cv::Mat lineFinder::testAction(cv::Mat& src)
{
    std::vector< std::vector<cv::Vec4i> > clusteredLines = findLines(src);

        cv::Mat backg = getEdge(src);

    if(clusteredLines.size()>1)
    {
        std::vector<cv::Vec4i>& lineUp = clusteredLines.at(0);
        std::vector<cv::Vec4i>& lineDown = clusteredLines.at(1);
        std::vector<cv::Vec4i> lines;
        lines.insert(lines.end(),lineUp.begin(),lineUp.end());
        lines.insert(lines.end(),lineDown.begin(),lineDown.end());

//        qDebug()<<"size of clustered lineUp: "<<lineUp.size();
//        qDebug()<<"size of clustered lineDown: "<<lineDown.size();

        backg = drawLinesOn(backg,lines);

        std::vector<cv::Vec4i> preLines = predictLines(clusteredLines,src.cols);
        for( size_t i = 0; i < preLines.size(); i++ )
        {
          //qDebug()<<"break here"<<endl;
          cv::Vec4i l = preLines[i];
          cv::line( backg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,255,0), 2, CV_AA);
        }

    }

    return backg;
}

cv::Mat lineFinder::getEdge(cv::Mat const& src1)
{
     cv::Mat src,edge,cannyEdge,binaryEdge,noUse;
     double lightThresh,graThresh;
    cv::cvtColor(src1,src,CV_BGR2GRAY);

    src.convertTo(src,CV_8UC1);
    cv::blur(src,src,cv::Size(3,3),cv::Point(-1,-1));

    graThresh = cv::threshold(findGraMagni(src), noUse, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    noUse.release();

       cv::Canny(src,cannyEdge,graThresh/2,graThresh,3);
       lightThresh =  findThreshAtEdge(src,cannyEdge);
       binaryEdge = findEdgeByBinary(src,lightThresh);
       edge = edgeCombine(cannyEdge,binaryEdge);
       edge = rmEdge(src,edge);

for(int i=0;i<1;i++)
{
    graThresh = findThreshAtEdge(findGraMagni(src),edge);
    cv::Canny(src,cannyEdge,graThresh/2,graThresh,3);
    lightThresh =  findThreshAtEdge(src,cannyEdge);
    binaryEdge = findEdgeByBinary(src,lightThresh);
    edge = edgeCombine(cannyEdge,binaryEdge);
    edge = rmEdge(src,edge);

}

    cv::cvtColor(edge,edge,CV_GRAY2BGR);
    return edge;
}

cv::Mat lineFinder::findGraMagni(cv::Mat& src)
{
    cv::Mat gx,gy,graMagni;
    cv::Sobel(src,gx,CV_32FC1,1,0);
    cv::Sobel(src,gy,CV_32FC1,0,1);

    cv::magnitude(gx,gy,graMagni);
    graMagni.convertTo(graMagni,CV_8UC1);

    return graMagni;
}

cv::Mat lineFinder::rmEdge(cv::Mat& src,cv::Mat& edge)
{
//    cv::Mat edge = findEdgeByCanny(src);
 //   cv::Mat graPhase = findGraPhase(src);

    cv::Mat gx,gy;

    cv::Sobel(src,gx,CV_16S,1,0);
    cv::Sobel(src,gy,CV_16S,0,1);



    int rows = edge.rows;
    int cols = edge.cols;
    short a;
    for(int i=0;i<rows;i++)
    {
        uchar* edgeElement = edge.ptr<uchar>(i);
        for(int j=0;j<cols;j++)
        {
            if(edgeElement[j]>0)
            {
                a = cv::fastAtan2(gy.at<short>(i, j), gx.at<short>(i, j));

//                if(a!=0)
//                qDebug()<<a<<endl;

                if(a>180)
                {
                    a = a - 360;
                }

                if((a>-30&&a<30)||(a<-150||a>150)||
                        (i<rows/2&&a>0) ||
                        (i>=rows/2&&a<0))
                {
                    edgeElement[j] = 0;
                }
            }
        }
    }
    return edge;
}

double lineFinder::findThreshAtEdge( cv::Mat const& src, cv::Mat const& edgeOriginal)
{
//    cv::Mat edge = findEdgeByCanny(src);
//     edge = rmEdge(edge);
    cv::Mat srcCopied = src.clone();
    cv::Mat edge;
    cv::dilate(edgeOriginal,edge,cv::Mat(3,3,CV_8U),cv::Point(-1, -1),1);

    int rows = src.rows;
    int cols = src.cols;
    uchar* srcCopiedElement;
    uchar* edgeElement;
    for(int i=0;i<rows;i++)
    {
        edgeElement = edge.ptr<uchar>(i);
        srcCopiedElement = srcCopied.ptr<uchar>(i);
        for(int j=0;j<cols;j++)
        {
            if(edgeElement[j]==0)
            {
                srcCopiedElement[j] = 0;
            }
        }
    }

    int countNonZero = cv::countNonZero(srcCopied);
    int count = 0;
    cv::Mat matToFindThre(1,countNonZero,CV_8UC1);
    uchar* matToFindThreElement = matToFindThre.ptr<uchar>(0);

    for(int i=0;i<rows;i++)
    {
        srcCopiedElement = srcCopied.ptr<uchar>(i);
        for(int j=0;j<cols;j++)
        {
            if(srcCopiedElement[j]>0)
            {
                matToFindThreElement[count] = srcCopiedElement[j];
                count++;
            }
        }
    }
    cv::Mat noUse;
    double LightThresh = cv::threshold(matToFindThre, noUse, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    return LightThresh;
}

cv::Mat lineFinder::findEdgeByBinary(cv::Mat& src,double Thresh)
{
    cv::Mat binarySrc,binaryEdge;
    cv::threshold(src, binarySrc, Thresh, 255, CV_THRESH_BINARY);
    // cv::Laplacian(binarySrc,binaryEdge,CV_8U);
   // binarySrc.convertTo(binarySrc,CV_8UC1);
    cv::Canny(binarySrc,binaryEdge,254,254,3);

    return binaryEdge;
}

cv::Mat lineFinder::edgeCombine(cv::Mat& cannyEdge,cv::Mat& binaryEdge)
{
//    cv::Mat cannyEdge,binaryEdge;
//    cannyEdge = findEdgeByCanny(src);
//    binaryEdge = findEdgeByBinary(src);
   cv::dilate(binaryEdge,binaryEdge,cv::Mat(2,2,CV_8U),cv::Point(-1, -1),1);
    int rows = cannyEdge.rows;
    int cols = cannyEdge.cols;
    uchar* binaryEdgeElement;
    uchar* cannyEdgeElement;
    for(int i=0;i<rows;i++)
    {
        cannyEdgeElement = cannyEdge.ptr<uchar>(i);

        for(int j=0;j<cols;j++)
        {
            if(cannyEdgeElement[j]>0)
            {
                binaryEdgeElement = binaryEdge.ptr<uchar>(i);
                if(binaryEdgeElement[j]==0)
                {
                    cannyEdgeElement[j]=0;
                }
            }
        }
    }
    return cannyEdge;
}

std::vector< std::vector<cv::Vec4i> > lineFinder::findLines(cv::Mat& src)
{
      cv::Mat edge = getEdge(src);

      cv::cvtColor(edge,edge,CV_BGR2GRAY);
      std::vector<cv::Vec4i> lines;

      int countNonZero = cv::countNonZero(edge);

      cv::HoughLinesP(edge, lines, 3, CV_PI/180*3, countNonZero/10, countNonZero/10, src.cols );

      std::vector< std::vector<cv::Vec4i> > myLineCluster =
              lineCluster(lines,edge.rows);

  //    qDebug()<<"size of lines: "<<lines.size();

      cv::cvtColor(edge,edge,CV_GRAY2BGR);
      return myLineCluster;
}

std::vector<cv::Vec4i> lineFinder::predictLines(std::vector<std::vector<cv::Vec4i> >& clusteredLines,int cols)
{
    std::vector<cv::Vec4i>& lineUp = clusteredLines.at(0);
    std::vector<cv::Vec4i>& lineDown = clusteredLines.at(1);

 //   qDebug()<<"size of clustered lineUp: "<<lineDown.size();

    std::vector<cv::Vec4i> preLines;
    for(size_t upIter=0;upIter<lineUp.size();upIter++){
        for(size_t downIter=0;downIter<lineDown.size();downIter++){
            cv::Vec4i& line1 = lineUp.at(upIter);
            cv::Vec4i& line2 = lineDown.at(downIter);

            double a1 = (line1[1]-line1[3])/(double)(line1[0]-line1[2]);
            double b1 = line1[1]-a1*line1[0];

            double a2 = (line2[1]-line2[3])/(double)(line2[0]-line2[2]);
            double b2 = line2[1]-a2*line2[0];

            double x,y,a,b;
            a = (a1+a2)/2;

            if(std::abs(a1-a2)<std::numeric_limits<double>::epsilon())   {//parallel
                b = (b1+b2)/2;
            }else{
                x = (b2 - b1) / (a1 - a2);
                y = a1 * x + b1;
                b = y - a*x;
            }

            cv::Vec4i line;
            line[0] = 0;
            line[1] = int(b);
            line[2] = cols-1;
            line[3] = int(a*line[2]+b);

            preLines.push_back(line);
        }
    }
    return preLines;
}

std::vector< std::vector<cv::Vec4i> > lineFinder::lineCluster(std::vector<cv::Vec4i> lines,int rows)
{
    std::vector<cv::Vec4i> clusteredLinesUp,clusteredLinesDown;
    for( size_t i = 0; i < lines.size(); i++ )
    {
      cv::Vec4i& l = lines[i];
      double a1 = (l[1]-l[3])/(double)(l[0]-l[2]);
      if(l[1]<rows/2&&l[3]<rows/2)   {       
        if(std::abs(a1)<0.6){  //gradient bigger than 31 degree
        clusteredLinesUp.push_back(l);
        }
      }else if (l[1]>rows/2&&l[3]>rows/2)    {
        if(std::abs(a1)<0.6){  //gradient bigger than 31 degree
        clusteredLinesDown.push_back(l);
        }
      }
    }
    std::vector< std::vector<cv::Vec4i> > lineCluster;
    lineCluster.push_back(clusteredLinesUp);
    lineCluster.push_back(clusteredLinesDown);

    for(size_t iter=0;iter<lineCluster.size();iter++) {
      std::vector<cv::Vec4i>& clusteredLines = lineCluster.at(iter);
      //& is very important, because we want to change the original data
      //actually in java, & can be removed
      bool breakFlag = 0;
      size_t size = clusteredLines.size();

      while(1){

          if(size == 0)
              break;

          for( size_t i = 0; i < clusteredLines.size()-1; i++ )  {
              cv::Vec4i &line1 = clusteredLines[i];

              for( size_t j = i+1; j < clusteredLines.size(); j++ ) {
                  cv::Vec4i& line2 = clusteredLines[j];
                  if(findCombinedLine(line1,line2))  {
                      clusteredLines.erase(clusteredLines.begin()+j);
                      breakFlag = 1;
                      break;
                  }
              }
              if(breakFlag) {
                  breakFlag = 0;
                  break;
              }
          }
          if(clusteredLines.size()<size)      {
              size = clusteredLines.size();
              continue;
          }else{
              break;
          }
      }



    }

//    qDebug()<<"size of lineCluster[0]: "<<lineCluster[0].size();
    return lineCluster;


}

bool lineFinder::findCombinedLine(cv::Vec4i& line1, cv::Vec4i& line2)
{
    double a1 = (line1[1]-line1[3])/(double)(line1[0]-line1[2]);
    double b1 = line1[1]-a1*line1[0];

    double a2 = (line2[1]-line2[3])/(double)(line2[0]-line2[2]);
    double b2 = line2[1]-a2*line2[0];

//    if(std::abs(a1-a2)<std::numeric_limits<double>::epsilon())   {//parallel
    if(std::abs(a1-a2)<=0.1){ // we think 6 degree is parallel enough
        if((std::abs(b1-b2)*(1-a1*a2))<6)
            return true;
        else
            return false;
    }else if (std::abs(a1-a2)> 0.1) // > 6 degree
    {
        return false;
    }

    return false;
}

cv::Mat lineFinder::drawLinesOn(cv::Mat& backg, std::vector<cv::Vec4i>& lines)
{
   // cv::Mat temp;
    if(testParam->isFindLines)
    {
        for( size_t i = 0; i < lines.size(); i++ )
        {
          cv::Vec4i l = lines[i];
          cv::line( backg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 2, CV_AA);
        }
    }
    return backg;
}

lineFinder::lineFinder()
{
    testParam = new TestParamSet;
}
