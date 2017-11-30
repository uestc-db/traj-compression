# traj-compression
The source code was use for the empirical study of trajectory compression ,"Trajectory Simplification: An Experimental Study andQuality Analysis"
# lossless algorithms
   1. TrajStore algorithm    2. Trajic algorithm
#Trajectory Simplification in Batch Mode
   1. DP algorithm           2. DPhull algorithm 
   3. TD-TR algorithm        4. MRPA algorithm 
   5. SP algorithm           6. Intersect algorithm
   7. Error-Search algorithm 8. Span-Search algorithm
# Trajectory Simplification in Online Mode
  1.Unifrom algorithm        2.OPW algorithm
  3.OPW-TR algorithm         4.Dead Reckoning algorithm
  5.Threshold algorithm      6.STTrace algorithm
  7.SQUISH algorithm         8.CDR algorithm
  9.SQUISH-E(λ) algorithm    10.SQUISH-E(μ) algorithm 
  11.Persistene algorithm    12.BQS algorithm
  13.FBQS algorithm          14.Angular algorithm
  15.Interval algorithm      16.DOTS algorithm
  17.OPERB algorithm  
#Usage: 
  1. lossless alorithms
     （1）cd lossless/trajic
     （2）make all,run the test and produces both the trajic and stats binaries (the latter of which is used for running experiments).
      (3) chmod +x Trajic.sh/TrajStore.sh
      (4) ./Trajic.sh or TrajStore.sh
  2.Trajectory Simplification in Batch Mode
      (1) SP,Intersect in batch/DPTS,make,./DPTS
      (2) Error-Search,Span-Search in batch/MinError,make,./DPTS
      (3) DP,DPhull,TD-TR,just chmod +x algorithm.sh,./algorithm.sh
      (4) MRPA,matlab,demo1
  3.Trajectory Simplification in Online Mode
     （1）Uniform,OPW,OPW-TR,Dead Reckoning,Threshold,STTrace,SQUISH,BQS,FBQS,Angular,Interval,OPERB,just chmod algorithm.sh,then ./algorithm
     （2）DOTS and Persistene,you need run them in QT IDE.
      (3) CDR,make,and chmod +x CDR.sh,finally ./CDR.sh.
      (4) The source code of SQUISH-E(λ),SQUISH-E(μ) are in SQUISH-E dirname,when the parameter ratio = 1,the compression result of SQUISH-E(μ);when the parameter sed = 0,the result is SQUISH-E(λ) algorithm.

