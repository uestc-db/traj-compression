
Before you use this code, you should know
=======================
	1. This code was used for the empirical study of the VLDB'2015 paper 
	"Trajectory Simplification: On Minimizing the Direction-based Error".
	1. This code is developed by Cheng Long (clong@cse.ust.hk).
	2. This code is written in C/C++.
	3. This code runs under Unix/Linux.
	4. In case that you encounter any problems when using this code,
	please figure out the problem by yourself 
	(The code in fact is easy to read and you can modify it for your own purpose).

Usage
=======================

Step 1: prepare the configuration file.
<This could be done by editting the "config.txt" file which format is explained in Appendix I.>

Step 2: compile the source code
make

Step 3: Run the code
./DPTS

Step 4: Collect running statistics

The running statistics are stored in "stat.txt" which format is explained in Appendix II.


Appendix

I. The format of Config.txt
=======================

<Data file>
<# of positions in the trajectory>
<The storage budget parameter which is terms of the portion of the original size>
<Algorithm indicator>
<Dataset indicator>

Explanation of some parameters in config.txt
-----------------------

<Data file>:
	The file storing the raw trajectory data.
	(the supported formats are those of Geolife (an example is data/20090330005208.plt) and T-Drive (an example is data/2237.txt), and to support your own data format, you should modify the "data reading" part of function DPTS by yourself)

<Algorithm indicator> 
	= 0: the HWT method (Haar Wavelet Transformaiton)
	= 1: the basic DP algorithm
	= 2: an enhanced DP algorithm (by employing the O(n^2log n) method for computing the approximation errors of all possible segments)
	= 3: the Error-Search algorithm
	= 4: the Span-Search algorithm
	= 5: the Dougolas-Peucker algorithm
	= 6: the Error-Search algorithm with the Dougolas-Peucker algorithm adapted for the error feasibility check

<Dataset indicator>
	= 1: the dataset is in the format of the Geolife dataset;
	= 2: the dataset is in the format of the T-Drive dataset.

(See file config.txt in the folder for example)


II. The format of <stat.txt>
=============================

<the minimized directional error>
<The running time>
<The memory usage (in MB)>



