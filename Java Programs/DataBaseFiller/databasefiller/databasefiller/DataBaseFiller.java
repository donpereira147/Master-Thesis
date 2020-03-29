package databasefiller;

import static databasefiller.DBConnector.populateDBwithResults;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

import databasefiller.Results;

public class DataBaseFiller 
{
	public static void main(String[] args)
    {
		int simNumber = -1;
		int simAux = -1;
		Result v2x, no_v2x;
		List<Results> allResults = new ArrayList<>();
		//ler o ficheiro
		
		try 
		{
			byte[] fileInBytes = Files.readAllBytes(Paths.get("..//..//src//veins-4.7.1//examples//veins//testResults2.txt"));
			String [] results = new String(fileInBytes).split("----------------------\r\n");
			
			
			for(int i = 0; i < results.length; i++)
			{
				String [] diffComponents = results[i].split("\r\n###\r\n");
				
				if(diffComponents.length == 3)
				{
					simAux = Integer.parseInt(diffComponents[2].replace("\n", "").replace("\r", ""));
					
					if(simAux < simNumber)
						simNumber++;
					else
						simNumber = simAux;
					
					v2x = filterStringIntoResult(diffComponents[0], simNumber);
					no_v2x = filterStringIntoResult(diffComponents[1], simNumber);
					
					if(!v2x.isEmpty() && !no_v2x.isEmpty())
						allResults.add(new Results(v2x, no_v2x));
					
					
					//imprimir para ver se está bem
					/*for(Results r : allResults)
					{
						System.out.println("---");
						System.out.println(r.getWithV2X());
						System.out.println(r.getWithoutV2X());
					}*/
					
					
					
					//meter as cenas na BD

				}
				
				
			}
			populateDBwithResults(allResults);
			
		} 
		catch (IOException e) 
		{
			e.printStackTrace();
		}
	
    }
	
	public static Result filterStringIntoResult(String res, int simNumber)
	{
		boolean v2x, bigGrid;
		int stoppedTime, totalTime;
		float totalDistance, co2;
		Result finalResult;
		String [] fields = res.split("\r\n");
		for(int i = 0; i < fields.length; i++)
			System.out.println(fields[i]);
		if(fields.length < 3)
			finalResult = new Result();
		else
		{
			if(fields[0].split(":")[1].equals("true") )
				v2x = true;
			else 
				v2x = false;
			
			if(fields[8].split(":")[1].equals("true") )
				bigGrid = true;
			else 
				bigGrid = false;
			
			/*System.out.println("->" + fields[3].split(":")[1]);
			System.out.println("->" + fields[4].split(":")[1]);
			System.out.println("->" + fields[5].split(":")[1]);
			System.out.println("->" + fields[6].split(":")[1]);*/
			
			totalTime = Integer.parseInt(fields[3].split(":")[1].replace("\n", "").replace("\r", ""));
			stoppedTime = Integer.parseInt(fields[4].split(":")[1].replace("\n", "").replace("\r", "").replace(" ", ""));
			
			totalDistance = Float.parseFloat(fields[5].split(":")[1].replace("\n", "").replace("\r", ""));
			co2 = Float.parseFloat(fields[6].split(":")[1].replace("\n", "").replace("\r", ""));
			
			//public Result(int simNumber, int elapsedTime, int stoppedTime, boolean v2x, float totalDistance, float co2) {
			finalResult = new Result(simNumber, totalTime, stoppedTime, v2x, totalDistance, co2, bigGrid);
		}
		
		
		
		return finalResult;
	}
}
