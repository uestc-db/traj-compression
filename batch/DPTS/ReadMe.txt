
Before you use this code, you should know
=======================
	1. This code was used for the empirical study of the VLDB'2013 paper 
	"Direction-Preserving Trajectory Simplification".
	1. This code is developed by Cheng Long (clong@cse.ust.hk).
	2. This code is written in C/C++.
	3. This code runs under Unix/Linux.
	4. In case that you encounter any problems when using this code,
	please figure out the problem by yourself 
	(The code in fact is easy to read and you can modify it for your own purpose).

Usage
=======================

Step 1: specify the data input information.
<This could be done by editting the "config.txt" file
which format is explained in Appendix I.>

Step 2: compile the source code
make

Step 3: Run the code
./DPTS

Step 4: Collect running statistics 
[you can ignore this step if you don't want to collect this information]

The running statistics are stored in "stat.txt"
which format is explained in Appendix II.




Appendix

I. The format of Config.txt
=======================

<Data file>
<# of the positions in the trajectory>
<Error tolerance parameter>
<Algorithm indicator>
<Dataset tag>

Explanation of the content in config.txt
-----------------------

<Data file>:
	The file that contains the trajectory.
	(the supported formats are those of Geolife and T-Drive,
	and to support your own data format, you should modify the "data reading" part of function DPTS by yourself)

<#positions>:
	The number of positions in the trajectory.

<Error tolerance>:
	The error tolerance of the DPTS problem.

<Algorithm indicator> 
	= 0: the basic DP
	= 1: the enhanced DP
	= 2: the basic SP
	= 3: the SP with the theoretical enhancement only
	= 4: the SP with the practical enhancement only
	= 5: the SP with the both enhancements
	= 6: the Intersect algorithm
	= 7: the Split algorithm
	= 8: the Merge algorithm
	= 9: the Greedy algorithm

<Dataset indicator>
	= 1: the dataset is the Geolife dataset;
	= 2: the dataset is the T-Drive dataset.

(See file config.txt in the folder for example)


II. The format of <stat.txt>
=============================

<The size of the original trajectory>
<The size of the simplified trajectory>
<The compression ratio>

<The running time>
<The memory usage (in MB)>



