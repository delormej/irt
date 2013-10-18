#!/usr/bin/env python
#################################################################
# python garmin object
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#

<<<<<<< HEAD
import itertools, sys, random, operator, datetime, time, usb
from antprotocol.bases import GarminANT, DynastreamANT
from antprotocol.protocol import ANTReceiveException

class Servo(object):
	"""Class to abstract interactions with servo motor.
	"""
	# Bus 001 Device 002: ID 0424:9512 Standard Microsystems Corp.

	def __init__(self, base = None):
		self.base = base

	def open(self, vid = None, pid = None):
		dev = usb.core.find(idVendor=0x0424, idProduct=0xEC00) #0x9512

		if dev is None:
			raise ValueError('Servo not found.')

		dev.set_configuration()

		# get an endpoint instance
		cfg = dev.get_active_configuration()
		interface_number = cfg[(0,0)].bInterfaceNumber
		alternate_setting = usb.control.get_interface(dev, interface_number)
		intf = usb.util.find_descriptor(
			cfg, bInterfaceNumber = interface_number,
			bAlternateSetting = alternate_setting
		)

		ep = usb.util.find_descriptor(
			intf,
			# match the first OUT endpoint
			custom_match = \
			lambda e: \
				usb.util.endpoint_direction(e.bEndpointAddress) == \
				usb.util.ENDPOINT_OUT
		)

		assert ep is not None

		# write the data
		#ep.write('test')		

	def turn0(self):
		print "sets resistance the motor to 0"

	def turn1(self):
		print "sets resistance the motor to 1"

=======
import itertools, sys, random, operator, datetime, time
from antprotocol.bases import GarminANT, DynastreamANT
from antprotocol.protocol import ANTReceiveException

>>>>>>> 347e9ae447830ede2d58af5a6123e8675e4957f3
class MyHRM(object):
    """Class to represent the Heart Rate Monitor device.
    """

    def __init__(self, base = None):
        self.base = base
        
    def init_ant(self):
       self.base.reset()   
       self.base.send_network_key(0, [0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45])
    
    def slave_connect(self):
        # ANT device initialization
       self.init_ant();
       
       # requests the capabilities
       self.base._send_message(0x54)
       x = self.base._receive_message()
       print x[3]
       
       # try getting serial # and version
       
       self.base.assign_channel()
       self.base.set_channel_frequency(0x39)
       
       self.base.set_search_timeout(0xFF)
       
       # Channel Id should be set with a device type (120) for HRM see {9.5.2.3 Set Channel ID (0x51)}
       # Default (0) seems to imply everything comes back which is probably ok for us right now? 
       #self.base.set_channel_id(0)
       print "sending my own set channel id"
       self.base._send_message(0x51, 0, 0, 0, 120, 0)
       self.base._check_ok_response()       

       #copied from ANTChannel.cpp ln 233: 8070 = 0x1F86, but sending in little endian format
       self.base.set_channel_period([0x86, 0x1F])
       
       self.base.open_channel()

       # try sending a request message to get the device type
       print "requesting the (0x51) channel, device number, device type, trans. type"
       self.base._send_message(0x4D, 0, 0x51)
       d = self.base._receive_message()
       print d[3] # this should include the device type?
       
    def master_connect(self):
        # Open a channel as a master sending power data
        self.init_ant()

        # Sending my own assign channel message
        self.base._send_message(0x42, 0x00, 0x10, 0x00)
        self.base._check_ok_response()

        # set channel id (device type = 11 (0x0B))
        self.base._send_message(0x51, 0x00, [0x40, 0x11], 0, 0x0B, 1)
        self.base._check_ok_response()       

        # set channel period = 8182
        self.base.set_channel_period([0xF6, 0x1F])

        # set rf frequency 57
        self.base.set_channel_frequency(0x39)

        # open channel
        self.base.open_channel()

        accumulated_power = 0
        update_event_count = 1
       
        for x in range(300):
            time.sleep(0.24969482421875)
            
            # page 0x10 - standard power only

            # generate random power
            power = random.randint(50,300)
            accumulated_power += power

            page_number = 0x10
            
            if update_event_count == 254:
                update_event_count = 1
            else:
                update_event_count += 1
                
            pedal_power = 0xff
            inst_cadence = 0xff
            accum_power = [0x00, 0x00]    
            inst_power =  [0x2d, 0x01]     

            # interleave every 121st message
            if x % 121 == 0:
                # every 121 messages required common data, manufacturer information
                self.base._send_message(0x50, 0xff, 0xff, 0x01, [0x00, 0xff], [0x00, 0x00])
                
            if x % 242 == 0:
                # every 121 messages required common data, product information
                self.base._send_message(0x51, 0xff, 0x0, 0x01, [0xff, 0xff])

            self.base._send_message(page_number, update_event_count, pedal_power, accum_power, inst_power)
        
    
    def wait_for_beacon(self):
        # device initialization
        print "Waiting for receive"
        
        for tries in range(100):
            try:
                d = self.base._receive_message()
                #if d[2] == 0x40:
                    #clear wait
                #   print "OK to stop waiting."
                #   return
            except Exception:
                pass
        
def main():
    base = GarminANT(debug=True)
    if not base.open():
        print "No devices connected!"
        return 1

    print "ANT open."
        
    device = MyHRM(base)
    
<<<<<<< HEAD
    device.slave_connect()
    #device.master_connect()
    device.wait_for_beacon()
=======
    device.master_connect()
    #device.wait_for_beacon()
>>>>>>> 347e9ae447830ede2d58af5a6123e8675e4957f3
    #d = base._receive_message()
    #d1 = base._receive_message()
    
    base.close()
    return 0

if __name__ == '__main__':
    sys.exit(main())
