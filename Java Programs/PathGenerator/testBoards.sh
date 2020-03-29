#muda para a diretoria do veins
cd C://Users//PEB2BRG//src//veins-4.7.1//examples//veins

sed -i 's/28/6/g' RSUExampleScenario.ned
sed -i 's/ RSU {/ SocketRSU {/g' RSUExampleScenario.ned
#mudar os campos do tese.launchd.xml
sed -i 's/tests.net/tese.net/g' tese.launchd.xml
sed -i 's/withV2X.rou/boards.rou/g' tese.launchd.xml
sed -i 's/withoutV2X.rou/boards.rou/g' tese.launchd.xml
sed -i 's/tese.rou/boards.rou/g' tese.launchd.xml
sed -i 's/tests.sumo/boards.sumo/g' tese.launchd.xml
sed -i 's/tese.sumo/boards.sumo/g' tese.launchd.xml
#correr o script em java que crie o omnetpp.ini adequado aos testes
"C:\Program Files\Java\jre8\bin\javaw.exe" -Dfile.encoding=Cp1252 -classpath "C:\Users\PEB2BRG\eclipse-workspace\OmnetGeneratorSockets\bin" sockets.OmnetppIniGenerator
#mudar o script que mude as cenas do ev, estou cansado
cd "C:\Users\PEB2BRG\src\veins-4.7.1\src\veins\modules\application\traci"
sed -i 's/lotsOfTestsBeingRun = true/lotsOfTestsBeingRun = false/g' EmergencyVehicle.cc
sed -i 's/routingBig.xml/routingSmall.xml/g' EmergencyVehicle.cc

#compilar o projeto
cd "C:\Users\PEB2BRG\src\veins-4.7.1"
./configure
make

cd "C://Users//PEB2BRG//src//veins-4.7.1//examples//veins"

#correr a simulação-->se der erro mudar o Qtenv para Cmdenv
../../../omnetpp-5.3/bin/opp_run.exe -m -u Cmdenv -n .:../../src/veins --image-path=../../images -l ../../src/veins omnetpp.ini
