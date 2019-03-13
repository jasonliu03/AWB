AWB algorithm

1. ./grayWorld test.jpg
2. ./baiDianDetect test.jpg
3. ./YuvBalance test.jpg
4. ./circle dirname/ : based on grayworld; detected based on equal circle; stddev weight; entropy calc;

# color correct
4. python ctcCurve.py : generate ctc curve according to black.txt,white.txt,gray.txt
   ./ctc test.jpg -1.01 0.402  :based on ctc curve;

# dark enhance
5. python logLAvg.py test.jpg: dark enhance based on 《Adaptive Local Tone Mapping Based on Retinex for High Dynamic Range Images  》

