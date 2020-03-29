package randompathgenerator;

import static randompathgenerator.PathBetweenTwoNodes.createFileWithEVPaths;
import static randompathgenerator.PathBetweenTwoNodes.pathsBetweenTwoNodes;
import static randompathgenerator.PathBetweenTwoNodes.manyManyPaths;
import static randompathgenerator.PathBetweenTwoNodes.generateRandomEmergencyVehicleRoute;

import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.concurrent.ThreadLocalRandom;

public class PathFileGenerator 
{

	public static void pathFileCreator()
	{
		int numVehicles;
		String routes, vehV2X, vehNoV2X;
		FileOutputStream file;
        StringBuilder xmlV2X = new StringBuilder();
        StringBuilder xmlNoV2X = new StringBuilder();
        try 
        {
        	numVehicles = ThreadLocalRandom.current().nextInt(400, 500);
            List<String> pathsBeforeTheLastOne = manyManyPaths(numVehicles);
            
            /*System.out.println("gGFÇIFF");
            System.out.println("Size:" + pathsBeforeTheLastOne.size());
            for(String s : pathsBeforeTheLastOne)
            	System.out.println(s);
            */

            
            
            String header = produceHeader(); 
            //simulação terá de ter mais de um carro e menos de 100
            //numVehicles = ThreadLocalRandom.current().nextInt(60, 100);
            
            
            xmlV2X.append(header).append("\n");
            xmlNoV2X.append(header).append("\n");
            routes = randomizeAndProduceRouteXML(pathsBeforeTheLastOne, generateRandomEmergencyVehicleRoute(), numVehicles);
            xmlV2X.append(routes).append("\n");
            xmlNoV2X.append(routes).append("\n");
            
            vehV2X = randomizeAndProduceVehiclesXML(numVehicles);
            vehNoV2X = vehV2X.replace("EmergencyVehicle", "JustSirenEmergencyVehicle");
            
            xmlV2X.append(vehV2X).append("\n");
            xmlNoV2X.append(vehNoV2X).append("\n");
            xmlV2X.append("</routes>");
            xmlNoV2X.append("</routes>");

            
            //com v2x
            file = new FileOutputStream("..//..//src//veins-4.7.1//examples//veins//withV2X.rou.xml");
            file.write(xmlV2X.toString().getBytes());
            file.close();
            
            
            //sem v2x
            file = new FileOutputStream("..//..//src//veins-4.7.1//examples//veins//withoutV2X.rou.xml");
            file.write(xmlNoV2X.toString().getBytes());
            file.close();
        } 
        catch (Exception e) 
        {
            e.printStackTrace();
        }
		
	}
	
	private static List<String> transformPaths(List<String> path)
	{
		List<String> newPaths = new ArrayList<String>();
		
		//substituir os primeiros pelo novo nodo onde começam
		String first = path.get(0);
		String last = path.get(path.size()-1);
		//substituit os últimos pelo último nodo onde acabam
		
		return newPaths;
		
	}
	
	private static List<String> getEmergencyVehiclePossiblePaths(List<String> paths)
    {
        int firstNode, lastNode;
        String[] campos;
        List<String> evPaths = new ArrayList<>();
        for(String path : paths)
        {
            campos = path.split(" ");
            firstNode = Integer.parseInt(campos[0].split("to")[0]);
            lastNode = Integer.parseInt(campos[campos.length-1].split("to")[1]);

            if((firstNode == 29 && lastNode == 32) ||(firstNode == 32 && lastNode == 29) ||(firstNode == 31 && lastNode == 30) ||(firstNode == 30 && lastNode == 31) )
            	evPaths.add(path);            		
        }
        return evPaths;
    }

