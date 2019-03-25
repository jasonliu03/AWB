import sys
import math
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import numpy as np

def Projection( a,  b,  x,  y,  m,  n,  c):
    x1 = 0; y4 = 0; x2 = 0; y2 = 0; x3 = 0; y3 = 0; gen = 0; gen2 = 0;
    if (a == 0):
        x1 = x;
        y4 = b;
        x2 = x;
        y2 = m * x2*x2 + n * x2 + c;
        return [x2,y2];
    else:
        x1 = (x + (y - b)*a) / ((a*a) + 1);
        y4 = a * x1 + b;
        gen = (n + (1 / a))*(n + (1 / a)) - 4 * m*(c - (1 / a)*x - y);
        if (gen >= 0):
            gen2 = math.sqrt(gen);
            x2 = (-n - (1 / a) + gen2) / (2 * m);
            y2 = (m*x2*x2) + n * x2 + c;
            x3 = (-n - (1 / a) - gen2) / (2 * m);
            y3 = (m*x3*x3) + n * x3 + c;
            if (y3 > y2):
                y2 = y3;
                x2 = x3;
            return [x2, y2];
        elif (gen < 0):
            return [x1, y4];

if __name__ == '__main__':
    srcPtX = []
    srcPtY = []
    bShowIdx = False
    if len(sys.argv) >= 3:
        bShowIdx = False if int(sys.argv[1]) == 0 else True
        srcPtX.append(float(sys.argv[2]))
        srcPtY.append(float(sys.argv[3]))
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
                tmpList.append(ptList[-3:])
                colorDict[fn.split(".")[0]] = tmpList
                line = f.readline()
        allList.extend(tmpList)
        tmpList = []
        ptList = []
    colorDict['all'] = allList
        
    GR = []
    GB = []
    
    def func(x, a, c):
        return a*x*x + c
    
    basicLayout = 230
    sArg_L = 0
    sArg_P = 0
    if len(sys.argv) == 6:
        sArg_L = float(sys.argv[4])
        sArg_P = float(sys.argv[5])
    else:
        print("please input point axis")
    for e in "black white gray all".split():
        index = 0
        for pt in colorDict[e]:
            x = math.log(1.0*int(pt[1])/int(pt[0]),2)
            y = math.log(1.0*int(pt[1])/int(pt[2]),2)
            GR.append(x)
            GB.append(y)
            index += 1
    
        fig = plt.figure('Color Curve')
        basicLayout+=1
        sub = fig.add_subplot(basicLayout)
        sub.set_title(e)
        sub.set_xlim([-0.75, 0.75])
        sub.set_ylim([-0.75, 0.75])
        sub.grid(True)
        sub.set_xlabel("x")
        sub.set_ylabel("y")
        sub.plot(GR,GB,'go')
        if bShowIdx: 
            for i in range(len(GR)):
                sub.annotate(str(i+1), xy = (GR[i], GB[i]), xytext = (GR[i]+0.01, GB[i]+0.001))
        sub.plot(srcPtX, srcPtY, 'yo')
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
        z1[1] = z1[1] + sArg_L
        z[1] = z[1] + sArg_P
        p = np.poly1d(z1)
        
        k = np.linspace(-10,11)
        t = []
        t.extend(k[i]/10.0 for i in range(0, len(k)))
        v1 = p(t)
        v = []
        v.extend(z[0]*i*i + z[1] for i in t)
        print z1
        print z

        a = z1[0]
        b = z1[1]
        x = srcPtX[0]
        y = srcPtY[0]
        x1=(x+(y-b)*a)/((a*a)+1);
        y1=a*x1+b;
        print("line:%0.3f, %0.3f"%(x1, y1))

        m = z[0]
        n = 0
        c = z[1]
        x2, y2 = Projection(a, b, x, y, m, n, c)
        print("para:%0.3f, %0.3f"%(x2, y2))
        sub.plot(x1,y1,'bo')
        sub.plot(x2,y2,'ro')
        sub.plot([x,x1,x2],[y,y1,y2],'b')
        sub.plot(t, v)
        sub.plot(t, v1)
        sub.annotate('(%.3f, %.3f)'%(x1, y1),
                    (x1, y1), xytext=(-1.3*offset, 1.3*offset), textcoords='offset points',
                    arrowprops=arrowprops)
        sub.annotate('(%.3f, %.3f)'%(x2, y2),
                    (x2, y2), xytext=(-1.2*offset, 1.2*offset), textcoords='offset points',
                    arrowprops=arrowprops)

        GR = []
        GB = []
    plt.show()
