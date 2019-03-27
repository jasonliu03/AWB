#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <dirent.h>
  
using namespace cv;  
using namespace std;
//double* Projection(double a, double b, double x, double y, double m, double n, double c)
//{
//    double  x1 = 0, y4 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0, gen = 0, gen2 = 0;  
//    if (a == 0)
//    {
//        x1 = x;
//        y4 = b;
//        x2 = x;
//        y2 = m * x2*x2 + n * x2 + c;
//        double *rst = new double[2]{x2, y2};
//        return rst;
//    }
//    else
//    {
//        x1 = (x + (y - b)*a) / ((a*a) + 1);
//        y4 = a * x1 + b;
//        gen = (n + (1 / a))*(n + (1 / a)) - 4 * m*(c - (1 / a)*x - y);
//        if (gen >= 0)
//        {
//            gen2 = sqrt(gen);
//            x2 = (-n - (1 / a) + gen2) / (2 * m);
//            y2 = (m*x2*x2) + n * x2 + c;
//            x3 = (-n - (1 / a) - gen2) / (2 * m);
//            y3 = (m*x3*x3) + n * x3 + c;
//            if (y3 > y2)
//            {
//                y2 = y3;
//                x2 = x3;
//            }
//            double *rst = new double[2]{x2, y2};
//            return rst;
//        }
//        else if (gen < 0)
//        {
//            double *rst = new double[2]{x1, y4};
//            return rst;
//        }
//    }
//}

double* Projection(double a, double b, double x, double y, double m, double n, double c)
{
    double  x1 = 0, y4 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0, gen = 0, gen2 = 0,x5=0,y5=0,x4=0,y6=0,gen3=0,gen4=0;
    gen3 = (n - a)*(n - a) - 4 * m*(c - b);//*先计算直线与抛物线的交点
    if (gen3 >= 0)
    {
        gen4 = sqrt(gen3);
        x5 = (a - n + gen4) / (2 * m);
        y5 = a * x5 + b;
        x4 = (a - n - gen4) / (2 * m);
        y6 = a * x4 + b;
        if (x5 >= x4)//*取最左边的点为X5
        {
            x5 = x4;
            y5 = y6;
        }
    }
    else if (gen3 < 0)//*如果没有交点 取（0，0）
    {
        x5 = 0;
        y5 = 0;
    }
    if (a == 0) //*此时投影是一条垂直X轴的直线 交点只有一个 
    {
        x1 = x;
        y4 = b;
        x2 = x;
        y2 = m * x2*x2 + n * x2 + c;
        if (x2 < x5) //*交点在X5左边时，取投影点
        {
            double *rst = new double[2]{ x1, y4 };
            return rst;
        }
        else //*交点在X5右边时，取交点
        {
            double *rst = new double[2]{ x2, y2 };
            return rst;
        }
    }
    else
    {
        x1 = (x + (y - b)*a) / ((a*a) + 1);
        y4 = a * x1 + b;
        gen = (n + (1 / a))*(n + (1 / a)) - 4 * m*(c - (1 / a)*x - y);
        if (gen >= 0)
        {
            gen2 = sqrt(gen);
            x2 = (-n - (1 / a) + gen2) / (2 * m);
            y2 = (m*x2*x2) + n * x2 + c;
            x3 = (-n - (1 / a) - gen2) / (2 * m);
            y3 = (m*x3*x3) + n * x3 + c;
            if (y3 > y2)
            {
                y2 = y3;
                x2 = x3;
            }
            if (x2 < x5) //*交点在X3左边时，取投影点
            {
                double *rst = new double[2]{ x1, y4 };
                return rst;
            }
            else //*交点在X3右边时，取交点
            {
                double *rst = new double[2]{ x2, y2 };
                return rst;
            }
        }
        else if (gen < 0)//*此时抛物线与垂线无交点，取投影点
        {
            double *rst = new double[2]{ x1, y4 };
            return rst;
        }
    }
}

int main(int argc, char** argv)  
{  

    char * dirname = "./";
    if(argc > 1)
    {
        dirname = argv[1];
    }

    struct dirent *dirp;
    
    DIR* dir = opendir(dirname);
    
    while ((dirp = readdir(dir)) != NULL) {
        if (dirp->d_type == DT_REG) {
            const char *filename = dirp->d_name;
            stringstream ss;
            ss << dirname << dirp->d_name;
            string sName = ss.str();
            const char *pathName = sName.c_str();
            cout << pathName << endl;
            Mat imageSource = imread(pathName);  
            cvNamedWindow("before", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
            cvResizeWindow("before", 1024, 768);
            imshow("before", imageSource);  
            
            Scalar mean2;
            mean2 = mean(imageSource);

            double avgB=0, avgG=0, avgR=0;
            avgB = mean2[0];
            avgG = mean2[1];
            avgR = mean2[2];
            cout << "avgB,G,R:" << avgB << " " << avgG << " " << avgR << endl;
            Scalar logPt = Scalar(log(avgG/avgR)/log(2), log(avgG/avgB)/log(2));
            
            float a = -1;
            float b = 0.5;
            float x = -1;
            float y = -1;
            float m = -1;
            float n = 0;
            float c = 1;
            float x0 = 0;
            float y0 = 0;
            
            x = logPt[0];
            y = logPt[1];
            if(argc > 2)
            {
                a = atof(argv[2]);
                b = atof(argv[3]);
                m = atof(argv[4]);
                c = atof(argv[5]);
            }
            
            double *tmp = NULL;
            tmp = Projection(a, b, x, y, m, n, c);
            x0 = tmp[0];
            y0 = tmp[1];
            delete(tmp);

            cout << "rst:" << x << " " << y << " " << x0 << " " << y0 << endl;
            
            double KR=0, KB=0;
            KR = pow(2, x0);
            KB = pow(2, y0);
            
            

            //求原始图像的RGB分量的均值  
            double R, G, B;  
            vector<Mat> imageRGB;  

            //RGB三通道分离  
            split(imageSource, imageRGB);  
                        
  
            //调整RGB三个通道各自的值  
            imageRGB[0] = imageRGB[0] * KB;  
            imageRGB[2] = imageRGB[2] * KR;  
  

            //RGB三通道图像合并  
            merge(imageRGB, imageSource);  
            cvNamedWindow("after", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
            cvResizeWindow("after", 1024, 768);
            cvMoveWindow("after", 1024, 0);
            imshow("after", imageSource);  
            string sOutName(filename);
            string pre("ctc_");
            pre.append(sOutName);
            imwrite(pre.c_str(), imageSource);

            cout << "KB,KG,kR:" << KB << " " << "1" << " " << KR << endl;
            //求变换后图像RGB分量的均值  
            B = mean(imageRGB[0])[0];  
            G = mean(imageRGB[1])[0];  
            R = mean(imageRGB[2])[0];  
            cout << "B,G,R:" << B << " " << G << " " << R << endl;
  
            //waitKey();  
        } else if (dirp->d_type == DT_DIR) {
            // 文件夹
        }
    }
    
    closedir(dir);
    return 0;  
}
