

import java.util.List;

/**
 * Test OPERB.
 * 
 * @author Lin (linxl@buaa.edu.cn)
 *
 */
public class TestOPERB {
	
	public static void printToConsole(TrajectoryCompressor tc, GPSLine[] lines) {

		int l_index = 0; // index to lines
		double ae = 0;
		double de = 0; // Accumulated arithmetic error
		double me = 0; // Accumulated absolute error
		int me_pindex = 0; // index of the max err point
		int me_lindex = 0; //

		for (int n = 0; n < tc.iTrajectory.size(); n++) { 
			
			GPSPoint loc = (GPSPoint) tc.iTrajectory.get(n);
			if (l_index < lines.length - 1) {
				if (loc.lineId >= lines[l_index + 1].index || loc.lineId > lines[l_index].index) {
					l_index++;
				}

			}

			{
				double dis = tc.getDistanceOfP2R(lines[l_index], loc);
				de = de + dis;
				ae = ae + Math.abs(dis);

				if (me < Math.abs(dis)) {
					me = Math.abs(dis);
					me_pindex = n;
					me_lindex = l_index;
				}

				if (tc.threshold < Math.abs(dis)) {
					System.out.println("Point " + n + " breaks up bound : " + dis); // For
					System.out.println("start point: "+lines[l_index].startPoint.index+" end point "+lines[l_index].endPoint.index);										
				}

			}
			
		}

		// System.out.println(" Average Absolute Error : " + ae/trajectory.size());
		// System.out.println(" Average Error : " + de/trajectory.size());
		System.out.println("    Max Error : " + me + "  at point:" + me_pindex);
		// System.out.print(" Isvalid : " + trajectory.get(me_pindex).isValid);
		// System.out.print(" Distance to R' : " +
		// trajectory.get(me_pindex).dis);
	}
	
	
	public static void main(String[] args) {		
 			
 	    	 int npoints =0;
 	    	 int nlines =0;
 	    	 double maxErr =0;
 	    	 
 	    	 for (int zeta =10; zeta<=100; zeta+=10){
 	    		 npoints =0;
 	        	 nlines =0;
 	        	 
 	        	 OPERB tc = new OPERB();
 	        	 tc.threshold = zeta;  // set the threshold of error

 	        	 System.out.println("Threshold : " + tc.threshold + " m.");
 	        	 GPSLine[] lines=null;
 	        	 for (int i=1; i<100; i++){
 	        		 
 	        		 /////////////////////////////set your file name correctly/////////////////////////////////
 	        		 //tc.strFileName = ".\\data\\test.txt";
 	        		tc.strFileName = ".\\data\\geolife\\" + i + ".txt";
 	   				//tc.strFileName = ".\\data\\taxi\\" + i + "";
 	        		 //tc.strFileName = ".\\data\\truck\\" + i + ".txt";
 	        		
 	   				tc.loadTrajectory();
 	   				
 	   				System.out.print("" + i);
 	   				System.out.println(".	Read lines (GPS Points) :" + tc.iTrajectory.size() );
 	   				npoints =npoints + tc.iTrajectory.size();
 	   				
 	   				double stime = System.currentTimeMillis();
 	   				
 	   				lines = tc.compress();      
 	   				
 	   				double etime = System.currentTimeMillis();
 	   				
 	   				System.out.println("Time:" + (etime - stime));
 	   				System.out.println("Compress By OPERB to : (" + lines.length + ") lines.");
 	   				printToConsole(tc, lines);	//to RL
 	   				
 	   				nlines = nlines + lines.length;
 	   				 				
 	        	 }  	

 	        	 System.out.println();
 	        	 System.out.println("Compress Ratio = " + (1.0*nlines) /(1.0*npoints) * 100 + "%.");
 	    	 }	 
	}	

}
