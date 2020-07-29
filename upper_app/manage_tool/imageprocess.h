#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include "typedef.h"

#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#if USE_OPENCV == 1
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif

class COpencvImgProcess
{
public:
    COpencvImgProcess(){};
    ~COpencvImgProcess(){};

    //根据地址加载图像
    bool load_image(QLabel *label, QString Path);
    void load_image(QLabel *label, const QImage &image);

    void set_file_extra(const QString &str)
    {
        file_extra = str;
    }

    const QString get_file_extra()
    {
        return file_extra;
    }

#if USE_OPENCV == 1
    void load_image(QLabel *label, const cv::Mat &mat);

    //图像均值滤波
    bool blur_image(QLabel *label, QString Path);

    //图像灰度转换
    bool gray_image(QLabel *label, QString Path);

    //图像腐蚀 -- 高亮部分缩小
    bool erode_image(QLabel *label, QString Path);

    //图像膨胀 -- 高亮部分扩大
    bool dilate_image(QLabel *label, QString Path);

    //边缘检测
    bool canny_image(QLabel *label, QString Path);

    //线性扩展
    bool line_scale_image(QLabel *label, QString Path);

    //非线性扩展
    bool noline_scale_image(QLabel *label, QString Path);

    //直方图均衡
    bool equalizeHist_image(QLabel *label, QString Path);

    //仿射变换
    bool warpaffine_image(QLabel *label, QString Path);

    //霍夫线变换
    bool houghlines_image(QLabel *label, QString Path);

    //直方图
    bool hist_image(QLabel *label, QString Path);
#else
    //图像均值滤波
    void blur_image(QLabel *label, QString Path){
        return load_image(label, Path);
    };

    //图像腐蚀 -- 高亮部分缩小
    void erode_image(QLabel *label, QString Path){
        return load_image(label, Path);
    };

    //图像膨胀 -- 高亮部分扩大
    void dilate_image(QLabel *label, QString Path){
       return load_image(label, Path);
    };

    //边缘检测
    void canny_image(QLabel *label, QString Path){
        return load_image(label, Path);
    };


    //线性扩展
    bool line_scale_image(QLabel *label, QString Path){
        return load_image(label, Path);
    };

    //非线性扩展
    bool noline_scale_image(QLabel *label, QString Path){
        return load_image(label, Path);
    };

    //直方图均衡
    bool equalizeHist_image(QLabel *label, QString Path){
        return load_image(label, Path);
    };

    //仿射变换
    bool warpaffine_image(QLabel *label, QString Path){
        return load_image(label, Path);
    };

    //霍夫线变换
    bool houghlines_image(QLabel *label, QString Path){
        return load_image(label, Path);
    }

    //直方图
    bool hist_image(QLabel *label, QString Path)｛
        return load_image(label, Path);
    ｝
#endif

private:
    QString file_extra{""};

#if USE_OPENCV == 1
    //将Opencv内部图像转变为QImage对象
    QImage cvMattoQImage(const cv::Mat& mat);
    cv::Mat QImagetocvMat(const QImage &image);			// QImage 改成 Mat
#endif
};


#endif // IMAGEPROCESS_H
