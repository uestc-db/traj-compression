

import java.util.*;

import java.io.*;
import java.math.*;

/**
 * Base class of OPERB.
 * 
 * @author Lin (linxl@buaa.edu.cn)
 *
 */
public abstract class TrajectoryCompressor {
	protected final static double EARTH_RADIUS = 6378137; // radius of the earth, in meter
	protected static double threshold = 20; // Threshold of PED error

	
	String strFileName = ""; //the original trajectory file
	protected List<GPSPoint> iTrajectory; // the original trajectory, saved in a list<GPSPoint>

	public abstract GPSLine[] compress(); //the compressing method

	/**
	 * load the trajectory from "strFileName" into "iTrajectory".
	 */
	void loadTrajectory() {
		File file = new File(strFileName);
		if (!file.isFile() || !file.exists()) {
			System.out.println("bad input file.");
			return;
		} else {
			// System.out.println("Read file: " + file);
		}
		try {
			InputStreamReader read = new InputStreamReader(new FileInputStream(file), "UTF-8");
			BufferedReader reader = new BufferedReader(read);
			String line;

			iTrajectory = new ArrayList<GPSPoint>();
			while ((line = reader.readLine()) != null) {
					parseLocation(line);
			}

			reader.close();
			read.close();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	/*
	 * parse a String line  into a GPSPoint.
	 * Structure of input text: device-sn, time-stamp, longitude, latitude, velo, direction
	 * Sample:   "967790028725";1440691884;39.8222766666667;116.422643333333;48.59;290
	 */
	void parseLocation(String str) {
		GPSPoint loc = null;

		String[] strs = str.split(" ");
		if (strs == null)
			return;
		if (strs.length >= 3) {
			loc = new GPSPoint();

			loc.time = strs[2];
			loc.latitude = new Double(strs[0]).doubleValue();
			loc.longitude = new Double(strs[1]).doubleValue();
			if (loc.latitude < 0 || loc.latitude > 90 || loc.longitude < 0 || loc.latitude > 180) {
				// System.out.println(" Special Point : " + loc.latitude + "," +
				// loc.longitude);
			}
			this.iTrajectory.add(loc);
		}
	}

	/*
	 * GeoLife has a different data format
	 * Structure: latitude, longitude, ?,high, day, date, time, 
	 * Sample: 39.994622,116.326757,0,492,39748.496400463,2008-10-27,11:54:49
	 */
	static long tcounter = 0;
	void parseGeolife(String str) {

		GPSPoint loc = null;

		if (str.length() < 10)
			return;
		String[] strs = str.split(",");
		if (strs == null)
			return;
		if (strs.length >= 3) {
			loc = new GPSPoint();

			//loc.time = ++tcounter;
			loc.latitude = new Double(strs[0]).doubleValue();
			loc.longitude = new Double(strs[1]).doubleValue();

			this.iTrajectory.add(loc);
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	protected static double rad(double d) {
		return d * Math.PI / 180.0;
	}

	/*
	 * Get the distance of two GPS points.
	 */
	public static double getDistanceOfP2P(double lat1, double lng1, double lat2, double lng2) {
		/*	
		double radLat1 = rad(lat1);
		double radLat2 = rad(lat2);

		double dw = radLat1 - radLat2; 
		double dj = rad(lng1) - rad(lng2); 

		double s = 2 * Math.asin(Math.sqrt(
				Math.pow(Math.sin(dw / 2), 2) + Math.cos(radLat1) * Math.cos(radLat2) * Math.pow(Math.sin(dj / 2), 2)));

		s = s * EARTH_RADIUS;
		return s;*/
		return Math.sqrt((lat1-lat2)*(lat1-lat2)+(lng1-lng2)*(lng1-lng2));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/**
	 * Get the approximate include angle between two directed line segments.
	 * @param angle1: angle of line 1.
	 * @param angle2: angle of line 2.
	 * @return [-Pi, Pi]
	 */
	static double getIncludedAngle(double angle1, double angle2) {
		double theta = angle2 - angle1;
		if (theta > Math.PI)
			theta = -2 * Math.PI + theta;
		if (theta < -Math.PI)
			theta = 2 * Math.PI + theta;
		return theta;
	}

	/*
	 * The distance of a point to a "direct line segment"
	 */
	static double getDistance(GPSPoint lineStart, GPSPoint lineEnd, GPSPoint loc) {

		double dis = 0;

		double r = getDistanceOfP2P(lineStart.latitude, lineStart.longitude, loc.latitude, loc.longitude);
		if (r == 0)
			return 0;

		double lineAngle = getAngleOfVector(lineStart.latitude, lineStart.longitude, lineEnd.latitude,
				lineEnd.longitude);

		double sita = getAngleOfVector(lineStart.latitude, lineStart.longitude, loc.latitude, loc.longitude);

		dis = r * Math.sin(sita - lineAngle);

		return dis;
	}

	/*
	 * The distance of a point to a "direct line segment"
	 */
	static double getDistanceOfP2R(GPSLine line, GPSPoint loc) {
		double dis = 0;

		double r = getDistanceOfP2P(line.startLatitude(), line.startLongitude(), loc.latitude, loc.longitude);
		if (r == 0)
			return 0;

		double sita = getAngleOfVector(line.startLatitude(), line.startLongitude(), loc.latitude, loc.longitude);

		dis = r * Math.sin(sita - line.angle);

		return dis;
	}

	
	/**
	 * get the approximate angle of a direct line segment
	 * 
	 * @param latS - of start point of direct line segment
	 * @param lngS - of start point of direct line segment
	 * @param latE - of end point of direct line segment
	 * @param lngE - of end point of direct line segment
	 * @return [0,2pi)
	 */
	static double getAngleOfVector(double latS, double lngS, double latE, double lngE) {

		if (latS == latE && lngS == lngE)
			return 0;
		else if (latS == latE && lngS < lngE)
			return 0;
		else if (latS == latE && lngS > lngE)
			return Math.PI;
		else if (latS < latE && lngS == lngE)
			return Math.PI / 2;
		else if (latS > latE && lngS == lngE)
			return 3.0 * Math.PI / 2;

		double high = getDistanceOfP2P(latE, lngE, latS, lngE);
		double length = getDistanceOfP2P(latS, lngS, latE, lngE);
		if (length == 0)
			return 0;

		double angle = Math.asin(high / length);// in [-pi/2, pi/2]

		if (latS < latE && lngS > lngE)
			angle = Math.PI - angle; // in [pi/2, pi)
		if (latS > latE && lngS > lngE)
			angle = Math.PI + angle; // in [pi, 3pi/2)
		if (latS > latE && lngS < lngE)
			angle = 2 * Math.PI - angle; // in [3pi/2, 2pi)

		return angle;
	}

}
