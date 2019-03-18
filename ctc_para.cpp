#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
  
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
    imshow("before", imageSource);  
    
    Scalar mean2;
    mean2 = mean(imageSource);

    double avgB=0, avgG=0, avgR=0;
    avgB = mean2[0];
    avgG = mean2[1];
    avgR = mean2[2];
    cout << "avgB,G,R:" << avgB << " " << avgG << " " << avgR << endl;
    Scalar logPt = Scalar(log(avgG/avgR)/log(2), log(avgG/avgB)/log(2));
    
    float x1=0,y1=0,x2=0,y2=0,x3=0,y3=0,gen=0,gen3=0,x4=0,x5=0,k=0,k2=0;
    double gen2=0,gen4=0;
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
    
    if (a==0)
    {
     x1=x;
     y1=b;
    }
    else
    {
     x1=(x+(y-b)*a)/((a*a)+1);
     y1=a*x1+b;
    }
    printf("\nͶӰ����Ϊ��");
    printf("(%f",x1);
    printf(",%f)",y1);
    if (a==0)
    {
     x2=x;
     y2=m*x2*x2+n*x2+c;
     printf("\n��������Ϊ����%f��%f��",x2,y2);
    }
    else
    {
     gen=(n+(1/a))*(n+(1/a))-4*m*(c-(1/a)*x-y);
      if (gen>0)
      {
       gen2=sqrt(gen);
       x2=(-n-(1/a)+gen2)/(2*m);
       y2=(m*x2*x2)+n*x2+c;
       x3=(-n-(1/a)-gen2)/(2*m);
       y3=(m*x3*x3)+n*x3+c;
       if (y2>y3)
       {
        printf("\n��������Ϊ����%f��%f��",x2,y2);
        x0 = x2;
        y0 = y2;
       }
       else if (y3>y2)
       {
        printf("\n��������Ϊ����%f��%f��",x3,y3);
        x0 = x3;
        y0 = y3;
       }
      }
      else if (gen==0)
      {
       x2=(-n-(1/a))/2*m;
       y2=(m*x2*x2)+n*x2+c;
       printf("\nֻ��һ�����㣬��������Ϊ����%f��%f��",x2,y2);
      }
      else if (gen<0)
      {
       printf("\nֱ����������û�н���");
      }
    }
    gen3=(n*n)-(4*m*c);
    if (gen3>=0)
    {
     gen4=sqrt(gen3);
     x4=(-n+gen4)/(2*m);
     x5=(-n-gen4)/(2*m);
     k=(-c)/x5;
     k2=(-c)/x4;
     if (k>=k2)
     {
      k=k2;
     }
     if (c>=0)
     {
      printf("\n��������������Ľ����ϵʽΪ��y=%fx+%f",k,c);
     }
     else if (c<0)
     {
      printf("\n��������������Ľ����ϵʽΪ��y=%fx%f",k,c);
     }
   
    }
    else if (gen3<0)
    {
     printf("\n���������������޽���");
    }


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
