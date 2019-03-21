#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
  
using namespace cv;  
using namespace std;
  
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
    
    double x = logPt[0];
    double y = logPt[1];
    double a = -1.015;
    double b = 0.423;
    if(argc > 2)
    {
        a = atof(argv[2]);
        b = atof(argv[3]);
    }
    

    double x1=(x+(y-b)*a)/((a*a)+1);
    double y1=a*x1+b;
    cout << "rst:" << x << " " << y << " " << x1 << " " << y1 << endl;
    
    double KR=0, KB=0;
    KR = pow(2, x1);
    KB = pow(2, y1);
    
    

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
  
    waitKey();  
    return 0;  
}
