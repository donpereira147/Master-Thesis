/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package randompathgenerator;


import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;
import java.io.File;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;
import java.util.concurrent.ThreadLocalRandom;

//modificado deste exemplo: https://medium.com/@bharatkulratan/print-all-the-paths-between-two-nodes-in-a-graph-89aee57544a4
class PathBetweenTwoNodes{
    static int dim = 168; // dim is number of nodes in graph

    static boolean[] color = new boolean[dim + 1];
    
    //graph is given in 2-D matrix form
    static int[][] graph = new int[dim+1][dim+1];
    
    // this list will store nodes on the path from source to destination
    static List<Integer> list = new ArrayList<>();
    
    // this is the size of "list" declared above
    // size is used to remember index of the node in the list to be removed
    // when  the node is marked unvisited
    static int size;
    
    
    public static List<String> manyManyPaths(int numVeh) throws IOException
    {
        List<String> paths = new ArrayList<>();
        
        
        for(int i = 1; i <= dim; i++)
        	for(int j = 1; j <= dim; j++)
        		graph[i][j] = 0;
        
        readXMLToGraph();
        
        
        for(int i = 0; i < numVeh; i++)
        {
        	String path = generateRandomVehicleRoute();
        	
        	if(!path.equals(""))
        		paths.add(path);
        	else i--;
        }
        	
        
        return paths;
    }
    
    public static String generateRandomEmergencyVehicleRoute()
    {
    	String path = "";
    	for(int i = 1; i <= dim; i++)
        	for(int j = 1; j <= dim; j++)
        		graph[i][j] = 0;
        
        readXMLToGraph();
        
        while((path = generateEmergencyVehicleRoute()).equals(""));
    	
    	return path;
    }
    
    public static String generateEmergencyVehicleRoute()
    {
    	List<Integer> nodes = new ArrayList<>();
    	
    	String path = "";
    	List<Integer> allPossibleNodes;
    	
    	List<Integer> initials = new ArrayList<>();
    	initials.add(145);
    	initials.add(161);
    	initials.add(152);
    	initials.add(168);
    	
    	Random r = new Random();
    	int initialNode = initials.get(r.nextInt(initials.size()));
    	int lastNode = 152;
    	
    	if(initialNode == 145)
    		lastNode = 168;
    	else if(initialNode == 152)
    		lastNode = 161;
    	if(initialNode == 168)
    		lastNode = 145;
    	else if(initialNode == 161)
    		lastNode = 152;
    	
    	nodes.add(initialNode);
    	
    	int currentNode = initialNode;
    	
    	
    	boolean firstTime = false;
    	while(currentNode < 145|| !firstTime)
    	{
    		firstTime = true;
    		
    		allPossibleNodes = getAllPossibleNodesForCurrentNodeEmergencyVehicle(currentNode, initialNode, lastNode, nodes);
    		if(allPossibleNodes.size() == 0)
    			return "";
    		int nextNode = allPossibleNodes.get(ThreadLocalRandom.current().nextInt(0, allPossibleNodes.size()));
    		
    		nodes.add(nextNode);
    		currentNode = nextNode;	
    	}
    	path = nodes.get(0) + "to" + nodes.get(1);
    	
    	for(int i = 1; i < nodes.size()-1; i++)
    		path += " " + nodes.get(i) + "to" + nodes.get(i + 1);

    	return path;
    	
    	
    }
    
    private static List<Integer> getAllPossibleNodesForCurrentNodeEmergencyVehicle(int current, int initial, int end, List<Integer> nodes)
    {
    	List<Integer> all = new ArrayList<>();
    	
    	for(int j = 1; j <= dim; j++)
    		if(graph[current][j] == 1)
    		{	
    			
    			if((initial == 145 && j == end) || (initial == 152 && j == end) || (initial == 161 && j == end) || (initial == 168 && j == end))
    			{
    				all = new ArrayList<>();
    				all.add(j);
    				return all;
    			}
    			else if(!nodes.contains(j) && j < 145)
	    		{
	    			//System.out.print(j + ",");
	    			all.add(j);
	    		}
    		}
    	//System.out.println();
    	return all;
    }
    
    
    public static String generateRandomVehicleRoute()
    {
    	List<Integer> nodes = new ArrayList<>();
    	
    	String path = "";
    	List<Integer> allPossibleNodes;
    	int initialNode = ThreadLocalRandom.current().nextInt(145, 168);
    	int lastNode = ThreadLocalRandom.current().nextInt(145, 168);
    	
    	while(initialNode == lastNode)
    		lastNode = ThreadLocalRandom.current().nextInt(145, 168);
    	
    	//System.out.println("initial: " + initialNode);
    	//System.out.println("last"
    		//	+ ": " + lastNode);
    	
    	nodes.add(initialNode);
    	
    	int currentNode = initialNode;
    	
    	
    	boolean firstTime = false;
    	while(currentNode < 145|| !firstTime)
    	{
    		firstTime = true;
    		
    		allPossibleNodes = getAllPossibleNodesForCurrentNode(currentNode, nodes);
    		if(allPossibleNodes.size() == 0)
    			return "";
    		int nextNode = allPossibleNodes.get(ThreadLocalRandom.current().nextInt(0, allPossibleNodes.size()));
    		
    		nodes.add(nextNode);
    		currentNode = nextNode;
    		//System.out.println("current:" + currentNode + ", firstTime: " + firstTime);
    		
    		
    	}
    	
    //	for(Integer i : nodes)
    	//	System.out.print(i + ",");
    	
    	//System.out.println();
    	
    	
    	path = nodes.get(0) + "to" + nodes.get(1);
    	
    	for(int i = 1; i < nodes.size()-1; i++)
    		path += " " + nodes.get(i) + "to" + nodes.get(i + 1);
    	
    	//System.out.println("path:" + path);
    
    	return path;
    }
    
