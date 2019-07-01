class MyMessage:

    nodeId = ''             # device
    childSensorId = ''      # sensor on the device
    command = ''
    ack = False
    messageType = ''
    payload = ''

    def toString(self):
        return "nodeId {}, childSensorId {}, command '{}', ack {}, message: {} '{}'".format(self.nodeId, self.childSensorId, self.command, self.ack, self.messageType, self.payload)
