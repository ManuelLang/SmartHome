from messageParser import *

# mysensors-out/254/1/1/0/16 0 1
# mysensors-out/177/255/0/0/17 0 2.1.1
# mysensors-out/177/255/3/0/6 0 0
# mysensors-out/177/255/3/0/11 0 MockMySensors
# mysensors-out/177/255/3/0/12 0 v0.5
# mysensors-out/177/22/0/0/24 0 Dust Level
# mysensors-out/177/22/1/0/37 0 12
# mysensors-out/177/19/0/0/20 0 Univeral Command
# mysensors-out/177/19/1/0/32 0 0
# mysensors-out/177/19/1/0/33 0 0


# TSP:MSG:SEND 122-122-0-0 s=8,c=1,t=1,pt=4,l=4,sg=0,ft=0,st=ok:75
# mysensors-out/122/8/1/0/1 0 75
class Programm:

    @staticmethod
    def main():
        parser = MessageParser()
        parser.parseFromMqtt("254/1/1/0/16 1")
        print("")
        parser.parseFromMqtt("177/255/0/0/17 2.1.1")
        print("")
        parser.parseFromMqtt("177/255/3/0/6 0")
        print("")
        parser.parseFromMqtt("177/255/3/0/11 MockMySensors")
        print("")
        parser.parseFromMqtt("177/255/3/0/12 v0.5")
        print("")
        parser.parseFromMqtt("177/22/0/0/24 Dust Level")
        print("")
        parser.parseFromMqtt("177/22/1/0/37 12")
        print("")
        parser.parseFromMqtt("177/19/0/0/20 Univeral Command")

        parser.buildMessageSet(122, 7, MessageParserSetReq.V_HUM, 59)

Programm.main()