#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
  
using namespace cv;  
using namespace std;
double* Projection(double a, double b, double x, double y, double m, double n, double c)
{
    double  x1 = 0, y4 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0, gen = 0, gen2 = 0;  
    if (a == 0)
    {
        x1 = x;
        y4 = b;
        x2 = x;
        y2 = m * x2*x2 + n * x2 + c;
        double *rst = new double[2]{x2, y2};
        return rst;
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
            double *rst = new double[2]{x2, y2};
            return rst;
        }
        else if (gen < 0)
        {
            double *rst = new double[2]{x1, y4};
            return rst;
        }
    }
}

int main(int argc, char** argv)  
{  
    char * filename = "test.jpg";
    if(argc > 1)
    {
        filename = argv[1];
    }
    Mat imageSource = imread(filename);  
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
    
    

    //��ԭʼͼ���RGB�����ľ�ֵ  
    double R, G, B;  
    vector<Mat> imageRGB;  

    //RGB��ͨ������  
    split(imageSource, imageRGB);  
                
  
    //����RGB����ͨ�����Ե�ֵ  
    imageRGB[0] = imageRGB[0] * KB;  
    imageRGB[2] = imageRGB[2] * KR;  
  

    //RGB��ͨ��ͼ��ϲ�  
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
    //��任��ͼ��RGB�����ľ�ֵ  
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
    cout << "B,G,R:" << B << " " << G << " " << R << endl;
  
    waitKey();  
    return 0;  
}
