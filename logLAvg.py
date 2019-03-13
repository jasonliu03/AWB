#!coding=utf-8
import sys
import cv2
import numpy as np
from numpy import array

#simple color balance
def simple_color_balance(input_img,out_img,s1,s2):
    h,w = input_img.shape[:2]
    sort_img = input_img.copy()
    one_dim_array = sort_img.flatten()#转化为一维数组
    sort_array = sorted(one_dim_array)#对一维数组按升序排序
 
    per1 = int((h*w)*s1/100)
    minvalue = sort_array[per1]
 
    per2 = int((h*w)*s2/100)
    maxvalue = sort_array[(h*w)-1-per2]
 
    #实施简单白平衡算法
    if(maxvalue<=minvalue):
        for i in range(h):
            for j in range(w):
                out_img[i,j] = maxvalue
    else:
        scale = 255.0/(maxvalue-minvalue)
        for m in range(h):
            for n in range(w):
                if(input_img[m,n] < minvalue):
                    out_img[m,n] = 0
                elif(input_img[m,n] > maxvalue):
                    out_img[m, n] = 255
                else:
                    out_img[m, n] = scale*(input_img[m,n]-minvalue)#映射中间段的图像像素
 
def ALTM(img):
    h,w = img.shape[:2]
    DouImg = array(img,dtype=np.float32)
    Lw = np.zeros(img.shape[:2],dtype = np.float32)
    log_Lw = np.zeros(img.shape[:2],dtype = np.float32)
    Img_out = np.zeros(img.shape,dtype=np.uint8)
    B = img[:,:,0]
    G = img[:,:,1]
    R = img[:,:,2]
    Dou_B = array(B, dtype=np.float32)
    Dou_G = array(G, dtype=np.float32)
    Dou_R = array(R, dtype=np.float32)
 
 
    Lw = 0.299*R+0.587*G+0.114*B
 
    Lwmax = Lw.max()
    log_Lw = np.log(0.001+Lw)
    Lw_sum = log_Lw.sum()
    Lwaver = np.exp(Lw_sum/(h*w))
    Lg = np.log(Lw/Lwaver+1)/np.log(Lwmax/Lwaver+1)
 
    #simple_color_balance(Lg, Lg, 2, 3)
    gain = Lg/Lw
 
    Dou_B = gain * Dou_B
    Dou_G = gain * Dou_G
    Dou_R = gain * Dou_R
 
##对于某些图像来说可能这个效果会好些
    simple_color_balance(Dou_B,Dou_B,2,3)
    simple_color_balance(Dou_G, Dou_G, 2, 3)
    simple_color_balance(Dou_R, Dou_R, 2, 3)
 
 
    DouImg = cv2.merge([Dou_B,Dou_G,Dou_R])
    Img_out=cv2.convertScaleAbs(DouImg)
 
    return Img_out

if __name__ == '__main__':
    filename = sys.argv[1] if len(sys.argv)>1 else "test.jpg"
    srcImg = cv2.imread(filename)
    outImg = ALTM(srcImg)
    cv2.namedWindow("after", 0)
    cv2.resizeWindow("after", 640, 480)
    cv2.imshow("after", outImg)
    cv2.waitKey()
