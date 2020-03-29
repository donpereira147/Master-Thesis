package databasefiller;

public class Result 
{
	private int simNumber, elapsedTime, stoppedTime;
	private boolean v2x, bigGrid;
	private float totalDistance, co2;
	
	public Result()
	{
		simNumber = elapsedTime = stoppedTime = -1;
		v2x = false;
		totalDistance = co2 = -1;
	}
	
	public Result(int simNumber, int elapsedTime, int stoppedTime, boolean v2x, float totalDistance, float co2, boolean bigGrid) {
		super();
		this.simNumber = simNumber;
		this.elapsedTime = elapsedTime;
		this.stoppedTime = stoppedTime;
		this.v2x = v2x;
		this.totalDistance = totalDistance;
		this.co2 = co2;
		this.bigGrid = bigGrid;
	}
	public int getSimNumber() {
		return simNumber;
	}
	public void setSimNumber(int simNumber) {
		this.simNumber = simNumber;
	}
	public int getElapsedTime() {
		return elapsedTime;
	}
	public void setElapsedTime(int elapsedTime) {
		this.elapsedTime = elapsedTime;
	}
	public int getStoppedTime() {
		return stoppedTime;
	}
	public void setStoppedTime(int stoppedTime) {
		this.stoppedTime = stoppedTime;
	}
	public boolean isV2x() {
		return v2x;
	}
	public void setV2x(boolean v2x) {
		this.v2x = v2x;
	}
	public float getTotalDistance() {
		return totalDistance;
	}
	public void setTotalDistance(float totalDistance) {
		this.totalDistance = totalDistance;
	}
	public float getCo2() {
		return co2;
	}
	public void setCo2(float co2) {
		this.co2 = co2;
	}
	
	public boolean isEmpty()
	{
		return (simNumber == -1);
	}
	
	public boolean isBigGrid()
	{
		return bigGrid;
	}
	
	public void setBigGrid(boolean b)
	{
		bigGrid = b;
	}
	

	@Override
	public String toString() {
		return "Result [simNumber=" + simNumber + ", elapsedTime=" + elapsedTime + ", stoppedTime=" + stoppedTime
				+ ", v2x=" + v2x + ", totalDistance=" + totalDistance + ", co2=" + co2 + "]";
	}

}
