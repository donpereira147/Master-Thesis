cd C://Users//PEB2BRG//src//veins-4.7.1//examples//veins
#alterar o RSUExampleScenario
sed -i 's/28/6/g' RSUExampleScenario.ned
sed -i 's/SocketRSU {/RSU {/g' RSUExampleScenario.ned
#mudar os campos do tese.launchd.xml
sed -i 's/tests.net/tese.net/g' tese.launchd.xml
sed -i 's/withV2X.rou/tese.rou/g' tese.launchd.xml
sed -i 's/withoutV2X.rou/tese.rou/g' tese.launchd.xml
sed -i 's/boards.rou/tese.rou/g' tese.launchd.xml
sed -i 's/tests.sumo/tese.sumo/g' tese.launchd.xml
sed -i 's/boards.sumo/tese.sumo/g' tese.launchd.xml
#correr o script em java que crie o omnetpp.ini adequado aos testes
"C:\Program Files\Java\jre8\bin\javaw.exe" -Dfile.encoding=Cp1252 -classpath "C:\Users\PEB2BRG\eclipse-workspace\OmnetGeneratorFewRSUs\bin" omnetfew.OmnetppIniGenerator
#mudar o script que mude as cenas do ev, estou cansado
cd "C:\Users\PEB2BRG\src\veins-4.7.1\src\veins\modules\application\traci"
sed -i 's/lotsOfTestsBeingRun = true/lotsOfTestsBeingRun = false/g' EmergencyVehicle.cc
sed -i 's/routingBig.xml/routingSmall.xml/g' EmergencyVehicle.cc
sed -i 's/fewRSUs = false/fewRSUs = true/g' RSU.cc

sed -i 's/fewTests = true/fewTests = false/g' AccidentVehicle.cc
sed -i 's/fewTests = true/fewTests = false/g' JustKerbsVehicle.cc
sed -i 's/fewTests = true/fewTests = false/g' NormalVehicle.cc
sed -i 's/fewTests = true/fewTests = false/g' SlowVehicle.cc

#compilar o projeto
cd "C:\Users\PEB2BRG\src\veins-4.7.1"
./configure
make


#muda para a diretoria do veins
cd C://Users//PEB2BRG//src//veins-4.7.1//examples//veins
#muda a linha do launchd.xml para os use cases
sed -i 's/withV2X/tese/g' tese.launchd.xml
#muda a linha do sumo.config para use cases
sed -i 's/withV2X/tese/g' tese.sumo.cfg

sed -i 's/boards/tese/g' tese.sumo.cfg
#muda a linha do launchd.xml para os use cases
sed -i 's/withoutV2X/tese/g' tese.launchd.xml
#muda a linha do sumo.config para use cases
sed -i 's/withoutV2X/tese/g' tese.sumo.cfg

sed -i 's/boards/tese/g' tese.sumo.cfg
#muda a linha do omnetpp.ini para o caso dos use cases
sed -i 's/\"WithoutVToXRSU\"/\"RSU\"/g' omnetpp.ini
#correr a simulação-->se der erro mudar o Qtenv para Cmdenv
../../../omnetpp-5.3/bin/opp_run.exe -m -u Qtenv -n .:../../src/veins --image-path=../../images -l ../../src/veins omnetpp.ini
