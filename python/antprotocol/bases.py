from .protocol import ANTReceiveException
from .libusb import ANTlibusb
import usb

class GarminANT(ANTlibusb):
    """Class that represents the Garmin USB-m stick base.
    Only needs to set VID/PID.
    
    """
    VID = 0x0fcf
    PID = 0x1009
    NAME = "Garmin"

class DynastreamANT(ANTlibusb):
    """Class that represents the Dynastream USB stick base, for
    garmin/suunto equipment. Only needs to set VID/PID.

    """
    VID = 0x0fcf
    PID = 0x1008
    NAME = "Dynastream"
