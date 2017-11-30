import java.util.List;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;

/**
 * Implementation of One-pass error-bounded trajectory simplification algorithm.
 * 
 * @author Lin (linxl@buaa.edu.cn)
 *
 */
public class OPERB extends BasicOPERP{
 
     public GPSLine[] compressByOPERA(){
    	
    	 	List<GPSCompLine> vcl=new ArrayList<GPSCompLine>();
            int iEnd = this.iTrajectory.size()-1;  
            int lineId =0;									
            int    n = 0;                                 	

            double max_p_dis =0;							//max positive distance
            double max_n_dis =0;							//max negative distance
            
       		///////////////////////////////////////////////////////////
       		//The first directed line segment
       		///////////////////////////////////////////////////////////
            GPSPoint loc = (GPSPoint)this.iTrajectory.get(n++);	
            
            GPSCompLine cline = new GPSCompLine();
            cline.index = ++lineId;
            loc.lineId = cline.index;
            
            cline.startPoint = loc;

            cline.points ++;
            vcl.add(cline);
                        
            n = this.getNextValidPoint(n, iEnd, cline);
                   
            max_p_dis =0;
            max_n_dis =0;
            
            ///////////////////////////////////////////////////////////
            //process each point
            ///////////////////////////////////////////////////////////
            while(n<=iEnd){
            	loc = (GPSPoint)this.iTrajectory.get(n++);
            	            	
                double rn = getDistanceOfP2P(loc.latitude, loc.longitude,cline.startPoint.latitude, cline.startPoint.longitude);
                
                double alpha = getAngleOfVector(cline.startPoint.latitude, cline.startPoint.longitude, loc.latitude, loc.longitude);
                
                double theta = getIncludedAngle(cline.angleRF, alpha); 
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                //The distance of the current point to direct line segment.
                double dis = rn * Math.sin(theta);   

                //////////////////////////////////////////////////
                if (theta > Math.PI/2){
                	theta = - Math.PI + theta;
                	dis = - dis;
                }else if (theta < - Math.PI/2){
                	theta = theta + Math.PI;
                	dis = -dis;                	
                }
                ///////////////////////////////////////////////////

                //max_p_dis��max_n_dis
                if (dis < max_n_dis)	max_n_dis = dis;
                if (dis > max_p_dis)	max_p_dis = dis;
              
                int isDeviase = 0;
                
                if (max_p_dis - max_n_dis >= this.threshold){
                	isDeviase = 1;  
                	
                }else if (Math.ceil(rn/this.threshold2-0.5) == Math.ceil(cline.lengthRL/this.threshold2-0.5) 
                		&&	Math.abs(this.getDistanceOfP2RL(cline, loc))>= this.threshold){
                	isDeviase = 1;  
                	
                }
                
                if (//(rn- line.lengthRF < - 1.5 * this.threshold) ||		//some moving object may turn around 		
                		//(Math.abs(theta) >= Math.PI/2) ||					
                		(isDeviase>0)){							
                	
                	double disL = this.getDistanceOfP2RL(cline, loc);
       			 	double disP2P = this.getDistanceOfP2P(loc.latitude, loc.longitude, cline.endPointRL.latitude, cline.endPointRL.longitude);
            		while (n<=iEnd && Math.abs(disL) < this.threshold && disP2P <this.threshold){
            			cline.points ++;
            			loc.lineId = cline.index;
            			loc = (GPSPoint)this.iTrajectory.get(n++);
            			disL = this.getDistanceOfP2RL(cline, loc);
            		}
            		
                	//////////////////////////////////////////////////////////////////
                	//preview valid or Longest point as the start point
                	//In this version, is the last valid point
            		GPSCompLine pline = cline;
                	cline = new GPSCompLine();
                	cline.index = ++lineId;
                	
                	cline.startPoint = pline.endPointRL;

                	vcl.add(cline);
                	
                	n = this.getNextValidPoint(--n, iEnd, cline); 
                    
                    if (n>iEnd) { break;}
                    
                    max_p_dis =0;
                    max_n_dis =0;

                                     
                }else if (rn- cline.lengthRF >= this.threshold2/2.0 ){ //valid point
                	cline.points ++;
                	cline.actPoints ++;
                	
                	loc.lineId = cline.index;
                	
                	{//save R
                    cline.endPointR = loc;
                    cline.lengthR = rn;     
                    cline.angleR = alpha;
                	}
                	
                	{//save RL: the Valid or RLongest
                    cline.endPointRL = loc;    
                 	cline.lengthRL = rn;  
                 	cline.angleRL = alpha;
                	}
                	
                 	//Update RF
                 	double oldm = cline.lengthRF /this.threshold2;
                    double m = rn / this.threshold2;	
                    m = Math.ceil(m - 0.5);
                    cline.lengthRF = m*this.threshold2;
                    
                    //update directed line segment
                    {			
                        double max_theta = 0;
                        
                        for (double j=oldm+1; j<=m; j++){
                        	if (dis >0 ) {max_theta = max_theta + Math.asin(max_p_dis/this.threshold2/j)/j;}
                            else {max_theta = max_theta + Math.asin(max_n_dis/this.threshold2/j)/j;}
                            if (Math.abs(max_theta) > Math.abs(theta)){
                            	max_theta = theta;
                            	break;
                            }
                        }
                        cline.angleRF = cline.angleRF + max_theta;   
                    }
                    
                    cline.angleRF = (cline.angleRF + 2*Math.PI) % (2*Math.PI);						//angle in [0,2pi]
                                        
                }else{//invalid point                                                                                    
                	cline.points ++;
                	loc.lineId = cline.index;
                	
                	{//Save R, only for calculating errors.
             		cline.endPointR = loc;
             		cline.lengthR = rn;
             		cline.angleR = alpha;
                	}
                } 
           }
            
           //translated to array
           GPSLine[]lines = new GPSLine[vcl.size()];
           for(int i=0; i<vcl.size();i++){
        	   GPSCompLine cl = (GPSCompLine)vcl.get(i);
        	   GPSLine line = new GPSLine();
        	   line.startPoint = cl.startPoint;
        	   line.endPoint = cl.endPointRL;
        	   line.angle = cl.angleRL;
        	   line.length = cl.lengthRL;
        	   
        	   line.index = cl.index;
        	   
               lines[i] = line;
           }
           
           return lines;
     }
     
     /**
      * This is a use case of OPERB
      * @param args
     * @throws IOException 
      */
	public static void main(String[] args) throws IOException {
        
	    BufferedReader reader = null;
		
	    //1. new OPERB
	    BasicOPERP tc = new OPERB();
			
	    //2. set the error bound
	    //tc.threshold = 20;
	    tc.threshold = Double.valueOf(args[1]).doubleValue();
	    //3. set the input data file and load the data into memory. 
	    //   Note, one can modify this to an input stream.
            tc.strFileName = args[0];//set the input data file name
            tc.loadTrajectory();
            //4. compress.
            //	Also, the output data structure could be modified to an output stream.
	    long startTime=System.nanoTime();
            GPSLine[] lines =  tc.compress();
	    long endTime=System.nanoTime();
	    System.out.println(args[0]);
	    System.out.println("running time:"+(endTime-startTime)/1000000000.);
	    double R = lines.length;
	    double T = tc.iTrajectory.size();
	    System.out.println("compression rate:"+(T/R));
		
     }
}

