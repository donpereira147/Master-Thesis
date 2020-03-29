#!/bin/bash
#ciclo for i < 100
cd C://Users//PEB2BRG//src//veins-4.7.1//examples//veins
#alterar o RSUExampleScenario
sed -i 's/6/28/g' RSUExampleScenario.ned
sed -i 's/SocketRSU {/RSU {/g' RSUExampleScenario.ned
#mudar os campos do tese.launchd.xml
sed -i 's/tese.net/tests.net/g' tese.launchd.xml
sed -i 's/tese.rou/withV2X.rou/g' tese.launchd.xml
sed -i 's/boards.rou/withV2X.rou/g' tese.launchd.xml
sed -i 's/tese.sumo/tests.sumo/g' tese.launchd.xml
sed -i 's/boards.sumo/tests.sumo/g' tese.launchd.xml
#correr o script em java que crie o omnetpp.ini adequado aos testes
"C:\Program Files\Java\jre8\bin\javaw.exe" -Dfile.encoding=Cp1252 -classpath "C:\Users\PEB2BRG\eclipse-workspace\OmnetGeneratorLotsOfRSUs\bin" omnetlots.OmnetppIniGenerator
#mudar o script que mude as cenas do ev, estou cansado
cd "C:\Users\PEB2BRG\src\veins-4.7.1\src\veins\modules\application\traci"
sed -i 's/lotsOfTestsBeingRun = false/lotsOfTestsBeingRun = true/g' EmergencyVehicle.cc
sed -i 's/routingSmall.xml/routingBig.xml/g' EmergencyVehicle.cc
sed -i 's/fewRSUs = true/fewRSUs = false/g' RSU.cc

sed -i 's/fewTests = false/fewTests = true/g' AccidentVehicle.cc
sed -i 's/fewTests = false/fewTests = true/g' JustKerbsVehicle.cc
sed -i 's/fewTests = false/fewTests = true/g' NormalVehicle.cc
sed -i 's/fewTests = false/fewTests = true/g' SlowVehicle.cc
#compilar o projeto
cd "C:\Users\PEB2BRG\src\veins-4.7.1"
./configure
make

x=1
while [ $x -le 100 ]; do
	#mudar para a diretoria onde dá para correr o java
	cd "C:\Users\PEB2BRG\eclipse-workspace\PathGenerator"
	#correr o java para criar os dois ficheiros aleatórios
	"C:\Program Files\Java\jre8\bin\javaw.exe" -Dfile.encoding=Cp1252 -classpath "C:\Users\PEB2BRG\eclipse-workspace\PathGenerator\bin" randompathgenerator.RandomPathGenerator
	#muda para a diretoria do veins
	cd C://Users//PEB2BRG//src//veins-4.7.1//examples//veins
	#muda a linha do launchd.xml para o caso v2x
	sed -i 's/withoutV2X/withV2X/g' tese.launchd.xml
	sed -i 's/boards/withV2X/g' tese.launchd.xml
	sed -i 's/"tese.rou.xml"/"withV2X.rou.xml"/g' tese.launchd.xml
	#muda a linha do sumo.config para o caso v2x
	sed -i 's/withoutV2X/withV2X/g' tests.sumo.cfg
	#sed -i 's/boards/withV2X/g' tests.sumo.cfg
	#sed -i 's/"tese.rou.xml"/"tests.rou.xml"/g' tests.sumo.cfg
	#muda a linha do omnetpp.ini para o caso V2X
	sed -i 's/\"WithoutVToXRSU\"/\"RSU\"/g' omnetpp.ini
	#correr a simulação
	../../../omnetpp-5.3/bin/opp_run.exe -m -u Cmdenv -n .:../../src/veins --image-path=../../images -l ../../src/veins omnetpp.ini
	#muda a linha do launchd.xml para o caso sem v2x
	sed -i 's/withV2X/withoutV2X/g' tese.launchd.xml
	sed -i 's/boards/withoutV2X/g' tese.launchd.xml
	sed -i 's/"tese.rou.xml"/"withoutV2X.rou.xml"/g' tese.launchd.xml
	#muda a linha do sumo.config para o caso v2x
	sed -i 's/withV2X/withoutV2X/g' tests.sumo.cfg
	#sed -i 's/boards/withoutV2X/g' tests.sumo.cfg
	#sed -i 's/"tese.rou.xml"/"withoutV2X.rou.xml"/g' tests.sumo.cfg
	#muda a linha do omnetpp.ini para o caso sem V2X
	sed -i 's/\"RSU\"/\"WithoutVToXRSU\"/g' omnetpp.ini
	#correr a simulação novamente
	../../../omnetpp-5.3/bin/opp_run.exe -m -u Cmdenv -n .:../../src/veins --image-path=../../images -l ../../src/veins omnetpp.ini
	echo $x >> "testResults.txt"
	echo "----------------------" >> "testResults.txt"
	x=$(( $x + 1 ))
done