    private static String produceHeader()
    {
        StringBuilder sb = new StringBuilder();
        sb.append("<routes>\n");
        sb.append("\t<vType accel=\"1.0\" decel=\"5.0\" id=\"NormalVehicle\" length=\"2.0\" maxSpeed=\"2.5\" sigma=\"0.0\"  type=\"NormalVehicle\"/>\n");
        sb.append("\t<vType accel=\"1.0\" decel=\"5.0\" id=\"JustKerbsVehicle\" length=\"2.0\" maxSpeed=\"2.5\" sigma=\"0.0\"  type=\"JustKerbsVehicle\"/>\n");
        sb.append("\t<vType accel=\"1.0\" decel=\"5.0\" id=\"EmergencyVehicle\" length=\"2.5\" maxSpeed=\"3.0\" sigma=\"0.0\" type=\"EmergencyVehicle\"/>\n");
        sb.append("\t<vType accel=\"1.0\" decel=\"5.0\" id=\"Bus\" length=\"6\" maxSpeed=\"1.1\" sigma=\"0.0\"/>\n");
        sb.append("\t<vType accel=\"1.0\" decel=\"5.0\" id=\"SlowVehicle\" length=\"2\" maxSpeed=\"0.2\" sigma=\"0.0\" type=\"SlowVehicle\"/>\n");
        sb.append("\t<vType accel=\"1.0\" decel=\"5.0\" id=\"AccidentVehicle\" length=\"2\" maxSpeed=\"2\" sigma=\"0.0\" type=\"AccidentVehicles\"/>\n");
        sb.append("\t<vType accel=\"1.0\" decel=\"5.0\" id=\"JustSirenEmergencyVehicle\" length=\"2.5\" maxSpeed=\"3.0\" sigma=\"0.0\" type=\"JustSirenEmergencyVehicle\"/>");
        return sb.toString();
    }


    private static String randomizeAndProduceRouteXML(List<String> paths, String pathEV, int numRoutes)
    {
        String route;
        int routeId, randomRoute;
        StringBuilder routes = new StringBuilder();
        
        //System.out.println("size do paths: " + paths.size());
        for(routeId = 0; routeId < numRoutes; routeId++)
        {
            randomRoute = ThreadLocalRandom.current().nextInt(0, paths.size() );
            route = "\t<route id=\""+ routeId +"\"  edges=\"" + paths.get(randomRoute) + "\"/>\n";
            routes.append(route);
        }
        //ev
        
        randomRoute = ThreadLocalRandom.current().nextInt(0, paths.size() + 1);
        route = "\t<route id=\""+ numRoutes +"\"  edges=\"" + pathEV + "\"/>\n";
        routes.append(route);

        return routes.toString();
    }

    private static String randomizeAndProduceVehiclesXML(int numVehicles)
    {
        Map<Integer, String> mapVehicles = new TreeMap<>();
        String vehicle;
        int departTime, idVehicle, randomRoute, randomType;
        String typeOfVehicle;
        StringBuilder vehicles = new StringBuilder();


        for( idVehicle = 0; idVehicle < numVehicles; idVehicle++)
        {
            randomRoute = ThreadLocalRandom.current().nextInt(0, numVehicles + 1);
            departTime = ThreadLocalRandom.current().nextInt(0, 2001);
            randomType = ThreadLocalRandom.current().nextInt(0, 10);

            
            if(randomType >= 0 && randomType <= 1)
            	typeOfVehicle = "JustKerbsVehicle";
            else if(randomType > 1 && randomType <= 2)
            	typeOfVehicle = "AccidentVehicle";
            else if(randomType > 2 && randomType <= 4)
            	typeOfVehicle = "SlowVehicle";
            else
            	typeOfVehicle = "NormalVehicle";

            vehicle = "\t<vehicle depart=\"" + departTime + "\" id=\"" + idVehicle + "\" route=\"" + randomRoute + "\" type=\"" + typeOfVehicle +"\"/>\n";
            mapVehicles.put(departTime, vehicle);
            
        }
        //adicionar o EV ao map
        
        departTime = ThreadLocalRandom.current().nextInt(0, 2001);
        typeOfVehicle = "EmergencyVehicle";

        
        vehicle = "\t<vehicle depart=\"" + departTime + "\" id=\"" + idVehicle + "\" route=\"" + numVehicles + "\" type=\"" + typeOfVehicle + "\"/>\n";
        mapVehicles.put(departTime, vehicle);

        for(Map.Entry<Integer, String> e : mapVehicles.entrySet())
            vehicles.append(e.getValue());
        
        return vehicles.toString();
    }
}
