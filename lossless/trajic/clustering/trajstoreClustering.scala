// Author: Dr Zhen He
//
// /Users/zhenhe/Documents/spark/spark-1.1.0/bin/spark-shell --master local[2]  --driver-memory 8g

def convertToSecsFromMidNight(time : String) : Int = {
	val splitTime = time.split(":").map(x => x.toInt)
	//require(splitTime(0) * 3600 + splitTime(1) * 60 + splitTime(2) > 0, "time string = " + time)
	math.abs(splitTime(0)) * 3600 + splitTime(1) * 60 + splitTime(2) 
}

// computes distance in terms of kms
def euclDistance (p1: (Double, Double ), p2: (Double, Double)): Double = {
	//math.sqrt((p1._1 - p2._1) * (p1._1 - p2._1) + (p1._2 - p2._2) * (p1._2 - p2._2) ) 
    var lat1 = p1._1 * math.Pi / 180.0
    var lat2 = p2._1 * math.Pi / 180.0
    var lon1 = p1._2 * math.Pi / 180.0
    var lon2 = p2._2 * math.Pi / 180.0

    var dlat = lat2 - lat1
    var dlon = lon2 - lon1
    var a = math.sin(dlat / 2.0) * math.sin(dlat / 2.0) + math.sin(dlon / 2.0) *  math.sin(dlon / 2.0) * math.cos(lat1) * math.cos(lat2)
    var c = 2 * math.atan2(math.sqrt(a), math.sqrt(1.0 - a))
    val EARTH_RADIUS = 6371.0
    require ((EARTH_RADIUS * c) >= 0.00000000, "distance is negative p1 = " + p1 + " p2 = " + p2)
//    println("distance equals = " + EARTH_RADIUS * c)
    EARTH_RADIUS * c
}

import breeze.linalg._

def trajMaxDistance(traj1 : Seq[(Double, Double)], traj2 : Seq[(Double, Double)], mError : Double) : Double = {
	var i = 0
	var distanceFromBeginningT1 =0.000000001
	var maxDistance = 0.0
	while (i < traj1.length) {
		var j = 0
		var distanceFromBeginningT2 = 0.000000000
		var distanceFromBeginningT2Prev = 0.000000000

		while ( (j < traj2.length) && (distanceFromBeginningT2 < (distanceFromBeginningT1 + 0.00000001)))  {
			j += 1
			if ( j < traj2.length) {
				distanceFromBeginningT2Prev = distanceFromBeginningT2
				distanceFromBeginningT2 += euclDistance(traj2(j), traj2(j -1))
			}
		}

		var correspondingT2Point = (0.0, 0.0)
		//println("here traj 2 length = "  + traj2.length + " j = " + j + "traj 1 length = "  + traj1.length + " i = " + i )
	    if (distanceFromBeginningT2 < (distanceFromBeginningT1 + 0.00000001)) {

			// we did not find a point within the current end point of T2 so we just use the current T2 end point
			correspondingT2Point = traj2(traj2.length -1)
		}
		else if (j == 0) {
			correspondingT2Point = traj2(0)
		}
		else {
			// we need use linear interploation 
			j = math.min(j, traj2.length -1)
			require(j > 0, "j is less than 1 traj2 length = " + traj2.length)
			val res = DenseVector(traj2(j-1)._1, traj2(j-1)._2)  + 
					  ((DenseVector(traj2(j)._1, traj2(j)._2) - DenseVector(traj2(j-1)._1, traj2(j-1)._2)) :*= (distanceFromBeginningT1 - distanceFromBeginningT2Prev) / euclDistance(traj2(j), traj2(j -1)))  
			correspondingT2Point = (res(0), res(1))
		}
		maxDistance = math.max(maxDistance, euclDistance(traj1(i), correspondingT2Point))
		if (maxDistance > mError) {
			return maxDistance
		}
	//	println("maxDistance = "  + maxDistance)
		i += 1
		if ( i < traj1.length)
			distanceFromBeginningT1 += euclDistance(traj1(i), traj1(i -1)) 
	}
	return maxDistance
}

