import math
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

colorDict = {}

tmpList = []
ptList = []
for fn in "black.txt white.txt gray.txt".split():
    with open(fn) as f:
        line = f.readline()
        while line:
            ptList = line.split()
            tmpList.append(ptList)
            colorDict[fn.split(".")[0]] = tmpList
            line = f.readline()
        print tmpList
    tmpList = []
    ptList = []
    
GR = []
GB = []

def func(x, a, c):
    return a*x*x + c

basicLayout = 130
sArg_L = 0.2
for e in "black white gray".split():
    for pt in colorDict[e]:
        x = math.log(1.0*int(pt[1])/int(pt[0]),2)
        y = math.log(1.0*int(pt[1])/int(pt[2]),2)
        GR.append(x)
        GB.append(y)

    fig = plt.figure('CTC')
    basicLayout+=1
    sub = fig.add_subplot(basicLayout)
    sub.set_title(e)

    plt.xlim(xmax=1,xmin=-1)
    plt.ylim(ymax=1,ymin=-1)
    plt.annotate("(3,6)", xy = (3, 6), xytext = (4, 5), arrowprops = dict(facecolor = 'black', shrink = 0.1))
    plt.xlabel("x")
    plt.ylabel("y")
    plt.plot(GR,GB,'go')
    import numpy as np
    z1 = np.polyfit(GR, GB, 1)
    z, pcov = curve_fit(func, GR, GB)
    p = np.poly1d(z1)
    
    k = range(-10,10,1)
    t = []
    t.extend(k[i]/10.0 for i in range(0, len(k)))
    v = []
    v.extend(z[0]*i*i + z[1] + sArg_L for i in t)
    v1 = p(t) + sArg_L
    print z1
    print z
    sub.plot(t, v)
    sub.plot(t, v1)
    GR = []
    GB = []
plt.show()
