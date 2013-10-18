#!/usr/bin/python

import usb.core
import usb.util

# this is a half-hearted attempt to fake up a pySerial interface for
# the ANT USB2 stick
 
# it is good enough that the ANT USB2 is interchangeable with AP1
# sticks for the Ant-over-IP server for Quarq production test scripts 

# requires pyUsb 1.0 or newer.  http://pyusb.sf.net

# Mark Rages, August 2010
# markrages@gmail.com

class FakeSerial:
    def __init__(self):
        self.baudrate=115200
        self.readbuf=[]
        self.timeout=1.0

        # find our device
        dev = usb.core.find(idVendor=0x0fcf, idProduct=0x1008)
        
        # was it found?
        if dev is None:
            raise ValueError('Device not found')

        # get an endpoint instance
        self.ep_out = usb.util.find_descriptor(
            dev.get_interface_altsetting(),   # first interface
            # match the first OUT endpoint
            custom_match = \
                lambda e: \
                usb.util.endpoint_direction(e.bEndpointAddress) == \
                usb.util.ENDPOINT_OUT
            )
        self.ep_in = usb.util.find_descriptor(
            dev.get_interface_altsetting(),   # first interface
            # match the first OUT endpoint
            custom_match = \
                lambda e: \
                usb.util.endpoint_direction(e.bEndpointAddress) == \
                usb.util.ENDPOINT_IN
            )

    def setBaudrate(self, baudrate): self.baudrate=baudrate
    def getBaudrate(self): return self.baudrate
    def getTimeout(self): return self.timeout

    def write(self, data):
        # write the data
        self.ep_out.write(data)

    def read(self, length):
        try:
            while len(self.readbuf) < length:
                self.readbuf=self.readbuf+list(self.ep_in.read(64, timeout=int(1000*self.timeout)))
        except usb.core.USBError: # would be better to match the timeout error only
            pass
        
        ret=self.readbuf[:length]
        self.readbuf=self.readbuf[length:]
        return "".join(chr(x) for x in ret) 

    def setTimeout(self, timeout): 
        self.timeout=timeout
    def setDTR(self, state=0): pass
    def setRTS(self, state=0): pass
    def flush(self): pass
    def flushInput(self): pass

if __name__=="__main__":
    fs=FakeSerial()
    # send an ant message
    fs.write("\xa2\x02\0x4d\x00\x54\xbf\x00\x00")
    while 1:
        c=fs.read(1)
        if c=='':
            break
        print "%02x"%ord(c),
    print
        

