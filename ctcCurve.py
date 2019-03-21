import sys
import math
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import numpy as np


if __name__ == '__main__':
    srcPtX = []
    srcPtY = []
    if len(sys.argv) == 3:
        srcPtX.append(float(sys.argv[1]))
        srcPtY.append(float(sys.argv[2]))
    else:
        print("please input point axis")
        exit(-1)
    colorDict = {}
    
    tmpList = []
    ptList = []
    allList = []
    for fn in "black.txt white.txt gray.txt".split():
        with open(fn) as f:
            line = f.readline()
            while line:
                ptList = line.split()
                tmpList.append(ptList)
                colorDict[fn.split(".")[0]] = tmpList
                line = f.readline()
        allList.extend(tmpList)
        tmpList = []
        ptList = []
    colorDict['all'] = allList
    print colorDict['all']
        
    GR = []
    GB = []
    
    def func(x, a, c):
        return a*x*x + c
    
    basicLayout = 230
    sArg_L = 0
    for e in "black white gray all".split():
        for pt in colorDict[e]:
            x = math.log(1.0*int(pt[1])/int(pt[0]),2)
            y = math.log(1.0*int(pt[1])/int(pt[2]),2)
            GR.append(x)
            GB.append(y)
    
        fig = plt.figure('Color Curve')
        basicLayout+=1
        sub = fig.add_subplot(basicLayout)
        sub.set_title(e)
        sub.set_xlim([-1, 1])
        sub.set_ylim([-1, 1])
        sub.grid(True)
        sub.set_xlabel("x")
        sub.set_ylabel("y")
        sub.plot(GR,GB,'go')
        sub.plot(srcPtX, srcPtY, 'ro')
        bbox = dict(boxstyle="round", fc="0.8")
        arrowprops = dict(
            arrowstyle = "->",
            connectionstyle = "angle,angleA=0,angleB=90,rad=10")
        
        offset = 75
        sub.annotate('(%.3f, %.3f)'%(srcPtX[0], srcPtY[0]),
                    (srcPtX[0], srcPtY[0]), xytext=(-1.5*offset, 1.5*offset), textcoords='offset points',
                    bbox=bbox, arrowprops=arrowprops)
        z1 = np.polyfit(GR, GB, 1)
        z, pcov = curve_fit(func, GR, GB)
        p = np.poly1d(z1)
        
        k = np.linspace(-10,11)
        t = []
        t.extend(k[i]/10.0 for i in range(0, len(k)))
        v = []
        v.extend(z[0]*i*i + z[1] + sArg_L for i in t)
        v1 = p(t) + sArg_L
        print z1
        print z
        a = z1[0]
        b = z1[1]
        x = srcPtX[0]
        y = srcPtY[0]
        x1=(x+(y-b)*a)/((a*a)+1);
        y1=a*x1+b;
        sub.plot(x1,y1,'bo')
        sub.plot([x,x1],[y,y1],'b')
        sub.plot(t, v)
        sub.plot(t, v1)
        GR = []
        GB = []
    plt.show()
