# traj-compression
The source code was use for 2018 vldb the empirical study of trajectory compression ,"Trajectory Simplification: An Experimental Study andQuality Analysis"
Please cite by :
Dongxiang Zhang, Mengting Ding, Dingyu Yang, Yi Liu, Ju Fan,and Heng Tao Shen. Trajectory Simplification: An Experimental Study and Quality Analysis. PVLDB, 11 (9): 934-946, 2018.DOI: https://doi.org/10.14778/3213880.3213885
# lossless algorithms
  1. TrajStore algorithm    
  2. Trajic algorithm
# Trajectory Simplification in Batch Mode
  1. DP algorithm           
  2. DPhull algorithm 
  3. TD-TR algorithm        
  4. MRPA algorithm 
  5. SP algorithm           
  6. Intersect algorithm
  7. Error-Search algorithm 
  8. Span-Search algorithm
# Trajectory Simplification in Online Mode
  1. Unifrom algorithm        
  2. OPW algorithm
  3. OPW-TR algorithm         
  4. Dead Reckoning algorithm
  5. Threshold algorithm      
  6. STTrace algorithm
  7. SQUISH algorithm         
  8. CDR algorithm
  9. SQUISH-E(λ) algorithm    
  10. SQUISH-E(μ) algorithm 
  11. Persistene algorithm    
  12. BQS algorithm
  13. FBQS algorithm          
  14. Angular algorithm
  15. Interval algorithm      
  16. DOTS algorithm
  17. OPERB algorithm  
# Usage: 
  # 1. lossless alorithms
    a. cd lossless/trajic
    b. make all,run the test and produces both the trajic and stats binaries (the latter of which is used for running experiments).
    c. chmod +x Trajic.sh/TrajStore.sh
    d. ./Trajic.sh or TrajStore.sh

  # 2. Trajectory Simplification in Batch Mode
    a. SP,Intersect in batch/DPTS,make,./DPTS
    b. Error-Search,Span-Search in batch/MinError,make,./DPTS
    c. DP,DPhull,TD-TR,just chmod +x algorithm.sh,./algorithm.sh
    d. MRPA,matlab,demo1

  # 3. Trajectory Simplification in Online Mode
    a. Uniform,OPW,OPW-TR,Dead Reckoning,Threshold,STTrace,SQUISH,BQS,FBQS,Angular,Interval,OPERB,just chmod algorithm.sh,then ./algorithm
    b. DOTS and Persistene,you need run them in QT IDE.
    c. CDR,make,and chmod +x CDR.sh,finally ./CDR.sh.
    d. The source code of SQUISH-E(λ),SQUISH-E(μ) are in SQUISH-E dirname,when the parameter ratio = 1,the compression result of SQUISH-E(μ);when the parameter sed = 0,the result is SQUISH-E(λ) algorithm.