def computeTimeSeq(clusterCenter : Seq[(Double, Double, Int)], clusterMember: Seq[(Double, Double, Int)]) : Seq[Int] =  {
	var i = 0 
	var distanceFromBeginningCenter = 0.000000001
	var returnList = List[Int]() 
	while (i < clusterCenter.length) {
		var j = 0
		var distanceFromBeginningMember = 0.00000000
		var distanceFromBeginningMemberPrev = 0.00000000
		while ( (j < clusterMember.length) && (distanceFromBeginningMember < (distanceFromBeginningCenter + 0.00000001)))  {
			j += 1
			if ( j < clusterMember.length) {
				distanceFromBeginningMemberPrev = distanceFromBeginningMember
				distanceFromBeginningMember += euclDistance((clusterMember(j)._1, clusterMember(j)._2),(clusterMember(j-1)._1, clusterMember(j-1)._2))
			}
		}
		if (distanceFromBeginningMember < (distanceFromBeginningCenter + 0.00000001)) {
			// we did not find a point within the current end point of T2 so we just use the current T2 end point
			returnList = returnList :+ clusterMember(clusterMember.length -1)._3 
		}
		else {
			j = math.min(j, clusterMember.length -1)
			require((clusterMember(j)._3 - clusterMember(j-1)._3) >= 0, "timestamp backwards")
			//require((distanceFromBeginningCenter - distanceFromBeginningMemberPrev) >= 0.000, "distance backwards, centerDist = " + distanceFromBeginningCenter + ", MemberPrev = " + distanceFromBeginningMemberPrev)
		//	require(clusterMember(j-1)._3 >= 0, "single cluster timestamp negative = " +  clusterMember(j-1)._3)
			// we need use linear interploation 
			var diff = distanceFromBeginningCenter - distanceFromBeginningMemberPrev
			if  (diff <= 0.0000000001) {
				diff = 0.000
			}
			val res = clusterMember(j-1)._3.toDouble  
				+ (diff / euclDistance((clusterMember(j)._1, clusterMember(j)._2),(clusterMember(j-1)._1, clusterMember(j-1)._2))) * (clusterMember(j)._3 - clusterMember(j-1)._3).toDouble
			require(res > 0, "res negative" )
			returnList = returnList :+ res.toInt

		}
		i += 1
		if ( i < clusterCenter.length)
			distanceFromBeginningCenter += euclDistance((clusterCenter(i)._1, clusterCenter(i)._2),(clusterCenter(i-1)._1, clusterCenter(i-1)._2)) 
	}
	returnList.toSeq
}


// This will return a tuple in which the first element stores the cluster centers and the second element stores the compressed members of the cluster centers
def compressTrajs(trajs : Array[(Long, Seq[(Double, Double, Int)])], mError : Double) : (List[(Long, Seq[(Double, Double, Int)])], List[(Long, Long, Seq[Int])]) = {
	var i = 0;
	var clusterCenterList = List[(Long, Seq[(Double, Double, Int)])]() 
	var clusterMemberList = List[(Long, Long, Seq[Int])]()
	//println("num trajs in grid cell = " + trajs.length)
	while (i < trajs.length) {
		
		if (trajs(i)._2.length == 1) {
			// put trajectories that have a single point into the centers list
			clusterCenterList =  trajs(i) :: clusterCenterList
		}
		else {
			if (clusterMemberList.filter{case(clusterParentID, clusterMemberID, trajInfo) => clusterMemberID == trajs(i)._1}.length == 0) {
				clusterCenterList = trajs(i) :: clusterCenterList
				var j = i+1
				while ( j < trajs.length) {
					println("inner iter = "  + j + " traj inner length = " + trajs(j)._2.length + " outer iter = "  + i + " traj outer length = " + trajs(i)._2.length + " total num of trajs in grid cell = " + trajs.length)
					if ((j != i) && (clusterMemberList.filter{case(clusterParentID, clusterMemberID, trajInfo) => clusterMemberID == trajs(j)._1}.length == 0)
						 && (trajMaxDistance(trajs(i)._2.map(x => (x._1, x._2)),trajs(j)._2.map(x => (x._1, x._2)), mError) < mError )) {
						var timeList = computeTimeSeq(trajs(i)._2, trajs(j)._2)
						clusterMemberList =  (trajs(i)._1, trajs(j)._1, timeList) :: clusterMemberList 
					}

					j += 1
				}
			}

		}
		i += 1
	}
	(clusterCenterList, clusterMemberList)
}

val input = sc.wholeTextFiles("/Users/zhenhe/Documents/spark/inputData/Trajstore/illonis data set/gpsdata/data/*")

val trajIdsMap = input.map{case(trajID, info) => trajID}.distinct.zipWithIndex.collect.toMap
val cleanedTrajectories = input.map{case(trajID, info) => (trajIdsMap(trajID), info.split("\n"))}
val cleanedTraj2 = cleanedTrajectories.flatMap{case(trajID, infoArray) => infoArray.map(x => (trajID, x))}
val cleanedTraj3 = cleanedTraj2.map{case(trajID, info) => (trajID, info.split("\\|"))}
val cleanedTraj4 = cleanedTraj3.filter(x => x._2.length >= 3).map{case(trajID, info) => (trajID, info(0).trim.toDouble / 100.0, info(1).trim.toDouble / 100.0, info(2).trim)}
val cleanedTrajWithTimeConverted =  cleanedTraj4.filter(x => x._4.split(":").length >= 3).map{case(trajID, lat,longitude,time) => (trajID, lat, longitude, convertToSecsFromMidNight(time))}
// don't forget to sort the data according to time later.
// we now break the data into 20 x 20 grid cells
val latNumDivisions = 20
val longNumDivisions = 20

