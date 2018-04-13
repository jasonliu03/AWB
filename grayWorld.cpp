#include <opencv2/opencv.hpp>
#include <iostream>
  
using namespace cv;  
using namespace std;

AB_HIGH = 145;
AB_LOW = 110;
  
void showMeanLab(const Mat labMap)
{
    //mean Lab
    double L, a, b;
    vector<Mat> imageLab;
    split(labMap, imageLab);
    L = mean(imageLab[0])[0];
    a = mean(imageLab[1])[0];
    b = mean(imageLab[2])[0];
    cout << "L,a,b:" << L << " " << a << " " << b << endl;
}

int main(int argc, char** argv)  
{  
    char * filename = "test.jpg";
    if(argc > 1)
    {
        filename = argv[1];
    }
    string op_imagename = filename;
    op_imagename = "gw_" + op_imagename;
    Mat imageSource = imread(filename);  
    cvNamedWindow("before", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("before", imageSource);  
    vector<Mat> imageRGB;  

    //Lab-ori
    Mat labMap_ori = imageSource.clone();
    cvtColor(imageSource, labMap_ori, COLOR_BGR2Lab);    
    cvNamedWindow("Lab_ori", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("Lab_ori", labMap_ori);
    showMeanLab(labMap_ori);
    imwrite(("Lab_" + op_imagename).c_str(), labMap_ori);

    vector<Mat> imageLab;
    split(labMap_ori, imageLab);
    Mat lab_A, lab_B;
    lab_A = imageLab[1];
    lab_B = imageLab[2];
    Mat lab_mask = Mat::zeros(lab_A.rows, lab_A.cols, CV_8U);
        
    
    uchar* pxvec_a=lab_A.ptr<uchar>(0);
    uchar* pxvec_b=lab_B.ptr<uchar>(0);
    uchar* pxvec_m=lab_mask.ptr<uchar>(0);
    //��������Mat�и�������ֵ
    for (int i = 0; i < lab_A.rows; i++)
    {
        pxvec_a = lab_A.ptr<uchar>(i);
        pxvec_b = lab_B.ptr<uchar>(i);
        pxvec_m = lab_mask.ptr<uchar>(i);
        //��ͨ�����ݶ��ڵ�һ���������У�����BGR˳��
        for (int j = 0; j < lab_A.cols*lab_A.channels(); j++)
        {
            if(pxvec_a[j]>AB_HIGH || pxvec_b[j]>AB_HIGH || pxvec_a[j]<AB_LOW || pxvec_b[j]<AB_LOW)
            {
                continue;
            }
            pxvec_m[j] = 255; 
        }
    }
    cvNamedWindow("lab_mask", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("lab_mask", lab_mask);

    //RGB��ͨ������  
    split(imageSource, imageRGB);  
  
    //super paras for BGR
    double SR, SG, SB;  
    SB = 111.245;
    SG = 114.335;
    SR = 116.251;

    //��ԭʼͼ���RGB�����ľ�ֵ  
    double R, G, B;  
    B = mean(imageRGB[0], lab_mask)[0];  
    G = mean(imageRGB[1], lab_mask)[0];  
    R = mean(imageRGB[2], lab_mask)[0];  
    //B = mean(imageRGB[0])[0];  
    //G = mean(imageRGB[1])[0];  
    //R = mean(imageRGB[2])[0];  
  
    //super para for value
    //double KValue = (SB+SG+SR)/(B+G+R);
    double KValue = 1;
    double FIX_KValue = 1.0;

    //��Ҫ������RGB����������  
    double KR, KG, KB;  
    //KB = FIX_KValue * KValue * (R + G + B) * SB / ((SB + SG + SR) * B);  
    //KG = FIX_KValue * KValue * (R + G + B) * SG / ((SB + SG + SR) * G);  
    //KR = FIX_KValue * KValue * (R + G + B) * SR / ((SB + SG + SR) * R);  
    KB = FIX_KValue * KValue * (R + G + B) / (3 * B);  
    KG = FIX_KValue * KValue * (R + G + B) / (3 * G);  
    KR = FIX_KValue * KValue * (R + G + B) / (3 * R);  
  
    //����RGB����ͨ�����Ե�ֵ  
    imageRGB[0] = imageRGB[0] * KB;  
    imageRGB[1] = imageRGB[1] * KG;  
    imageRGB[2] = imageRGB[2] * KR;  
  
    //RGB��ͨ��ͼ��ϲ�  
    merge(imageRGB, imageSource);  
    cvNamedWindow("after", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("after", imageSource);  
    imwrite(op_imagename.c_str(), imageSource);
    cout << "before B,G,R:" << B << " " << G << " " << R << endl;
    cout << "KB,KG,kR:" << KB << " " << KG << " " << KR << endl;

    //��任��ͼ��RGB�����ľ�ֵ  
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
    cout << "after B,G,R:" << B << " " << G << " " << R << endl;


    //Lab
    Mat labMap = imageSource.clone();
    cvtColor(imageSource, labMap, COLOR_BGR2Lab);    
    showMeanLab(labMap);
    
    
    imageSource.release();
    labMap_ori.release();
    labMap.release();
  
    waitKey();  
    return 0;  
}
