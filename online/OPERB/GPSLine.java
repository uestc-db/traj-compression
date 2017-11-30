

import java.util.List;

/**
 * Data structure representing a output line segment.
 * @author Lin (linxl@buaa.edu.cn)
 *
 */
public class GPSLine{
	int index = 1;				//line id, from 1
    
    double length=0;			
    double angle=0;          
     
    GPSPoint startPoint = null;
    GPSPoint endPoint = null;
    
    
    ///////////////////////////////////////////////////////
    int startIndex(){
    	return this.startPoint.index;
    }
    double startLatitude(){
    	return this.startPoint.latitude;
    }
    double startLongitude(){
    	return this.startPoint.longitude;
    }
    String startTime(){
    	return this.startPoint.time;
    }
    int endIndex(){
    	return this.endPoint.index;
    }
    double endLatitude(){
    	return this.endPoint.latitude;
    }
    double endLongitude(){
    	return this.endPoint.longitude;
    }
    
    String endTime(){
    	return this.endPoint.time;
    }
    
}