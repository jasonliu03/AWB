#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/opencv.hpp>    
#include <stdio.h>
#include <iostream>    
#include <vector>    

using namespace std;  
using namespace cv;  

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

double baidianave(Mat frame,int n)  
{    
    int a[256];  
    for (int i=0;i<256;i++)  
    {  
        a[i]=0;  
    }  
    double sum=0;  
    double ave;  
    for (int i=0;i<n;i++)  
    {  
        int d=frame.at<double>(0,i);  
        a[d]++;  
    }  
    int n0=255;  
    for (int k=255;k>0;k--)  
    {  
        sum+=a[k];  
        if (sum>frame.rows*frame.cols/10)  
        {  
            break;  
        }  
        n0--;  
    }  
    sum=0;  
    for (int i=n0;i<256;i++)  
    {  
        sum+=a[i]*i;  
    }  
    ave=sum/(frame.rows*frame.cols/10);  
    return ave;  
}  

double baidianave(Mat frame)  
{   
    int a[256];  
    for (int i=0;i<256;i++)  
    {  
        a[i]=0;  
    }  
    double sum=0;  
    double ave;  
    for (int i=0;i<frame.rows;i++)  
    {  
        for (int j=0;j<frame.cols;j++)  
        {  
            int d=(int)frame.at<uchar>(i,j);  
            a[d]++;  
        }  
    }  
  
    int n0=255;  
    for (int k=255;k>0;k--)  
    {  
        sum+=a[k];  
        if (sum>frame.rows*frame.cols/10)  
        {  
            break;  
        }  
        n0--;  
    }  
    sum=0;  
    for (int i=n0;i<256;i++)  
    {  
        sum+=a[i]*i;  
    }  
    ave=sum/(frame.rows*frame.cols/10);  
    return ave;  
  
}  
  
  
int main(int argc, char* argv[])  
{  
    char *filename = "test.jpg";
    if(argc > 1)
    {   
        filename = argv[1];
    }   
 
    string img_nm = filename;  
    string op_imagename = "Yuv_" + img_nm;  
    int pos = img_nm.rfind('.');  
    string img_fmt = img_nm.substr(pos+1);  
    Mat frame= imread(img_nm,1);  

    //Lab
    Mat labMap = frame.clone();
    cvtColor(frame, labMap, COLOR_BGR2Lab);    
    showMeanLab(labMap);
    cvNamedWindow("before", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("before", frame);

    // cout<<frame.rows<<"  "<<frame.cols<<endl;  
    // cvShowImage("处理前图像",frame);  
    int heightyiban=frame.rows;  
    int widthyiban=frame.cols;  
    double Mb,Db;//图像分成四部分，每部分Cb的均值和均方差  
    double Mr,Dr;//Cr的均值和均方差      
    Mat imageYCrCb =  Mat::zeros(frame.size(), CV_8UC3);  
    Mat imageCb = Mat::zeros(frame.size(), CV_8UC1);  
    Mat imageCr = Mat::zeros(frame.size(), CV_8UC1);  
    Mat imageY = Mat::zeros(frame.size(), CV_8UC1);  
    cvtColor(frame,imageYCrCb,CV_BGR2YCrCb);   

    std::vector<cv::Mat>ybr(imageYCrCb.channels());  
    split(imageYCrCb,ybr);  

    Mat imageb=Mat::zeros(frame.size(), CV_8UC1);  
    Mat imagec=Mat::zeros(frame.size(), CV_8UC1);  
    ybr[1].copyTo(imageb);  
    ybr[2].copyTo(imagec);  
    Mat  savg,sfangcha;//全局scalar 变量用来放平均值和方差  
    meanStdDev(ybr[2],savg,sfangcha);  
    Mb=savg.at<double>(0);  
    Db=sfangcha.at<double>(0);//求出第一部分cb的均值和均方差  
    meanStdDev(ybr[1],savg,sfangcha);  
    Mr=savg.at<double>(0);  
    // cout<<"Mr:  "<<Mr[0]<<endl;  
    Dr=sfangcha.at<double>(0);;//求出第一部分cr的均值和均方差  
    // cout<<"Dr:  "<<Dr[0]<<endl;  
    double b,c;  
    if (Mb<0)//计算mb+db*sign（mb）  
    {   
        b=Mb+Db*(-1);  
    }  
    else  
        b=Mb+Db;  

      
    if (Mr<0)//计算1.5*mr+dr*sign（mb）；  
    {  
        c=1.5*Mr+Dr*(-1);  
    }  
    else  
        c=1.5*Mr+Dr;  
    double Ymax=baidianave(ybr[0]);  
    //下面是对第一部分进行鐧????的选择  
    Mat Bbaidian=Mat::zeros(1,20000000,CV_64FC1);  
    Mat Gbaidian=Mat::zeros(1,20000000,CV_64FC1);  
    Mat Rbaidian=Mat::zeros(1,20000000,CV_64FC1);  

    //CvScalar s1;  
    int n1=0;  
    // cout<<"b[0]:   "<<b[0]<<"  c[0]:  "<<c[0]<<endl;  
    for (int i=0;i<heightyiban;i++)  
    {  
        for (int j=0;j<widthyiban;j++)  
        {  
            if (((ybr[2].at<uchar>(i,j)-b)<(1.5*Db))&&((ybr[1].at<uchar>(i,j)-c)<(1.5*Dr)))  
            {  
          
                double d1=frame.at<Vec3b>(i,j)[0];  
                Bbaidian.at<double>(0,n1)=d1;  
                  
                double d2=frame.at<Vec3b>(i,j)[1];  
                Gbaidian.at<double>(0,n1)=d2;  
              
                double d3=frame.at<Vec3b>(i,j)[2];  
                Rbaidian.at<double>(0,n1)=d3;  
                n1++;  
            }  
        }  
    }  

  
    double Bave1=baidianave(Bbaidian,n1);  
    double Gave1=baidianave(Gbaidian,n1);  
    double Rave1=baidianave(Rbaidian,n1);  
    // cout<<"Bave1:  "<<Bave1<<"    Gave1:  "<<Gave1<<"      Rave1:   "<<Rave1<<"  Ymax:  "<<Ymax<<endl;  
    double Bgain1=Ymax/(Bave1);  
    double Ggain1=Ymax/(Gave1);  
    double Rgain1=Ymax/(Rave1);  
    // cout<<Bgain1<<"  "<<Ggain1<<"  "<<Rgain1<<endl;   
    for (int i=0;i<heightyiban;i++)  
    {  
        for (int j=0;j<widthyiban;j++)  
        {  
            int tb=Bgain1*frame.at<Vec3b>(i,j)[0];  
            int tg=Ggain1*frame.at<Vec3b>(i,j)[1];  
            int tr=Rgain1*frame.at<Vec3b>(i,j)[2];  
            if (tb>255)  
            {  
                tb=255;  
            }  
            if (tg>255)  
            {  
                tg=255;  
            }  
            if (tr>255)  
            {  
                tr=255;  
            }                 
            frame.at<Vec3b>(i,j)[0]=tb;  
            frame.at<Vec3b>(i,j)[1]=tg;  
            frame.at<Vec3b>(i,j)[2]=tr;  
        }  
    }  

    cvtColor(frame, labMap, COLOR_BGR2Lab);    
    showMeanLab(labMap);
    cvNamedWindow("after", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("after", frame);
    imwrite(op_imagename.c_str(),frame);          
    
    waitKey(0);

    
    frame.release();
    
    imageYCrCb.release();  
    imageCb.release();
    imageCr.release();
    imageY.release();
    imageb.release();
    imagec.release();
    savg.release();
    sfangcha.release();
    Bbaidian.release();  
    Gbaidian.release();  
    Rbaidian.release();  
      
    return 0;  
}  
