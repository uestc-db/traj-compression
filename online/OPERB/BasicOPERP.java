

import java.util.List;
import java.util.ArrayList;
import java.util.Date;

/**
 * Base class of OPERB.
 * 
 * @author Lin (linxl@buaa.edu.cn)
 *
 */
public abstract class BasicOPERP extends TrajectoryCompressor {

	protected double threshold2 = TrajectoryCompressor.threshold/2.0;

	public GPSLine[] compress() {
		this.threshold2 = TrajectoryCompressor.threshold/2.0;
		return compressByOPERA();		
	}
	
	public abstract GPSLine[] compressByOPERA();

    
     /**
      * get the first valid point. 
      * @param n
      * @param iEnd
      * @param line
      * @return
      */
    int getNextValidPoint(int n, int iEnd, GPSCompLine cline){
         
         while ( n<=iEnd ){ 
        	 GPSPoint loc = (GPSPoint)this.iTrajectory.get(n++);
        	 loc.lineId = cline.index;
        	 
         	 cline.points ++;

 			 //R
         	 cline.endPointR = loc;
             
 			 cline.angleR = getAngleOfVector(cline.startPoint.latitude, cline.startPoint.longitude, loc.latitude, loc.longitude);                        			
         	 double r1 = this.getDistanceOfP2P(loc.latitude, loc.longitude, cline.startPoint.latitude, cline.startPoint.longitude);     
 			 cline.lengthR = r1;
            
         	 if (Math.abs(r1)>= this.threshold) {  
         		 cline.actPoints++;	//valid point
     				
         		 {//RLongest
                 cline.endPointRL = loc;
                 
                 cline.angleRL = cline.angleR;
                 cline.lengthRL = r1;
         		 }	
         		 
         		 {//RF
         		 cline.angleRF = cline.angleR;
                 double m = r1 / this.threshold2;
                 m = Math.ceil(m - 0.5);
         		 cline.lengthRF = m*this.threshold2;
         		 }	
         		 
         		 break;         			
         	 }	
            
         }//end while
         
         return n;
     }
    
    
    double getDistanceOfP2RL(GPSCompLine cline, GPSPoint loc){
    	GPSLine line = new GPSLine();
    	line.startPoint = cline.startPoint;
    	line.endPoint = cline.endPointRL;
    	line.angle = cline.angleRL;
    	line.length = cline.lengthRL;
    	
    	return this.getDistanceOfP2R(line, loc);
    }
       
}
