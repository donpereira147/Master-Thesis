import java.io.File;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class Main 
{
	 static int dim = 168;
	 static int[][] graph = new int[dim+1][dim+1];
	
	public static void main(String[] args)
	{
		/*
		int vert, hor;
		vert = 8;
		hor = 6;
		int[][] matriz = new int [6][];
		for(int i = 0; i < hor; i++)
			matriz[i] = new int[vert];
		
		int total = 1;
		for(int i = 0; i < hor; i++)
			for(int j = 0; j < vert; j++)
				matriz[i][j] = total++;
		
		
		int xInicial, yInicial;
		xInicial = yInicial = 30;
		
		/*for(int i = 0; i < hor; i++)
		{	
			for(int j = 0; j < vert; j++)
				System.out.print(matriz[i][j]+ " ");
			
			System.out.println();
		}*/	
		
		//horizontal
		
		for(int i = 1; i <= dim; i++)
        	for(int j = 1; j <= dim; j++)
        		graph[i][j] = 0;
		
		readXMLToGraph();
		
		for(int i = 0; i <= dim; i++)
		{
			
			for(int j = 0; j <= dim; j++)
			{
				
				if(graph[i][j] != 0)
				{
					if(i > 144 || j > 144)
						System.out.println("<route id=\"" + i + "to" + j + "\" xI =\"-100"  + "\" yI=\"-100"  + "\" xF =\"-100"  + "\" yF=\"-100" + "\" time=\"7.5\"/>");
					else
						System.out.println("<route id=\"" + i + "to" + j + "\" xI =\""  + "\" yI=\""  + "\" xF =\""  + "\" yF=\"" + "\" time=\"7.5\"/>");
				}
			
			}
				
		}
		/*
		
		//vertical
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 5; j++)
			{
				System.out.println("<route id=\"" + matriz[j][i] + "to" + matriz[j+1][i] + "\" xI =\"" + xInicial + "\" yI=\"" + yInicial + "\" xF =\"" + xInicial + "\" yF=\"" + (yInicial+250) + "\"/>");
				System.out.println("<route id=\"" + matriz[j+1][i] + "to" + matriz[j][i] + "\" xI =\"" + xInicial + "\" yI=\"" + (yInicial+250) + "\" xF =\"" + xInicial + "\" yF=\"" + yInicial + "\"/>");
				yInicial += 250;
				
			}
			xInicial +=250;
			yInicial = 30;
			System.out.println();
			
		}*/
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
}
