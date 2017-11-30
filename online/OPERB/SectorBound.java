

public class SectorBound {
	double angleU;
	double angleL;

	public SectorBound(double angleL, double angleU) {
		super();
		this.angleU = angleU;
		this.angleL = angleL;
	}

	public SectorBound() {
		angleU = 0;
		angleL = 0;

	}

	public SectorBound intersects(SectorBound s) {
		if(s == null) return null;
		SectorBound result = new SectorBound(angleL, angleU);
		double delta = angleL - s.angleU;
		// if(delta > 0 || ) return null;
		if (this.angleU > this.angleL) {
			if (s.angleU > s.angleL) {
				if (s.angleU < this.angleL || s.angleL > this.angleU)
					return null;
				if (s.angleU < this.angleU)
					result.angleU = s.angleU;
				if (s.angleL > this.angleL)
					result.angleL = s.angleL;
			} else {
				result = new SectorBound(s.angleL,s.angleU);
				if ((s.angleU < this.angleL && this.angleU < s.angleL))
					return null;
				if(this.angleU > s.angleL || this.angleU < s.angleU)
					result.angleU = this.angleU;
				if(this.angleL > s.angleL || this.angleL < s.angleU)
					result.angleL = this.angleL;
				// if(this.angleL < s.angleL) result.angleL = s.angleL;
			}
		}

		else {
			if (s.angleU > s.angleL) {
				if ((this.angleU < s.angleL && s.angleU < this.angleL))
					return null;
				if(s.angleU >this.angleL || s.angleU < this.angleU)
					result.angleU = s.angleU;
				if(s.angleL > this.angleL || s.angleL < this.angleU)
					result.angleL = s.angleL;

			} else {
				result.angleU = Math.min(this.angleU, s.angleU);
				result.angleL = Math.max(this.angleL, s.angleL);
			}
		}
		return result;
	}

	public static void main(String[] args) {

		// case1:
		SectorBound sb1 = new SectorBound(30, 110);
		SectorBound sb2 = new SectorBound(10, 100);
		testIntersect(sb1, sb2);

		// case2:
		sb1 = new SectorBound(320, 50);
		sb2 = new SectorBound(30, 100);
		testIntersect(sb1, sb2);

		// case3:
		sb1 = new SectorBound(320, 50);
		sb2 = new SectorBound(300, 340);
		testIntersect(sb1, sb2);
		// case3:
		sb1 = new SectorBound(320, 50);
		sb2 = new SectorBound(300, 310);
		testIntersect(sb1, sb2);

		// case3:
				sb1 = new SectorBound(320, 50);
				sb2 = new SectorBound(60, 70);
				testIntersect(sb1, sb2);
				
		// case4:
		sb1 = new SectorBound(320, 50);
		sb2 = new SectorBound(300, 10);
		testIntersect(sb1, sb2);
		
		// case4:
		sb1 = new SectorBound(6.259224760063978, 0.3542372361427244);  //358.62,20.29
		sb2 = new SectorBound(0.07336764241565774, 0.13040223113043176); //4.2,7.448
		testIntersect(sb1, sb2);
		
	}

	public static void testIntersect(SectorBound sb1, SectorBound sb2) {
		SectorBound res = new SectorBound();
		res = sb1.intersects(sb2);
		System.out.print(
				"(" + sb1.angleL + "," + sb1.angleU + ")" + "intersects" + "(" + sb2.angleL + "," + sb2.angleU + ")");
		if (res != null) {
			System.out.println(" is " + "(" + res.angleL + "," + res.angleU + ")");
		} else {
			System.out.println(" is null");
		}
		res = sb2.intersects(sb1);
		System.out.print(
				"(" + sb2.angleL + "," + sb2.angleU + ")" + "intersects" + "(" + sb1.angleL + "," + sb1.angleU + ")");
		if (res != null) {
			System.out.println(" is " + "(" + res.angleL + "," + res.angleU + ")");
		} else {
			System.out.println(" is null");
		}
	}
}