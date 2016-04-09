//this imageFormat class is got from github
//https://github.com/JaimeIvanCervantes/Tracking/

#include "imageformat.h"

QImage ImageFormat::Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp;
     cv::cvtColor(src, temp,CV_BGR2RGB);
     QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     QImage dest2(dest);
     dest2.detach();
     return dest2;
}
//    {
//       switch ( inMat.type() )
//       {
//          // 8-bit, 4 channel
//          case CV_8UC4:
//          {
//             QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

//             return image;
//          }

//          // 8-bit, 3 channel
//          case CV_8UC3:
//          {
//             QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

//             return image.rgbSwapped();
//          }

//          // 8-bit, 1 channel
//          case CV_8UC1:
//          {
//             static QVector<QRgb>  sColorTable;

//             // only create our color table once
//             if ( sColorTable.isEmpty() )
//             {
//                for ( int i = 0; i < 256; ++i )
//                   sColorTable.push_back( qRgb( i, i, i ) );
//             }

//             QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

//             image.setColorTable( sColorTable );

//             return image;
//          }

//          default:
////             qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
//             break;
//       }

//       return QImage();
//    }



QImage ImageFormat::MatGray2QImage(cv::Mat const& src) {
//     cv::Mat temp;
     QImage dest((uchar*) src.data, src.cols, src.rows, src.step, QImage::Format_Indexed8);
     QImage dest2(dest);
     dest2.detach();
     return dest2;
}

cv::Mat ImageFormat::QImage2Mat(QImage const& src) {
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result;
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}