val minLat = cleanedTrajWithTimeConverted.map(x => x._2).min
val maxLat = cleanedTrajWithTimeConverted.map(x => x._2).max
val minLong = cleanedTrajWithTimeConverted.map(x => x._3).min
val maxLong = cleanedTrajWithTimeConverted.map(x => x._3).max

def gridCell(v : Double, minV : Double, maxV : Double, numDiv : Int) : Int = {
	math.floor((v - minV) * numDiv.toDouble / (maxV - minV)).toInt
}
val griddedTrajData = cleanedTrajWithTimeConverted.map{case(trajID, lat, longitude, time) => (trajID, gridCell(lat, minLat, maxLat, latNumDivisions).toString + "-" + gridCell(longitude, minLong, maxLong, longNumDivisions).toString, lat, longitude, time)}
val grouped = griddedTrajData.map{case(trajID, gridC,lat, longitude, time) => (gridC, (trajID, lat, longitude, time))}.groupByKey
val trajIDGrouped = grouped.map{case((gridC, trajs)) => (gridC, trajs.toSeq.groupBy{case(trajID, lat, longitude, time) => (trajID)})}.map{case(gridC, trajs) => (gridC, trajs.toList)}
val trajIDGroupedSorted = trajIDGrouped.map{case(gridC, trajs) => (gridC, trajs.map{case(trajID, pointsList) => (trajID, pointsList.map{case(tID, lat, longitude, time)=> (lat, longitude, time)}.sortBy(x => x._3))})}.repartition(2).cache
val errorArray = Array(0.001, 0.005, 0.010, 0.020, 0.030, 0.040, 0.050, 0.060, 0.07, 0.08, 0.09, 0.100)  
import java.io._
val uncompressedCount = cleanedTrajWithTimeConverted.count
var compressionRatioWriter = new PrintWriter(new File("/Users/zhenhe/Documents/spark/outputData/trajStoreOutput/compressionRatio.txt"))
compressionRatioWriter.write("maxError, orig no. of doubles, orig no. of ints, orig size (bytes), compressed no. of doubles, compressed no. ints, compressed size (bytes), compression ratio\n")
var k =0
while (k < errorArray.length) {
    var maxError = errorArray(k)
   	val compressedData = trajIDGroupedSorted.collect.map{case(gridC, trajs) => (gridC, compressTrajs(trajs.toArray, maxError))}
	val centerArray = compressedData.map{case(gridC, (centers, members )) => centers.map{case(trajID, trajInfo) => trajInfo.map{case(lat, lon, time) => (gridC, trajID, lat, lon, time)}}.flatten}.flatten
	val memberArray = compressedData.map{case(gridC, (centers, members )) => members.map{case(centerID, memberID, timeSeq) => timeSeq.map(time => (gridC, centerID, memberID, time))}.flatten}.flatten
	var writer = new PrintWriter(new File("/Users/zhenhe/Documents/spark/outputData/trajStoreOutput/centerList-maxError-" +maxError+ ".txt" ))
	centerArray.foreach(x => writer.write(x._1 + "," + x._2 +"," + x._3 + "," + x._4 + "," + x._5 + "\n"))
	writer.close

	writer = new PrintWriter(new File("/Users/zhenhe/Documents/spark/outputData/trajStoreOutput/memberList-maxError-" +maxError+ ".txt" ))
	memberArray.foreach(x => writer.write(x._1 + "," + x._2 +"," + x._3 + "," + x._4 + "\n"))
	writer.close
	val origSize = uncompressedCount * 2 * 8 + uncompressedCount * 4
	val compressedNumInts = centerArray.length + memberArray.length
	val compressedSize = centerArray.length * 2 * 8 + compressedNumInts * 4
	val compressionRatio =  origSize.toDouble / compressedSize.toDouble 
	compressionRatioWriter.write(maxError + "," + uncompressedCount * 2 + "," + uncompressedCount + "," + origSize + "," +  centerArray.length * 2 + "," + compressedNumInts  + "," +  compressedSize + "," + compressionRatio + "\n")
	compressionRatioWriter.flush
	k += 1
}
compressionRatioWriter.close




