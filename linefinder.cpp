#include "linefinder.h"
#include <QDebug>
#include <vector>
#include <cmath>
#include <limits>
lineFinder::lineFinder()
{
    testParam = new testparamset;
    trackedLine[0]=trackedLine[1]=trackedLine[2]=trackedLine[3]=0;
}

cv::Mat lineFinder::testAction(cv::Mat& src)
{
    cv::Mat backg = getEdge(src);
    rowsConst=src.rows;
    colsConst=src.cols;

    std::vector<cv::Vec4i> lines = findLines(backg);
    foundLines=lines;
    updateTrackedLine();

    if(testParam->do1){
        backg = drawLines(backg,lines,cv::Scalar(0,255,0));

        cv::Vec4i midLine;
        midLine[0]=midLine[2]=src.cols/2;
        midLine[1]=0;
        midLine[3]=src.rows-1;
        std::vector<cv::Vec4i> midLines;
        midLines.push_back(midLine);
        backg = drawLines(backg,midLines,cv::Scalar(255,0,0));

        cv::Vec4i predictLine;
        predictLine[0]=colsConst/2;
        predictLine[1]=rowsConst/2;
        predictLine[2]=calIntersection(trackedLine,rowsConst,0.5);
        predictLine[3]=rowsConst/2;
        std::vector<cv::Vec4i> predictLines;
        predictLines.push_back(predictLine);
        backg = drawLines(backg,predictLines,cv::Scalar(0,0,255));
    }
        return backg;
}

void lineFinder::updateTrackedLine()
{
    if(trackedLine[0]==0&&trackedLine[1]==0&&trackedLine[2]==0&&trackedLine[3]==0){
        selectLine();
    }else{
        int intersectionOfTracked=calIntersection(trackedLine,rowsConst,0.5);
        int minDis=1000;
        int minDisIdx=0;
        for(int i=0;i<foundLines.size();i++){
            int intersectionTemp=calIntersection(foundLines[i],rowsConst,0.5);
            if(std::abs(intersectionOfTracked-intersectionTemp)<minDis){
                minDis=std::abs(intersectionOfTracked-intersectionTemp);
                minDisIdx=i;
            }
        }
        if(minDis<colsConst/5){
            trackedLine=foundLines[minDisIdx];
        }
    }
}

void lineFinder::selectLine()
{
    int minDistance=colsConst;
    int minDisIndex=0;
    for(int i=0;i<foundLines.size();i++){
        int intersectionTemp=calIntersection(foundLines[i],rowsConst,0.5);
        if(std::abs(colsConst/2-intersectionTemp)<minDistance){
            minDistance=std::abs(colsConst/2-intersectionTemp);
            minDisIndex=i;
        }
    }
    trackedLine=foundLines[minDisIndex];
}

cv::Mat lineFinder::getEdge(cv::Mat const& src1)
{
    cv::Mat src,dst;
    cv::cvtColor(src1,src,CV_BGR2GRAY);
    src.convertTo(src,CV_8UC1);

    cv::Mat edge(src.rows,src.cols,src.type(),cv::Scalar(0));

    int winWidth=int(src.rows/5);
    int winheight=int(src.cols/20);
    cv::Mat window(winWidth,winheight,src.type(),cv::Scalar(0));

    //cv::equalizeHist(src,src);
    cv::Mat srcUp=src(cv::Rect(0,0,src.cols-1,src.rows/2-1));
    cv::Mat srcDown=src(cv::Rect(0,src.rows/2,src.cols-1,src.rows/2-1));
//    int globalMin = findKthMin(src,5);
    int globalMinUp = findKthMin(srcUp,10);
    int globalMinDown = findKthMin(srcDown,10);

//    qDebug()<<"globalMinUp: "<<globalMinUp<<endl;
//    qDebug()<<"globalMinDown: "<<globalMinDown<<endl;

    for(int i=0;i<src.rows-winWidth;i+=int(winWidth*2/4)){
        for(int j=0;j<src.cols-winheight;j+=int(winheight*2/4)){

            for(int m=0;m<winWidth;m++){
                for(int n=0;n<winheight;n++){
                    window.at<uchar>(m,n)=src.at<uchar>(i+m,j+n);
                }
            }

            int localMin=findKthMin(window,10);

            for(int m=0;m<winWidth;m++){
                for(int n=0;n<winheight;n++){
                    if(m<winWidth/2){
                        if(window.at<uchar>(m,n)<localMin&&window.at<uchar>(m,n)<globalMinUp){
                            window.at<uchar>(m,n)=1;
                        }else{
                            window.at<uchar>(m,n)=0;
                        }
                    }else{
                        if(window.at<uchar>(m,n)<localMin&&window.at<uchar>(m,n)<globalMinDown){
                            window.at<uchar>(m,n)=1;
                        }else{
                            window.at<uchar>(m,n)=0;
                        }
                    }
                }
            }

            for(int m=0;m<winWidth;m++){
                for(int n=0;n<winheight;n++){
                    if(window.at<uchar>(m,n)>0){
                        edge.at<uchar>(i+m,j+n)=255;
                    }
                }
            }
        }
    }

   edge=rmEdgeByGraDir(src,edge);
   edge=thinImage(edge,-1);

    cv::cvtColor(edge,dst,CV_GRAY2BGR);
    return dst;
}

