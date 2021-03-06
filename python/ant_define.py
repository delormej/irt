
# ----------------------------------------------------------------------------
# COMMON SECTION FOR ALL ANT DEFINES
# ----------------------------------------------------------------------------
ANT_NETWORK_KEY = [0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45]

INDEX_MESG_ID=2
INDEX_MESG_DATA=3
INDEX_CHANNEL_NUM=INDEX_MESG_DATA
MESG_BROADCAST_DATA_ID=0x4E
MESG_RESPONSE_EVENT_ID=0x40
MESG_CHANNEL_ID_ID=0x51
MESG_BURST_DATA_ID=0x50
RESPONSE_NO_ERROR=0x00
CHANNEL_TYPE_RX=0x00
CHANNEL_TYPE_TX=0x10
SPEED_PAGE_0 = 0x00
POWER_TORQUE_PAGE = 0x11
RESERVED = 0xFF

ANT_SPEED_RX_CHANNEL = 0X00
ANT_POWER_CHANNEL = 0X01
ANT_SPEED_TX_CHANNEL = 0X02

SPEED_DEVICE_TYPE=0x7B
SPEED_MESSAGE_PERIOD=[0xB6, 0x1F] #8118
SPEED_FREQUENCY=0x39 #57
SPEED_TIME_OUT=0x0C

SPEED_CAD_DEVICE_TYPE=0x79
SPEED_CAD_MESSAGE_PERIOD=[0x96, 0x1F] # 8086

WHEEL_REVS_ROLLOVER=65536

POWER_DEVICE_TYPE=0x0B
POWER_FREQUENCY=0x39
POWER_MESSAGE_PERIOD=[0xF6,0X1F]
POWER_KCKR_TRANSMISSION_TYPE=0xA5 # To make us look like a KCKR
#POWER_TRANSMISSION_TYPE=0x5 # Not used, use above
POWER_DEVICE_NUMBER=[0x64,0x00]  #dummy device number

# data page number, reserved, reserved, HW revision, Manufacturer ID LSB, Manufacturer ID MSB, Model LSB, Model MSB
POWER_MANUFACTURER_MESSAGE = [MESG_BROADCAST_DATA_ID, ANT_POWER_CHANNEL, 0x50, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x01, 0x00]

# data page number, reserved, Supplemental SW revision, SW Revision, Serial # (4 bytes)
POWER_PRODUCT_MESSAGE = [MESG_BROADCAST_DATA_ID, ANT_POWER_CHANNEL, 0x51, 0xFF, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00]