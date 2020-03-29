package randompathgenerator;


import java.io.FileOutputStream;

public class OmnetppIniGenerator 
{
	public static void generateOmnetppInit(boolean v2x)
	{
		String typeOfRSU;
		String header = "[General]\ncmdenv-express-mode = true\ncmdenv-autoflush = true\ncmdenv-status-frequency = 1s\n**.cmdenv-log-level = info\nned-path = .\nimage-path = ../../images\nnetwork = RSUExampleScenario\n";
		String simPar = "debug-on-errors = true\r\n" + 
				"print-undisposed = true\r\n" + 
				"\r\n" + 
				"sim-time-limit = 20000s\r\n" + 
				"\r\n" + 
				"**.scalar-recording = true\r\n" + 
				"**.vector-recording = true\r\n" + 
				"\r\n" + 
				"**.debug = false\r\n" + 
				"**.coreDebug = false\r\n" + 
				"\r\n" + 
				"*.playgroundSizeX = 2500m\r\n" + 
				"*.playgroundSizeY = 2500m\r\n" + 
				"*.playgroundSizeZ = 50m";
		String otherPar = "\r\n" + 
				"##########################################################\r\n" + 
				"# Annotation parameters                                  #\r\n" + 
				"##########################################################\r\n" + 
				"*.annotations.draw = true\r\n" + 
				"\r\n" + 
				"##########################################################\r\n" + 
				"# Obstacle parameters                                    #\r\n" + 
				"##########################################################\r\n" + 
				"#*.obstacles.debug = false\r\n" + 
				"#*.obstacles.obstacles = xmldoc(\"config.xml\", \"//AnalogueModel[@type='SimpleObstacleShadowing']/obstacles\")";
		
		
		
		String someMoreThings = "\r\n" + 
				"##########################################################\r\n" + 
				"#            TraCIScenarioManager parameters             #\r\n" + 
				"##########################################################\r\n" + 
				"*.manager.updateInterval = 1s\r\n" + 
				"*.manager.host = \"localhost\"\r\n" + 
				"*.manager.port = 9999\r\n" + 
				"*.manager.autoShutdown = true\r\n" + 
				"*.manager.launchConfig = xmldoc(\"erlangen.launchd.xml\")\r\n" + 
				"\r\n" + 
				"\r\n" + 
				"\r\n" + 
				"##########################################################\r\n" + 
				"#                       RSU SETTINGS                     #\r\n" + 
				"#                                                        #\r\n" + 
				"#                                                        #\r\n" + 
				"##########################################################\r\n" + 
				"*.rsu[0].mobility.x = 270.05\r\n" + 
				"*.rsu[0].mobility.y = 288.45\r\n" + 
				"*.rsu[0].mobility.z = 3\r\n" + 
				"*.rsu[1].mobility.x = 520.05\r\n" + 
				"*.rsu[1].mobility.y = 288.45\r\n" + 
				"*.rsu[1].mobility.z = 3\r\n" + 
				"\r\n" + 
				"*.rsu[2].mobility.x = 770.05\r\n" + 
				"*.rsu[2].mobility.y = 288.45\r\n" + 
				"*.rsu[2].mobility.z = 3\r\n" + 
				"*.rsu[3].mobility.x = 520.05\r\n" + 
				"*.rsu[3].mobility.y = 514.85\r\n" + 
				"*.rsu[3].mobility.z = 1\r\n" + 
				"*.rsu[4].mobility.x = 779.95\r\n" + 
				"*.rsu[4].mobility.y = 514.85\r\n" + 
				"*.rsu[4].mobility.z = 1\r\n" + 
				"*.rsu[5].mobility.x = 1029.95\r\n" + 
				"*.rsu[5].mobility.y = 288.45\r\n" + 
				"*.rsu[5].mobility.z = 1";
		
		if(v2x)
			typeOfRSU = "\n*.rsu[*].applType = \"RSU\"\n";
		else
			typeOfRSU = "\n*.rsu[*].applType = \"WithoutVToXRSU\"\n";
		
		String restOfTheFile = "*.rsu[*].appl.headerLength = 80 bit\r\n" + 
				"*.rsu[*].appl.sendBeacons = false\r\n" + 
				"*.rsu[*].appl.dataOnSch = false\r\n" + 
				"*.rsu[*].appl.beaconInterval = 1s\r\n" + 
				"*.rsu[*].appl.beaconUserPriority = 7\r\n" + 
				"*.rsu[*].appl.dataUserPriority = 5\r\n" + 
				"\r\n" + 
				"##########################################################\r\n" + 
				"#            11p specific parameters                     #\r\n" + 
				"#                                                        #\r\n" + 
				"#                    NIC-Settings                        #\r\n" + 
				"##########################################################\r\n" + 
				"*.connectionManager.sendDirect = true\r\n" + 
				"*.connectionManager.maxInterfDist = 2600m\r\n" + 
				"*.connectionManager.drawMaxIntfDist = false\r\n" + 
				"\r\n" + 
				"*.**.nic.mac1609_4.useServiceChannel = false\r\n" + 
				"\r\n" + 
				"*.**.nic.mac1609_4.txPower = 20mW\r\n" + 
				"*.**.nic.mac1609_4.bitrate = 6Mbps\r\n" + 
				"*.**.nic.phy80211p.sensitivity = -89dBm\r\n" + 
				"\r\n" + 
				"*.**.nic.phy80211p.useThermalNoise = true\r\n" + 
				"*.**.nic.phy80211p.thermalNoise = -110dBm\r\n" + 
				"\r\n" + 
				"*.**.nic.phy80211p.decider = xmldoc(\"config.xml\")\r\n" + 
				"*.**.nic.phy80211p.analogueModels = xmldoc(\"config.xml\")\r\n" + 
				"*.**.nic.phy80211p.usePropagationDelay = true\r\n" + 
				"\r\n" + 
				"*.**.nic.phy80211p.antenna = xmldoc(\"antenna.xml\", \"/root/Antenna[@id='monopole']\")\r\n" + 
				"\r\n" + 
				"##########################################################\r\n" + 
				"#                    WaveAppLayer                        #\r\n" + 
				"##########################################################\r\n" + 
				"*.manager.moduleType = \"EmergencyVehicle=org.car2x.veins.nodes.Car JustSirenEmergencyVehicle=org.car2x.veins.nodes.Car  AccidentVehicle=org.car2x.veins.nodes.Car SlowVehicle=org.car2x.veins.nodes.Car JustKerbsVehicle=org.car2x.veins.nodes.Car *=org.car2x.veins.nodes.Car\"\r\n" + 
				"*.manager.moduleName=\"EmergencyVehicle=emergency JustSirenEmergencyVehicle=siren AccidentVehicle=accident SlowVehicle=slow JustKerbsVehicle=kerbs *=node\"\r\n" + 
				"\r\n" + 
				"*.emergency[*].applType = \"EmergencyVehicle\"\r\n" + 
				"*.emergency[*].appl.headerLength = 80 bit\r\n" + 
				"*.emergency[*].appl.sendBeacons = false\r\n" + 
				"*.emergency[*].appl.dataOnSch = false\r\n" + 
				"*.emergency[*].appl.beaconInterval = 1s\r\n" + 
				"\r\n" + 
				"*.siren[*].applType = \"JustSirenEmergencyVehicle\"\r\n" + 
				"*.siren[*].appl.headerLength = 80 bit\r\n" + 
				"*.siren[*].appl.sendBeacons = false\r\n" + 
				"*.siren[*].appl.dataOnSch = false\r\n" + 
				"*.siren[*].appl.beaconInterval = 1s\r\n" + 
				"\r\n" + 
				"*.accident[*].applType = \"AccidentVehicle\"\r\n" + 
				"*.accident[*].appl.headerLength = 80 bit\r\n" + 
				"*.accident[*].appl.sendBeacons = false\r\n" + 
				"*.accident[*].appl.dataOnSch = false\r\n" + 
				"*.accident[*].appl.beaconInterval = 1s\r\n" + 
				"\r\n" + 
				"*.slow[*].applType = \"SlowVehicle\"\r\n" + 
				"*.slow[*].appl.headerLength = 80 bit\r\n" + 
				"*.slow[*].appl.sendBeacons = false\r\n" + 
				"*.slow[*].appl.dataOnSch = false\r\n" + 
				"*.slow[*].appl.beaconInterval = 1s\r\n" + 
				"\r\n" + 
				"*.kerbs[*].applType = \"JustKerbsVehicle\"\r\n" + 
				"*.kerbs[*].appl.headerLength = 80 bit\r\n" + 
				"*.kerbs[*].appl.sendBeacons = false\r\n" + 
				"*.kerbs[*].appl.dataOnSch = false\r\n" + 
				"*.kerbs[*].appl.beaconInterval = 1s\r\n" + 
				"\r\n" + 
				"*.node[*].applType = \"NormalVehicle\"\r\n" + 
				"*.node[*].appl.headerLength = 80 bit\r\n" + 
				"*.node[*].appl.sendBeacons = false\r\n" + 
				"*.node[*].appl.dataOnSch = false\r\n" + 
				"*.node[*].appl.beaconInterval = 1s\r\n" + 
				"\r\n" + 
				"##########################################################\r\n" + 
				"#                      Mobility                          #\r\n" + 
				"##########################################################\r\n" + 
				"*.emergency[*].veinsmobilityType.debug = true\r\n" + 
				"*.emergency[*].veinsmobility.x = 0\r\n" + 
				"*.emergency[*].veinsmobility.y = 0\r\n" + 
				"*.emergency[*].veinsmobility.z = 1.895\r\n" + 
				"\r\n" + 
				"*.siren[*].veinsmobilityType.debug = true\r\n" + 
				"*.siren[*].veinsmobility.x = 0\r\n" + 
				"*.siren[*].veinsmobility.y = 0\r\n" + 
				"*.siren[*].veinsmobility.z = 1.895\r\n" + 
				"\r\n" + 
				"*.accident[*].veinsmobilityType.debug = true\r\n" + 
				"*.accident[*].veinsmobility.x = 0\r\n" + 
				"*.accident[*].veinsmobility.y = 0\r\n" + 
				"*.accident[*].veinsmobility.z = 1.895\r\n" + 
				"\r\n" + 
				"*.slow[*].veinsmobilityType.debug = true\r\n" + 
				"*.slow[*].veinsmobility.x = 0\r\n" + 
				"*.slow[*].veinsmobility.y = 0\r\n" + 
				"*.slow[*].veinsmobility.z = 1.895\r\n" + 
				"\r\n" + 
				"*.kerbs[*].veinsmobilityType.debug = true\r\n" + 
				"*.kerbs[*].veinsmobility.x = 0\r\n" + 
				"*.kerbs[*].veinsmobility.y = 0\r\n" + 
				"*.kerbs[*].veinsmobility.z = 1.895\r\n" + 
				"\r\n" + 
				"*.node[*].veinsmobilityType.debug = true\r\n" + 
				"*.node[*].veinsmobility.x = 0\r\n" + 
				"*.node[*].veinsmobility.y = 0\r\n" + 
				"*.node[*].veinsmobility.z = 1.895\r\n" + 
				"\r\n" + 
				"\r\n" + 
				"#*.accident[1].veinsmobility.accidentCount = 1\r\n" + 
				"#*.accident[1].veinsmobility.accidentStart = 160s\r\n" + 
				"#*.accident[1].veinsmobility.accidentDuration = 20000s\r\n" + 
				"*.accident[*].veinsmobility.accidentCount = 1\r\n" + 
				"*.accident[*].veinsmobility.accidentStart = 60s\r\n" + 
				"*.accident[*].veinsmobility.accidentDuration = 20000s\r\n" + 
				"\r\n" + 
				"[Config Default]\r\n" + 
				"\r\n" + 
				"[Config WithBeaconing]\r\n" + 
				"*.rsu[*].appl.sendBeacons = true\r\n" + 
				"*.node[*].appl.sendBeacons = true\r\n" + 
				"\r\n" + 
				"[Config WithChannelSwitching]\r\n" + 
				"*.**.nic.mac1609_4.useServiceChannel = true\r\n" + 
				"*.node[*].appl.dataOnSch = true\r\n" + 
				"*.rsu[*].appl.dataOnSch = true\r\n" + 
				"\r\n" + 
				"";
		
		StringBuilder sb = new StringBuilder();
		sb.append(header)
			.append(simPar)
			.append(otherPar)
			.append(someMoreThings)
			.append(typeOfRSU)
			.append(restOfTheFile);
		
		FileOutputStream out;
		try {
			out = new FileOutputStream("omnetpp.ini");
			out.write(sb.toString().getBytes());
	        out.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
        
        
		
		System.out.println(sb.toString());
	}
}
