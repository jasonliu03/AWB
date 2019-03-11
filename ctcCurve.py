import math
import matplotlib.pyplot as plt

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

basicLayout = 130
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

    plt.xlim(xmax=0.8,xmin=-0.8)
    plt.ylim(ymax=0.8,ymin=-0.8)
    plt.annotate("(3,6)", xy = (3, 6), xytext = (4, 5), arrowprops = dict(facecolor = 'black', shrink = 0.1))
    plt.xlabel("x")
    plt.ylabel("y")
    plt.plot(GR,GB,'go')
    import numpy as np
    z = np.polyfit(GR, GB, 1)
    p = np.poly1d(z)
    t = range(-1,2)
    v = p(t)
    print z
    sub.plot(t, v)
    GR = []
    GB = []
plt.show()
