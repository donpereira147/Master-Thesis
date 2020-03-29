package databasefiller;

import java.util.List;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class DBConnector 
{
	private static Connection c;
	
	public static void populateDBwithResults(List<Results> allResults)
	{
		Result withV2X, withoutV2X;
		try 
		{
			c = Connect.connect();
			System.out.println("size: " + allResults.size());
			for(Results res : allResults)
			{
				withV2X = res.getWithV2X();
				withoutV2X = res.getWithoutV2X();
				addResultToDB(withV2X);
				addResultToDB(withoutV2X);
				
				addGainsToDB(withV2X, withoutV2X, withV2X.getSimNumber());
			}
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
		}
		finally
		{
			try 
			{
				c.close();
			} catch (Exception e2) {
				e2.printStackTrace();
			}
		}
	}
	
	
	private static void addResultToDB(Result r) throws SQLException
	{
		int idDist, idTime, idEmission;
		idDist = idTime = idEmission = 0;
		
		PreparedStatement insertDist = c.prepareStatement("insert into Distance values (null, ?)");
		insertDist.setFloat(1, r.getTotalDistance());
		insertDist.executeUpdate();
		
		PreparedStatement insertTime = c.prepareStatement("insert into Time values (null, ?,?)");
		insertTime.setInt(1, r.getElapsedTime());
		insertTime.setInt(2, r.getStoppedTime());
		insertTime.executeUpdate();
		
		PreparedStatement insertCO2 = c.prepareStatement("insert into Emission values (null, ?)");
		insertCO2.setFloat(1, r.getCo2());
		insertCO2.executeUpdate();
		
		PreparedStatement getIdDist = c.prepareStatement("select max(idDistance) from distance");
		ResultSet rs = getIdDist.executeQuery();
		if(rs.next())
			idDist = rs.getInt(1);
		
		PreparedStatement getIdTime = c.prepareStatement("select max(idTime) from time");
		ResultSet rs1 = getIdTime.executeQuery();
		if(rs1.next())
			idTime = rs1.getInt(1);
		
		PreparedStatement getIdEmission = c.prepareStatement("select max(idEmission) from emission");
		ResultSet rs2 = getIdEmission.executeQuery();
		if(rs2.next())
			idEmission = rs2.getInt(1);
		
		PreparedStatement insertResult = c.prepareStatement("insert into Result values (null, ?, ?, ?, ?, ?, ?)");
		insertResult.setInt(1, idTime);
		insertResult.setInt(2, idEmission);
		insertResult.setInt(3, idDist);
		insertResult.setBoolean(4, r.isV2x());
		insertResult.setBoolean(5, r.isBigGrid());
		insertResult.setInt(6, r.getSimNumber());
		insertResult.executeUpdate();
		
		
	}
	
	private static void addGainsToDB(Result with, Result without, int simNumber) throws SQLException
	{
		//calcular ganhos/perdas
		/*float gainDist = gain(with.getTotalDistance(), without.getTotalDistance());
		float gainCO2 = gain(with.getCo2(), without.getCo2());
		float gainStopped = gain(with.getStoppedTime(), without.getStoppedTime());
		float gainElapsed = gain(with.getElapsedTime(), without.getElapsedTime());*/
		float gainDist = with.getTotalDistance() - without.getTotalDistance();
		float gainCO2 = with.getCo2() - without.getCo2();
		float gainStopped = with.getStoppedTime() - without.getStoppedTime();
		float gainElapsed = with.getElapsedTime() - without.getElapsedTime();
		
		
		
		//inserir ganhos na BD
		PreparedStatement insertDist = c.prepareStatement("insert into Distance_Gain values (?, ?)");
		insertDist.setInt(1, simNumber);
		insertDist.setFloat(2, gainDist);
		insertDist.executeUpdate();
		
		PreparedStatement insertStopped = c.prepareStatement("insert into StoppedTime_Gain values (?, ?)");
		insertStopped.setInt(1, simNumber);
		insertStopped.setFloat(2, gainStopped);
		insertStopped.executeUpdate();
		
		PreparedStatement insertElapsed = c.prepareStatement("insert into ElapsedTime_Gain values (?, ?)");
		insertElapsed.setInt(1, simNumber);
		insertElapsed.setFloat(2, gainElapsed);
		insertElapsed.executeUpdate();
		
		PreparedStatement insertCO2 = c.prepareStatement("insert into Emission_Gain values (?, ?)");
		insertCO2.setInt(1, simNumber);
		insertCO2.setFloat(2, gainCO2);
		insertCO2.executeUpdate();
		
		
	}
	
	private static float gain(int n, int o)
	{
		int increase = n - o;
		float sumting = increase / o;
		return sumting * 100;
		
	}
	
	private static float gain(float n, float o)
	{
		float increase = n - o;
		float sumting = increase / o;
		return sumting * 100;
	}
}
