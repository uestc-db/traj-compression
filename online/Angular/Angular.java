
import java.io.*;
import java.util.*;

import javax.jws.Oneway;



public class Angular {

public static double PI = 3.1415926;
public static double error= PI/4;
public static double error_t=error/2;
public static String str = "20081106231922";
public static String dest="e"+String.valueOf(error)+"TimeRate.txt";
public static String save_path ;
public static ArrayList<Long> time = new ArrayList();
public static ArrayList<Double> rate = new ArrayList();
public static long averagetime;
public static double averagerate;
public static ArrayList<Point> R = new ArrayList();
static class Point{
	double x;
	double y;
	String time;
	int num;
}
public static double Standardization(double a){		//将角度都约束在[0,2*PI)之间
	if (a<0)
		a=a+2*PI;
	if (a>=2*PI)
		a=a-2*PI;
	return a;
}
public static double AngularDeviation(ArrayList<Point> P, int i){	//pi点的偏转角（有正负之分）
	double d1 = Math.atan2(P.get(i).y-P.get(i-1).y,P.get(i).x-P.get(i-1).x);//在[0，2PI}之间
	double d2 = Math.atan2(P.get(i+1).y-P.get(i).y,P.get(i+1).x-P.get(i).x);
	double deviation = Math.min(Math.abs(d2-d1),2*PI-Math.abs(d2-d1));
	if(d1>=0){	//[0,PI]
		if(d2>=d1+PI||d2<=d1){
			return -deviation;
		}		
	}
	else{	//(-PI,0)
		if(d2>d1-PI&&d2<d1){
			return -deviation;
		}
	}
	return deviation;
}


/*public static void AngularRunningTime(){
	
	BufferedReader reader = null;   
    
	String fileDir = "E:/javaworkspace/Simplification/tempdata/geolifedata/";
	File file = new File(fileDir);
	File[] subFiles = file.listFiles();//1，2，3，4，5文件夹
	File subFile;
	String filePath;
	File[] files;
	int subLength;
	int length = subFiles.length;
	File fileInNeed;
	String fileInNeedPath; 
	try {
	for(int i = 0;i < length ; i++){//length
		subFile = subFiles[i];
		filePath = subFile.getAbsolutePath();//..../geolifedata/1
		//System.out.println("filePath:"+filePath);
		files = subFile.listFiles();//重新定义string = subFile.getAbsolutePath()+“/”
		subLength = files.length;//subLength=10
		
		String writefilePath = filePath+"/Angsize"+(i+1)+dest;			  
		BufferedWriter bufferWritter = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(writefilePath)));
		
		for(int jj = 0; jj < 10; jj++){
			ArrayList<Point> P = new ArrayList();
			ArrayList<Point> R = new ArrayList();
			fileInNeed = files[jj];
			fileInNeedPath = fileInNeed.getAbsolutePath();	//.txt文件	
			System.out.println(fileInNeedPath);
			
			Point temppoint = new Point();
			reader = new BufferedReader(new FileReader(fileInNeedPath));
   		
    		String tempString = null;
    		int line = 1;
    		int j=0;
        // 一次读入一行，直到读入null为文件结束
    		while ((tempString = reader.readLine()) != null) {	//给点赋值，ArrayList P是一条轨迹
            // 显示行号       	
    			if(line>6){    				
    				String[] temp=tempString.split(",");   				
    				//System.out.println("p"+line+","+temp[0]+","+temp[1]+","+temp[5]+","+temp[6]);
    				//System.out.println("line " + line + ": " + tempString);            
    				temppoint = new Point();
    				temppoint.x=Double.valueOf(temp[0]);	
    				temppoint.y=Double.valueOf(temp[1]);
    				temppoint.time=temp[5]+","+temp[6];
    				temppoint.num=j;
    				P.add(temppoint);
    				j++;
    			}
    			line++;   			
    		}
//P中点存储一条轨迹
    		
    		
    		long startTime=System.nanoTime();
    		
    		
    		
    		
    		R.add(P.get(0));
    		double deviation=0;
    		for(int ii=1;ii<P.size()-1;ii++){
    			deviation = deviation + AngularDeviation(P, ii);
    			if(Math.abs(deviation)>error_t){
    				R.add(P.get(ii));
    				deviation = 0;
    			}
    		}   		
    		R.add(P.get(P.size()-1));
    		
    		long endTime=System.nanoTime();
			
    		
    	    System.out.println("running time:"+(endTime-startTime)/1000000.+"ms");
    	    time.add((endTime-startTime));
    		double T=P.size();
    		double T1=R.size();
    		System.out.println("compression rate:"+((T-T1)/T));
    		rate.add(((T-T1)/T));
    		
  
    		reader.close();
    			
		}
		averagetime=0;
		averagerate=0;
		for(int m=0;m<time.size();m++){
			averagetime = averagetime+time.get(m);
		}
		for(int m=0;m<rate.size();m++){
			averagerate = averagerate+rate.get(m);
		}
		
		bufferWritter.write(String.valueOf(averagetime/time.size()));
		bufferWritter.newLine();
		bufferWritter.write(String.valueOf(averagerate/rate.size()));
		bufferWritter.close();
	}
	}
		     catch (IOException e) {
		        e.printStackTrace();
		    } finally {
		        if (reader != null) {
		            try {
		                reader.close();
		            } catch (IOException e1) {
		            }
		        }
	
		}

	
	
	
	
	
}
*/
   public static void one(String pp) {
	    //System.out.println(error);
	    //error_t = error/2;
	    //System.out.println(error_t);
	    BufferedReader reader = null;   
	    R = new ArrayList(); 
	    ArrayList<Point> P = new ArrayList();
	    try {
	    		Point temppoint = new Point();
	    		reader = new BufferedReader(new FileReader(pp));//"tempdata/geolifedata/5/"+str+".txt"));
	    		String tempString = null;
	    		int line = 1;
	    		int j=0;
	        // 一次读入一行，直到读入null为文件结束
	    		while ((tempString = reader.readLine()) != null) {	//给点赋值，ArrayList P是一条轨迹
	            // 显示行号       	
	    		    				
	    				String[] temp=tempString.split(" ");   				
	    				temppoint = new Point();
	    				temppoint.x=Double.valueOf(temp[0]);	
	    				temppoint.y=Double.valueOf(temp[1]);
					temppoint.time=temp[2];
					//System.out.println(temp[0]+","+temp[1]+","+temp[2]);
	    				//temppoint.time=temp[5]+","+temp[6];
	    				temppoint.num=j;
	    				P.add(temppoint);
	    				j++;
	    				line++;   			
	    		}
	    		long startTime=System.nanoTime();
	    		R.add(P.get(0));
	    		double deviation=0;
	    		for(int i=1;i<P.size()-1;i++){
	    			deviation = deviation + AngularDeviation(P, i);
				//System.out.println(error_t);
	    			if(Math.abs(deviation)>error_t){
	    				R.add(P.get(i));
	    				deviation = 0;
	    			}
	    		}   		
	    		R.add(P.get(P.size()-1));
	    		long endTime=System.nanoTime();
	    		double T=P.size();
	    		double T1=R.size();	
	    		System.out.println("running time:"+(endTime-startTime)*1.0/1000000.);
	    		System.out.println("compression rate:"+((T1)/T));
	    		reader.close();
	    } catch (IOException e) {
	        e.printStackTrace();
	    } finally {
	        if (reader != null) {
	            try {
	                reader.close();
	            } catch (IOException e1) {
	            }
	        }
	    }
		
    }

public static void main(String args[]) throws IOException{	
	 System.out.println(args[0]);
	 //System.out.print(" Angular: ");
	 error_t = Double.valueOf(args[1]).doubleValue();
	 one(args[0]);
}

}
