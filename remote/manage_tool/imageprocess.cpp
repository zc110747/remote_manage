
#include <QVector>
#include "imageprocess.h"

bool COpencvImgProcess::load_image(QLabel *label, QString Path)
{
#if USE_OPENCV == 1
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    if(!ImgMat.data)
    {
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }
    load_image(label, ImgMat);
    return true;
#else
    QImage image;
    if(!(image.load(Path))){
        qDebug()<<"Image load failed, Path:"<<Path;
        return false;
    }
    qDebug()<<"Image load ok";
    label->clear();
    label->setPixmap(QPixmap::fromImage(image));
    label->setScaledContents(true);
    return true;
#endif
}

#if USE_OPENCV == 1
void COpencvImgProcess::load_image(QLabel *label, const cv::Mat &mat)
{
    QImage image = cvMattoQImage(mat);
    qDebug()<<"Image load ok";
    load_image(label, image);
}

void COpencvImgProcess::load_image(QLabel *label, const QImage &image)
{
    qDebug()<<"Image load ok";
    label->clear();
    label->setPixmap(QPixmap::fromImage(image));
    label->setScaledContents(true);
}

QImage COpencvImgProcess::cvMattoQImage(const cv::Mat& mat)
{
    switch(mat.type())
    {
        case CV_8UC1:
        {
            static QVector<QRgb> sColorVector;
            if(sColorVector.isEmpty())
            {
                for(int i=0; i<256; i++)
                {
                    sColorVector.push_back(qRgb(i, i, i));
                }
            }
            QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
            image.setColorTable(sColorVector);
            return image;
        }
        break;

        case CV_8UC3:
        {
            QImage image(mat.data, mat.cols, mat.rows, (int)mat.step, QImage::Format_RGB888);
            return image.rgbSwapped();
        }

        case CV_8UC4:
        {
            QImage image(mat.data, mat.cols, mat.rows, (int)mat.step, QImage::Format_ARGB32);
            return image;
        }
        break;
        default:
            qDebug("Image format is not supported: depth=%d and %d channels\n",
                   mat.depth(), mat.channels());
        break;
    }

    return QImage();
}

cv::Mat QImagetocvMat(const QImage &image)
{
    cv::Mat ImgMat;

    switch(image.format())
    {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            ImgMat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
        {
            ImgMat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(ImgMat, ImgMat, cv::COLOR_BGR2RGB);
            break;
        }
        case QImage::Format_Indexed8:
            ImgMat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        default:
            return cv::Mat();
    }
    return ImgMat;
}

bool COpencvImgProcess::blur_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat ImgMatOut;

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::blur(ImgMat, ImgMatOut, cv::Size(7, 7));
    //cv::GaussianBlur(ImgMat, ImgMatOut, cv::Size(7, 7), 0, 0);
    //cv::medianBlur(ImgMat, ImgMatOut, 3);
    //cv::bilateralFilter(ImgMat, ImgMatOut, 4, 4*2, 4/2);

    load_image(label, ImgMatOut);
    return true;
}

bool COpencvImgProcess::erode_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat ImgMatOut;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::erode(ImgMat, ImgMatOut, element);
    load_image(label, ImgMatOut);

    return true;
}

bool COpencvImgProcess::dilate_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat ImgMatOut;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(7, 7));

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::dilate(ImgMat, ImgMatOut, element);
    load_image(label, ImgMatOut);

    return true;
}

bool COpencvImgProcess::canny_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat GrayImgMat, BlurImgMat, CannyImgMat;

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::cvtColor(ImgMat, GrayImgMat, cv::COLOR_BGR2GRAY);
    cv::blur(GrayImgMat, BlurImgMat, cv::Size(7, 7));
    cv::Canny(BlurImgMat, CannyImgMat, 30, 60, 3, false);

    load_image(label, CannyImgMat);

    return true;
}

bool COpencvImgProcess::gray_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat GrayImgMat, BlurImgMat;

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::cvtColor(ImgMat, GrayImgMat, cv::COLOR_BGR2GRAY);
    cv::blur(GrayImgMat, BlurImgMat, cv::Size(7, 7));

    load_image(label, BlurImgMat);

    return true;
}


bool COpencvImgProcess::line_scale_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat GrayImgMat, lineScaleImageMat;
    int tmp = 0;

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::cvtColor(ImgMat, GrayImgMat, cv::COLOR_BGR2GRAY);
    lineScaleImageMat = GrayImgMat;
    for (int y = 0; y < GrayImgMat.rows; y++)
    {
        for (int x = 0; x < GrayImgMat.cols; x++)
        {
            tmp = ((GrayImgMat.data + GrayImgMat.cols*y))[x];
            unsigned char* tmp_ptr = &((lineScaleImageMat.data + lineScaleImageMat.cols*y)[x]);
            if (tmp < 64)
            {
                *tmp_ptr = tmp / 2;
            }
            else if (tmp < 192)
            {
                *tmp_ptr = tmp + (tmp / 2);
            }
            else
            {
               *tmp_ptr = tmp / 2;
            }
        }
    }

    load_image(label, lineScaleImageMat);
    return true;
}


