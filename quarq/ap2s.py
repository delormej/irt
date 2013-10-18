#!/usr/bin/python

# simple pyUsb interface to ANT USB2 stick. 
# requires pyUsb 1.0 or newer.  http://pyusb.sf.net

# Mark Rages, August 2010
# markrages@gmail.com

import usb.core
import usb.util

# find our device
dev = usb.core.find(idVendor=0x0fcf, idProduct=0x1008)
        
# was it found?
if dev is None:
    raise ValueError('Device not found')

# get an endpoint instance
ep_out = usb.util.find_descriptor(
    dev.get_interface_altsetting(),   # first interface
    # match the first OUT endpoint
    custom_match = \
        lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_OUT
    )
ep_in = usb.util.find_descriptor(
    dev.get_interface_altsetting(),   # first interface
    # match the first OUT endpoint
    custom_match = \
        lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_IN
    )

# send an ant message
ep_out.write("\xa2\x02\0x4d\x00\x54\xbf\x00\x00")
# read an ant message
print " ".join(("%02x"%x for x in ep_in.read(64)))

