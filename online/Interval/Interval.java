
import java.io.*;
import java.util.*;




public class Interval {
	public static double PI = 3.1415926;
	public static double error = PI;
	public static double error_t=error/2;
	public static String str = "20081106231922";
	public static String dest="e"+String.valueOf(error)+"TimeRate.txt";
	public static ArrayList<Point> R = new ArrayList();
	//public static ArrayList<Long> time = new ArrayList();
	//public static ArrayList<Double> rate = new ArrayList();
	public static String save_path ;
	public static long averagetime;
	public static double averagerate;
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
    static class Range{//定义区间
    	double left;
    	double right;
    }
	public static Range Fan(double d, double error){//给出一个角度的规范化后的扇形区域
		Range range = new Range();
		range.left= Standardization(d+error);
		range.right = Standardization(d-error);
		return range;
	}
	public static boolean notInt(Range ComSub, Range range){//判断两个区间是否有交集，true=无交集
		if(ComSub.left<ComSub.right){		//ComSub的区间跨（0，1）方向
			if(range.left>range.right){
				if(range.right>ComSub.left&&range.left<ComSub.right){
				return true;
			}
			}			
							
		}
		else{		//ComSub不跨（0，1）方向
			if(range.left<range.right){//跨（0，1）
				if(ComSub.right>range.left&&ComSub.left<range.right){		
					return true;
				}				
			}
			else if(range.left<ComSub.right||range.right>ComSub.left){
				return true;
			}
				}				
		return false;
	}
	public static boolean isInt1(Range inter, double direction){//判断方向是否在区间内
		if(inter.left<inter.right){
			if((direction<=inter.left)||(direction>=inter.right)){
				return true;
			}
		}
		else{
			if((direction<=inter.left)&&(direction>=inter.right)){
				return true;
			}
		}
		return false;
	}
	public static Range Intersection(Range r1, Range r2){//求交集（r1的范围小于r2）
		Range intersection = new Range();
		if(r1.left<r1.right){		//r1跨(0,1)
			if((r2.left<=r1.left)||(r2.left>=r1.right)){		//交叉
				intersection.left = r2.left;
				intersection.right = r1.right;
			}
			else if((r2.right<=r1.left)||(r2.right>=r1.right)){		//交叉
				intersection.left = r1.left;
				intersection.right = r2.right;
			}
			else {		//不交叉	,r2包含了r1		
					intersection.left = r1.left;
					intersection.right = r1.right;				
			}
		}
		else{		//不跨(0,1)
			if((r2.left<=r1.left)&&(r2.left>=r1.right)){		//交叉
				intersection.left = r2.left;
				intersection.right = r1.right;
			}
			else if((r2.right<=r1.left)&&(r2.right>=r1.right)){		//交叉
				intersection.left = r1.left;
				intersection.right = r2.right;
			}
			else{		//不交叉,r2包含了r1
				intersection.left = r1.left;
				intersection.right = r1.right;
			}
		}
		return intersection;
	}
	
	
	
/*	public static void IntervalRunningTime(){

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
				
				String writefilePath = filePath+"/Intsize"+(i+1)+dest;			  
				BufferedWriter bufferWritter = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(writefilePath)));
				
				for(int jj = 0; jj < 10; jj++){
					ArrayList<Point> P = new ArrayList();
					ArrayList<Point> R = new ArrayList();
					fileInNeed = files[jj];
					fileInNeedPath = fileInNeed.getAbsolutePath();	//.txt文件	
					System.out.println(fileInNeedPath);
			
			
			
		Range ComSub = new Range();
		Range interval = new Range();
	    //File whole=new File("tempdata");
	    
	    
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
	    		//System.out.println(P.size());		// P.size()=21
	    		
	    		
	    	
	    		long startTime=System.nanoTime();
	    	    
	    		R.add(P.get(0));	    		
	    		double direcion12 = Standardization(Math.atan2((P.get(1).y-P.get(0).y),(P.get(1).x-P.get(0).x)));	//p0p1方向    		
	    		ComSub = Fan(direcion12,error);		//p0p1的扇形区域
	    		int s = 0;
	    		double directsum;		//简化段的方向
	    		for(int l=1;l<P.size()-1;l++){	    			
	    			//从p1p2开始算方向
	    			double directi=Standardization(Math.atan2((P.get(l+1).y-P.get(l).y), (P.get(l+1).x-P.get(l).x)));
	    			////////////////////////////////////System.out.println("p"+i+"p"+(i+1)+":"+directi);
	    			//directi表示pipi+1的方向，在[0,2pi)之间
	    			interval = Fan(directi,error);		//interval表示pipi+1段的扇形区域
	    			if(notInt(ComSub, interval)==true){		//无交集的时候，保留pi
	    				R.add(P.get(l));	    				
	    				//System.out.println("wujiaoji:ComSub:["+ComSub.left+","+ComSub.right+"]--interval:["+interval.left+","+interval.right+"]");	    				
	    				//System.out.println("jiaoducha"+"p"+s+"p"+(i+1)+":"+Standardization(Math.atan2((P.get(i+1).y-P.get(s).y), (P.get(i+1).x-P.get(s).x))));
	    				ComSub = interval;	
	    				s = l;	    				
	    			}
	    			
	    			  else{//有交集
	    				ComSub = Intersection(ComSub, interval);		//求交集
	    				directsum = Standardization(Math.atan2(P.get(l+1).y-P.get(s).y, P.get(l+1).x-P.get(s).x));
	    				if (isInt1(ComSub, directsum)==false){		//不在ComSub内
	    					R.add(P.get(l));
	    					//System.out.println("jiaoji:["+ComSub.left+","+ComSub.right+"]+direction="+directsum+":point"+s+"point"+(i+1));
	    					ComSub = interval;
	    					s = l;    					
	    				}
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
	   // long endTime=System.currentTimeMillis(); //获取结束时间
		//System.out.println("running time:"+(endTime-startTime)+"ms");
	
	}*/
	public static void one(String pp) {
		BufferedReader reader = null;
		ArrayList<Point> P = new ArrayList();	
		R = new ArrayList();
		Range ComSub = new Range();
		Range interval = new Range();    	   
	    try { 	    	    	
	    	Point temppoint = new Point();
    		reader = new BufferedReader(new FileReader(pp));
//    		String filePath ="tempdata/geolifedata/5/"+str+"newInt.txt";
//    		BufferedWriter bufferWritter = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(filePath)));
    		String tempString = null;
    		int line = 1;
    		int j=0;
	        // 一次读入一行，直到读入null为文件结束
    		while ((tempString = reader.readLine()) != null) {	//给点赋值，ArrayList P是一条轨迹
            // 显示行号      	
    							
    				String[] temp=tempString.split(" ");
    				//System.out.println("p"+line+","+temp[0]+","+temp[1]+","+temp[5]+","+temp[6]);
    				//System.out.println("line " + line + ": " + tempString);            
    				temppoint = new Point();
    				temppoint.x=Double.valueOf(temp[0]);	
    				temppoint.y=Double.valueOf(temp[1]);
				temppoint.time=temp[2];
    				//temppoint.time=temp[5]+","+temp[6];
    				temppoint.num=j;
    				P.add(temppoint);
    				j++;
				line++;	    			
    		}
    		long startTime=System.nanoTime(); 
    		
    		R.add(P.get(0));	    		
    		double direcion12 = Standardization(Math.atan2((P.get(1).y-P.get(0).y),(P.get(1).x-P.get(0).x)));	//p0p1方向    		
    		ComSub = Fan(direcion12,error);		//p0p1的扇形区域
    		int s = 0;
    		double directsum;		//简化段的方向
    		for(int i=1;i<P.size()-1;i++){	    			
    			//从p1p2开始算方向
    			double directi=Standardization(Math.atan2((P.get(i+1).y-P.get(i).y), (P.get(i+1).x-P.get(i).x)));
    			////////////////////////////////////System.out.println("p"+i+"p"+(i+1)+":"+directi);
    			//directi表示pipi+1的方向，在[0,2pi)之间
    			interval = Fan(directi,error);		//interval表示pipi+1段的扇形区域
    			if(notInt(ComSub, interval)==true){		//无交集的时候，保留pi
    				R.add(P.get(i));	    				
    				//System.out.println("wujiaoji:ComSub:["+ComSub.left+","+ComSub.right+"]--interval:["+interval.left+","+interval.right+"]");	    				
    				//System.out.println("jiaoducha"+"p"+s+"p"+(i+1)+":"+Standardization(Math.atan2((P.get(i+1).y-P.get(s).y), (P.get(i+1).x-P.get(s).x))));
    				ComSub = interval;	
    				s = i;	    				
    			}
    			
    			else{//有交集
    				ComSub = Intersection(ComSub, interval);		//求交集
    				directsum = Standardization(Math.atan2(P.get(i+1).y-P.get(s).y, P.get(i+1).x-P.get(s).x));
    				if (isInt1(ComSub, directsum)==false){		//不在ComSub内
    					R.add(P.get(i));
    					//System.out.println("jiaoji:["+ComSub.left+","+ComSub.right+"]+direction="+directsum+":point"+s+"point"+(i+1));
    					ComSub = interval;
    					s = i;    					
    				}
    			}	
    			   			
    		}   		
    		long endTime=System.nanoTime();
    		
   	        System.out.println("running time:"+(endTime-startTime)/1000000.);
    		R.add(P.get(P.size()-1));
    		double T=P.size();
    		double T1=R.size();
    		System.out.println("compression rate:"+((T1)/T));
	    		
    		reader.close();
    		//bufferWritter.close();	    		
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
		 error = Double.valueOf(args[1]).doubleValue();
		 one(args[0]);
	}	
}