bool COpencvImgProcess::noline_scale_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat GrayImgMat, nolineScaleImageMat;
    int tmp = 0;

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::cvtColor(ImgMat, GrayImgMat, cv::COLOR_BGR2GRAY);
    nolineScaleImageMat = cv::Mat::zeros(GrayImgMat.rows, GrayImgMat.cols, GrayImgMat.type());

    for (int y = 0; y < GrayImgMat.rows; y++)
    {
        for (int x = 0; x < GrayImgMat.cols; x++)
        {
            tmp = ((GrayImgMat.data + GrayImgMat.cols*y))[x];
            unsigned char* tmp_ptr = &((nolineScaleImageMat.data + nolineScaleImageMat.cols*y)[x]);
            *tmp_ptr = (unsigned char)(255.0 / log10(256.0)) * log10((float)(1 + tmp));
        }
    }

    load_image(label, nolineScaleImageMat);
    return true;
}

bool COpencvImgProcess::equalizeHist_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat GrayImgMat, EqualizeHistImageMat;

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::cvtColor(ImgMat, GrayImgMat, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(GrayImgMat, EqualizeHistImageMat);

    load_image(label, EqualizeHistImageMat);
    return true;
}

bool COpencvImgProcess::warpaffine_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat WrapImgMat, WrapRotateImgMat;
    cv::Point2f srcTri[3];
    cv::Point2f dstTri[3];

    cv::Mat rot_mat( 2, 3, CV_32FC1 );
    cv::Mat warp_mat( 2, 3, CV_32FC1 );

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    WrapImgMat = cv::Mat::zeros(ImgMat.rows, ImgMat.cols, ImgMat.type());

    srcTri[0] = cv::Point2f( 0,0 );
    srcTri[1] = cv::Point2f( ImgMat.cols - 1, 0 );
    srcTri[2] = cv::Point2f( 0, ImgMat.rows - 1 );

    dstTri[0] = cv::Point2f( ImgMat.cols*0.0, ImgMat.rows*0.33 );
    dstTri[1] = cv::Point2f( ImgMat.cols*0.85, ImgMat.rows*0.25 );
    dstTri[2] = cv::Point2f( ImgMat.cols*0.15, ImgMat.rows*0.7 );
    warp_mat = cv::getAffineTransform( srcTri, dstTri );
    cv::warpAffine( ImgMat,  WrapImgMat, warp_mat, WrapImgMat.size() );

    cv::Point center = cv::Point( WrapImgMat.cols/2, WrapImgMat.rows/2 );
    double angle = -50.0;
    double scale = 0.6;

    rot_mat = getRotationMatrix2D( center, angle, scale );

    warpAffine(WrapImgMat, WrapRotateImgMat, rot_mat, WrapImgMat.size());

    load_image(label, WrapRotateImgMat);

    return true;
}

bool COpencvImgProcess::houghlines_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cv::Mat HoughlinesImgMat, LineImgMat;
    cv::Canny(ImgMat, HoughlinesImgMat, 50, 200, 3);
    cv::cvtColor(HoughlinesImgMat, LineImgMat, cv::COLOR_GRAY2BGR);

    std::vector<cv::Vec4i> lines;
    HoughLinesP(HoughlinesImgMat, lines, 1, CV_PI/180, 50, 50, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
      cv::Vec4i l = lines[i];
      cv::line( LineImgMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, 16);
    }

    load_image(label, LineImgMat);
    return true;
}

bool COpencvImgProcess::hist_image(QLabel *label, QString Path)
{
    cv::Mat ImgMat = cv::imread(Path.toStdString(), 1);
    cv::Mat HsvImgMat, HueImgMat;

    if(!ImgMat.data){
        qDebug()<<"img load failed, Path:"<<Path;
        return false;
    }

    cvtColor(ImgMat, HsvImgMat, cv::COLOR_BGR2HSV);

    HueImgMat.create(HsvImgMat.size(), HsvImgMat.depth());
    int ch[] = {0, 0};
    cv::mixChannels(&HsvImgMat, 1, &HueImgMat, 1, ch, 1);

    cv::MatND hist;

    int histSize = 256;
    float hue_range[] = { 0, 180 };
    const float* ranges = { hue_range };

    cv::calcHist(&HueImgMat, 1, 0, cv::Mat(), hist, 1, &histSize, &ranges, true, false );
    cv::normalize( hist, hist, 0, 255, cv::NORM_MINMAX, -1, cv::Mat() );

    cv::MatND backproj;
    cv::calcBackProject(&HueImgMat, 1, 0, hist, backproj, &ranges, 1, true );

    int w = 400; int h = 400;
    int bin_w = cvRound( (double) w / histSize );
    cv::Mat histImgMat = cv::Mat::zeros( w, h, CV_8UC3 );

    for(int i=0; i<histSize; i++)
    {
        rectangle( histImgMat, cv::Point( i*bin_w, h ),
                   cv::Point( (i+1)*bin_w, h - cvRound( hist.at<float>(i)*h/255.0 ) ),
                   cv::Scalar( 0, 0, 255 ), -1 );
    }

    load_image(label, histImgMat);

    return true;
}
#endif