    private static List<Integer> getAllPossibleNodesForCurrentNode(int current, List<Integer> nodes)
    {
    	List<Integer> all = new ArrayList<>();
    	
    	for(int j = 1; j <= dim; j++)
    		if(graph[current][j] == 1)
    		{	
    			if(!nodes.contains(j))
	    		{
	    			//System.out.print(j + ",");
	    			all.add(j);
	    		}
    		}
    	//System.out.println();
    	return all;
    }
    
    public static List<String> pathsBetweenTwoNodes() throws IOException
    {
        int i, j;
        List<String> paths = new ArrayList<>();
        BufferedWriter writer = new BufferedWriter(new FileWriter("allPossiblePaths.txt"));
        
        for(i = 1; i <= dim; i++)
        	for(j = 1; j <= dim; j++)
        		graph[i][j] = 0;
        
        
        
        
        /*BufferedReader br = new BufferedReader (new FileReader("nodes.txt"));
        for (i = 1; i <= dim; i++) 
        {
			
            String[] values = br.readLine().split(" ");
            int len = values.length;
            
            for (j = 1; j <= len; j++) 
                graph[i][j] = Integer.parseInt(values[j - 1]);
        }*/
        
        readXMLToGraph();

        /*for(i = 1; i <= dim; i++)
        {
        	for(j = 1; j <= dim; j++)
        		System.out.print(graph[i][j] + " ");
        	System.out.println();
        }*/
        /*
        Arrays.fill(color, false);// initially all are unvisited
        dfs(145, 160, paths, writer);
        size--;
        list.remove(size);
        */
        
      /*  for(i = 145; i <= dim; i++)
        {
            Arrays.fill(color, false);// initially all are unvisited
            for(j = 145 + 1; j <= dim; j++)
            {
                if(i != j)
                {
                    Arrays.fill(color, false);// initially all are unvisited
                    dfs(i, j, paths, writer);
                    size--;
                    list.remove(size);

                }
            }
            
        }
        for(i = 145; i <= dim; i++)
        {
        	Arrays.fill(color, false);// initially all are unvisited
        	dfs(i, 145, paths, writer);
        	size--;
        	list.remove(size);
        	
        }
        //br.close();
        writer.close();*/
        return paths;

    }

    private static void dfs(int src, int dst, /*List<String> paths,*/ BufferedWriter writer) throws IOException 
    {
	//node added to path
    	//System.out.println(src);
        list.add(src);
        size++;
        
        //node marked as visited
        color[src] = true;
        
        //when the destination node is found
        if (src == dst) 
        {
            String path = "";
            System.out.println("CUNT\n");
            for(int x = 0; x < list.size() - 1; x++)
                path += list.get(x) + "to" + list.get(x + 1) + " ";
            //System.out.println(path);
            //paths.add(path);
            writer.write(path + "\n");
            return;
        }

        for (int I = 1; I <= dim; I++) 
        {
            // if there's an edge between  src and node
            if (graph[src][I] == 1) 
            {
                //and that node is not visited yet
                if (color[I] == false) 
                {

                    //start dfs from that node
                    dfs(I, dst, /*path*/ writer);

                    //This line is critical to understand it marks the node unvisited which we have just visited so that the dfs could find another path to destination
                    color[I] = false;

                    //size of list reduced by 1 as the node is marked unvisited hence removed from the list
                    size--;

                    //remove that node at index "size" from list
                    list.remove(size);
                }
            }
        }
    }
    
    private static void readXMLToGraph()
    {
    	String [] campos;
    	try
    	{
    		File fXmlFile = new File("my_edge.edg.xml");
    		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
    		DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
    		Document doc = dBuilder.parse(fXmlFile);
    				

    		doc.getDocumentElement().normalize();
    		
    		NodeList nList = doc.getElementsByTagName("edge");
    		
    		for (int temp = 0; temp < nList.getLength(); temp++) 
    		{
    			Node nNode = nList.item(temp);
    			if (nNode.getNodeType() == Node.ELEMENT_NODE) 
    			{

    				Element eElement = (Element) nNode;
    				//System.out.println("route id : " + eElement.getAttribute("id"));
    				campos = eElement.getAttribute("id").split("to");
    				
    				graph[Integer.parseInt(campos[0])][Integer.parseInt(campos[1])] = 1;
    			}
    		}
    	}
    	catch(Exception e)
    	{
    		e.printStackTrace();
    	}
    }
    
    
    public static void createFileWithEVPaths()
    {
    	try
    	{
    		
		} catch (Exception e) {
			e.printStackTrace();
		}
    	
    }
}