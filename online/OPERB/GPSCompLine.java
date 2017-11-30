


import java.util.List;

/**
 * Data structure representing directed line segments.
 * 
 * @author Lin (linxl@buaa.edu.cn)
 *
 */
public class GPSCompLine{
	int index = 1;					//line id, from 1.
    int points = 0;				//the number of point the line representing, including inactive points.
    int actPoints =0;			//the number of point the line representing

    GPSPoint startPoint =null;
        
    //////////////////////////////////////////////////////////
    //To the last data point
    GPSPoint endPointR =null;
    double lengthR=0;			
    double angleR=0;          	
    
    //////////////////////////////////////////////////////////
    //To the last active point
    GPSPoint endPointRL =null;
    double lengthRL=0;		
    double angleRL =0;		 
    
    //////////////////////////////////////////////////////////
    //To the last point of R'
    GPSPoint endPointRF =null;
    double lengthRF=0;			
    double angleRF =0;			 
    
    ///////////////////////////////////////////////////////////
    double max_p_dis =0;		//The max positive distance to R'
    double max_n_dis =0;		//The max negative distance to R'
    
}