std::vector<cv::Vec4i>  lineFinder::findLines(cv::Mat& edge)
{
      cv::cvtColor(edge,edge,CV_BGR2GRAY);
      std::vector<cv::Vec4i> lines;

      int countNonZero = cv::countNonZero(edge);
      int threshold = countNonZero/9;

      cv::HoughLinesP(edge, lines, 10, CV_PI/180, threshold, threshold, edge.rows );
//      qDebug()<<"countNonZero: "<<countNonZero ;
    //  qDebug()<<"threshold: "<<threshold<<endl<<endl ;

      std::vector<cv::Vec4i>  myLineCluster;

      myLineCluster = lineCluster(lines,edge.rows);

  //    qDebug()<<"size of lines: "<<lines.size();

      cv::cvtColor(edge,edge,CV_GRAY2BGR);
      return myLineCluster;
}

int lineFinder::findKthMin(cv::Mat& src,float percent)
{
    int min=0;

    int len=src.rows*src.cols;
    int arrays[len];

    for(int i=0;i<src.rows;i++){
        for(int j=0;j<src.cols;j++){
            arrays[src.cols*i+j]=src.at<uchar>(i,j);
        }
    }
    std::nth_element(arrays,arrays+int(len/percent),arrays+len);
    min=arrays[int(len/percent)];

    return min;
}

std::vector<cv::Vec4i> lineFinder::lineCluster(std::vector<cv::Vec4i> lines,int rows)
{
    std::vector<cv::Vec4i> clusteredLines;
    QList<int> intersectionList;
    QList<int> clusteredNum;
    for( size_t i = 0; i < lines.size(); i++ ){
      cv::Vec4i& l = lines[i];
      bool verticalFlag=false;
      if((l[0]-l[2])==0){
         verticalFlag=true;
      }else if(std::abs((l[1]-l[3])/(double)(l[0]-l[2]))>1.732){
          verticalFlag=true;
      }
      if(verticalFlag){
          int intersectionTemp=calIntersection(l,rows,0.5);
          if(intersectionList.isEmpty()){
              intersectionList.append(intersectionTemp);
              clusteredNum.append(1);
              clusteredLines.push_back(l);
          }else{
              bool addClusterFlag=true;
              for(int i=0;i<intersectionList.size();i++){
                  if(std::abs(intersectionTemp-intersectionList.at(i))<10){
                      addClusterFlag=false;

                      clusteredNum[i]++;

                      int updatedIntersection=
                              ((clusteredNum[i]-1)*intersectionList.at(i)
                               +intersectionTemp)/clusteredNum[i];
                      intersectionList[i]=updatedIntersection;

                      cv::Vec4i lineTemp;
                      lineTemp[1]=0;
                      lineTemp[3]=rows;
                      int stored0=calIntersection(clusteredLines[i],rows,0);
                      int stored2=calIntersection(clusteredLines[i],rows,1);
                      int new0=calIntersection(l,rows,0);
                      int new2=calIntersection(l,rows,1);
                      lineTemp[0]=(stored0*(clusteredNum[i]-1)+new0)/clusteredNum[i];
                      lineTemp[2]=(stored2*(clusteredNum[i]-1)+new2)/clusteredNum[i];

                      clusteredLines[i]=lineTemp;
                  }
              }
              if(addClusterFlag){
                  intersectionList.append(intersectionTemp);
                  clusteredNum.append(1);
                  clusteredLines.push_back(l);
              }
          }
      }
    }

//    qDebug()<<"size of lineCluster[0]: "<<lineCluster[0].size();
    return clusteredLines;
}

