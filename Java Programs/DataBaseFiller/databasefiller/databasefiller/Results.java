package databasefiller;

import databasefiller.Result;

public class Results 
{
	private Result withV2X, withoutV2X;

	public Results()
	{
		
	}
	
	public Results(Result withV2X, Result withoutV2X) 
	{
		super();
		this.withV2X = withV2X;
		this.withoutV2X = withoutV2X;
	}

	public Result getWithV2X() 
	{
		return withV2X;
	}

	public void setWithV2X(Result withV2X) 
	{
		this.withV2X = withV2X;
	}

	public Result getWithoutV2X() 
	{
		return withoutV2X;
	}

	public void setWithoutV2X(Result withoutV2X) 
	{
		this.withoutV2X = withoutV2X;
	}
	
	
}
