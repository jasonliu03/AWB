AWB algorithm

# awb
1. ./grayWorld test.jpg
2. ./baiDianDetect test.jpg
3. ./YuvBalance test.jpg
4. ./circle dirname/ : based on grayworld; detected based on equal circle; stddev weight; entropy calc;

# color correct
1. python ctcCurve.py 1 -0.1 0.1: generate ctc curve according to black.txt,white.txt,gray.txt; 1:bShowIdx, -0.1,0.1: input pt coordinate;
   ./ctc test.jpg -1.64 0.44: based on ctc line curve;
   ./ctc_para test.jpg -1.64 0.44 -1.61 0.22: based on line and parabola curve;

2. ./colourShiftCheck test.jpg: colour shift check

# dark enhance
2. python logLAvg.py test.jpg: dark enhance based on 《Adaptive Local Tone Mapping Based on Retinex for High Dynamic Range Images  》