int lineFinder::calIntersection(cv::Vec4i l, int rows,float location)
{
    int temp=int(rows*location);
    if(temp>=rows){
        temp=rows-1;
    }else if(temp<0){
        temp=0;
    }
    return((temp-l[3])/((l[1]-l[3])/(double)(l[0]-l[2]))+l[2]);
}

cv::Mat lineFinder::rmEdgeByGraDir(cv::Mat &src,cv::Mat& edge)
{
    cv::Mat edgeSobel;
    cv::Sobel(src,edgeSobel,CV_8U,1,0);
    for(int i=0;i<src.rows;i++){
        for(int j=0;j<src.cols;j++){
            if(edge.at<uchar>(i,j)>0){
                if(edgeSobel.at<uchar>(i,j)>20){
                    edge.at<uchar>(i,j)=255;
                }else{
                    edge.at<uchar>(i,j)=0;
                }
            }
        }
    }
    return edge;
}

cv::Mat lineFinder::drawLines(cv::Mat& backg, std::vector<cv::Vec4i>& lines,cv::Scalar color)
{
   // cv::Mat temp;
        for( size_t i = 0; i < lines.size(); i++ )
        {
          cv::Vec4i l = lines[i];
          cv::line( backg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), color, 2, CV_AA);
        }
    return backg;
}

cv::Mat lineFinder::thinImage(const cv::Mat & src, const int maxIterations)
{
    cv::Mat dst;
    int width  = src.cols;
    int height = src.rows;
    src.copyTo(dst);
    int count = 0;  //记录迭代次数
    while (true)
    {
        count++;
        if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达
            break;
        std::vector<uchar *> mFlag; //用于标记需要删除的点
        //对点标记
        for (int i = 0; i < height ;++i)
        {
            uchar * p = dst.ptr<uchar>(i);
            for (int j = 0; j < width; ++j)
            {
                //如果满足四个条件，进行标记
                //  p9 p2 p3
                //  p8 p1 p4
                //  p7 p6 p5
                uchar p1 = p[j];
                if (p1 == 0) continue;
                uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
                uchar p8 = (j == 0) ? 0 : *(p + j - 1);
                uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
                uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
                uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
                uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
                uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
                uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
                if ((((((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2*255
                        && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6*255)))))
                {
                    int ap = 0;
                    if (p2 == 0 && p3 != 0) ++ap;
                    if (p3 == 0 && p4 != 0) ++ap;
                    if (p4 == 0 && p5 != 0) ++ap;
                    if (p5 == 0 && p6 != 0) ++ap;
                    if (p6 == 0 && p7 != 0) ++ap;
                    if (p7 == 0 && p8 != 0) ++ap;
                    if (p8 == 0 && p9 != 0) ++ap;
                    if (p9 == 0 && p2 != 0) ++ap;

                    if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
                    {
                        //标记
                        mFlag.push_back(p+j);
                    }
                }
            }
        }

        //将标记的点删除
        for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
        {
            **i = 0;
        }

        //直到没有点满足，算法结束
        if (mFlag.empty())
        {
            break;
        }
        else
        {
            mFlag.clear();//将mFlag清空
        }

        //对点标记
        for (int i = 0; i < height; ++i)
        {
            uchar * p = dst.ptr<uchar>(i);
            for (int j = 0; j < width; ++j)
            {
                //如果满足四个条件，进行标记
                //  p9 p2 p3
                //  p8 p1 p4
                //  p7 p6 p5
                uchar p1 = p[j];
                if (p1 == 0) continue;
                uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
                uchar p8 = (j == 0) ? 0 : *(p + j - 1);
                uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
                uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
                uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
                uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
                uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
                uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

                if ((((((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2*255
                        && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6*255)))))
                {
                    int ap = 0;
                    if (p2 == 0 && p3 != 0) ++ap;
                    if (p3 == 0 && p4 != 0) ++ap;
                    if (p4 == 0 && p5 != 0) ++ap;
                    if (p5 == 0 && p6 != 0) ++ap;
                    if (p6 == 0 && p7 != 0) ++ap;
                    if (p7 == 0 && p8 != 0) ++ap;
                    if (p8 == 0 && p9 != 0) ++ap;
                    if (p9 == 0 && p2 != 0) ++ap;

                    if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
                    {
                        //标记
                        mFlag.push_back(p+j);
                    }
                }
            }
        }

        //将标记的点删除
        for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
        {
            **i = 0;
        }

        //直到没有点满足，算法结束
        if (mFlag.empty())
        {
            break;
        }
        else
        {
            mFlag.clear();//将mFlag清空
        }
    }
    return dst;
}
