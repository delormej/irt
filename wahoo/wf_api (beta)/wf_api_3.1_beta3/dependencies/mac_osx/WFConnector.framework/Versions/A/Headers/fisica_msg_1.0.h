/*
 *  fisica_msg_1.0.h
 *  WFConnector
 *
 *  fisica communication message definitions and data strutures.
 *
 *  version 1.0
 *  see document FisicaMessages_v_1.0.doc for details.
 *
 *  Created by Michael Moore on 5/19/10.
 *  Copyright 2010 Wahoo Fitness. All rights reserved.
 *
 */


#ifndef __FISICA_MSG_1_0_H__
#define __FISICA_MSG_1_0_H__


// Define data from hardware using simple types that can be packed
// in proper byte ordering.  This makes the hardware socket reads
// easier...

// fixed byte alignment
#pragma pack(1)

#define FISICA_SYNC_BYTE                    0xEE
#define FISICA_MESSAGE_VERSION              0x01
#define FISICA_MESSAGE_DATA_OFFSET          0x03


/**
 * Describes the message identification for the fisica serial message protocol.
 */
enum DATA_PACKET_IDS
{
	/** Firmware version message. */
	FMSG_FIRMWARE_VERSION                                   = 0x00,
	/** Firmware version message acknowledgement. */
	FMSG_FIRMWARE_VERSION_ACK                               = 0x01,
	
	FMSG_PING                                               = 0x02,
	FMSG_ACK                                                = 0x03,
	FMSG_REQUEST_SERIAL_NUMBER                              = 0x04,
	FMSG_RETURN_SERIAL_NUMBER                               = 0x05,
	
	/** ANT Reset message, resets ANT network. */
	FMSG_ANT_RESET                                          = 0x10,
	/** ANT Data Transfer from device to iPod. */
	FMSG_DEV_ANT_TRANSFER                                   = 0x11,
	/** ANT Data Transfer from the iPod to the device. */
	FMSG_IPOD_ANT_TRANSFER                                  = 0x12,
	
	FMSG_WRITE_SERIAL_NUMBER                                = 0x50,
	
	FMSG_ANT_BURST_PACKET                                   = 0xAB,
    FMSG_ANT_EXT_BURST_PACKET                               = 0xAE,
	FMSG_REQUEST_DEVICE_INFO                                = 0xE0,
	FMSG_RETURN_DEVICE_INFO                                 = 0xE1,
	FMSG_START_FIRMWARE_UPDATE                              = 0xE2,
	FMSG_FIRMWARE_DATA                                      = 0xE3,
	FMSG_FIRMWARE_DATA_COMPLETE                             = 0xE4,
	
	WF_PACKET_LASTID
};

/**
 * Describes the acknowledgement status for the fisica firmware version
 * message.
 */
enum FISICA_FIRMWARE_ACK_STATUS
{
	/** Version okay; Status normal operation. s*/
	FISICA_FIRMWARE_ACK_OK                                 = 0x00,
	/** Version incorrect; Shut down. */
	FISICA_FIRMWARE_ACK_SHUTDOWN                           = 0x01,
	/** Version incorrect; Prepare for firmware update. */
	FISICA_FIRMWARE_ACK_UPDATE                             = 0x02,
};

enum FISICA_ACK_STATUS
{
	FISICA_ACK_STATUS_SUCCESS                              = 0x00,
	FISICA_ACK_STATUS_FAILURE                              = 0x01,
	FISICA_ACK_STATUS_SERIAL_ALREADY_WRITTEN               = 0x02,
	FISICA_ACK_STATUS_BAD_CHECKSUM                         = 0x03,
	FISICA_ACK_STATUS_VERSION_ERROR                        = 0x04,
	FISICA_ACK_STATUS_LOCATION_ERROR                       = 0x05,
	FISICA_ACK_STATUS_DATA_LENGTH_ERROR                    = 0x06,
	FISICA_ACK_STATUS_DATA_WRITE_ERROR                     = 0x07,
	FISICA_ACK_STATUS_CONFIG_WRITE_ERROR                   = 0x08,
};

//--------------------------------------------------------------------------------
/**
 * Data structure describing the fisica serial message protocol message header.
 */
typedef struct
{
	/** The fisica message sync byte. */
	UCHAR sync;
	/** The version of the fisica serial message protocol. */
	UCHAR version;
	/** The length of the message payload, beginning with the message ID byte. */
	UCHAR payloadLength;
	/** The fisica message identifier. */
	UCHAR messageId;
	
} MsgPacketHeader;

//--------------------------------------------------------------------------------
/**
 * Data structure describing the fisica serial message protocol firmware
 * version message.
 */
typedef struct
{
	/** The standard fisica message header. */
	MsgPacketHeader header;
	/** The major version of the installed firmware. */
	UCHAR majorVersionNumber;
	/** The minor version of the installed firmware. */
	UCHAR minorVersionNumber;
	/** The version revision of the installed firmware. */
	UCHAR revisionNumber;
	
} FirmwareVersionMsg;

//--------------------------------------------------------------------------------
/**
 * Data structure describing the fisica serial message protocol firmware
 * version acknowledgement message.
 */
typedef struct
{
	/** The standard fisica message header. */
	MsgPacketHeader header;
	/** The firmware version ACK status (see ::FISICA_FIRMWARE_ACK_STATUS). */
	UCHAR status;
	
} FirmwareVersionAckMsg;

//--------------------------------------------------------------------------------
typedef struct
{
	MsgPacketHeader header;
	UCHAR acknowledgedCommand;
	UCHAR status;
	
} FisicaAckMsg;

//--------------------------------------------------------------------------------
/**
 * Data structure describing the fisica serial message protocol ANT Data
 * Transfer message (to or from the device to the iPod).
 */
typedef struct
{
	/** The standard fisica message header. */
	MsgPacketHeader msgHeader;
	/** The first byte of the ANT message data. */
	UCHAR firstDataByte;
	
} AntDataMsg;

//--------------------------------------------------------------------------------
typedef struct
{
	MsgPacketHeader header;
	UCHAR serialNumberLSB;
	UCHAR serialNumberB1;
	UCHAR serialNumberB2;
	UCHAR serialNumberMSB;
	
} FisicaSerialNumberMsg;
#define SERIAL_NUMBER_MSG_SIZE 5

//--------------------------------------------------------------------------------
#define REQUEST_DEVICE_INFO_MSG_SIZE 1
typedef struct
{
	MsgPacketHeader header;
	UCHAR hardwareVersionLSB;
	UCHAR hardwareVersionMSB;
	UCHAR bootLoaderVersionLSB;
	UCHAR bootLoaderVersionMSB;
	UCHAR firmwareVersionMajor;
	UCHAR firmwareVersionMinor;
	UCHAR firmwareVersionRevision;
	UCHAR runningHighMemory;
	UCHAR authChipPass;
	
} FisicaDeviceInfoMsg;

//--------------------------------------------------------------------------------
typedef struct
{
	MsgPacketHeader header;
	UCHAR firmwareVersionMajor;
	UCHAR firmwareVersionMinor;
	UCHAR firmwareVersionRevision;
	UCHAR baseAddressLSB;
	UCHAR baseAddressMSB;
	
} FisicaStartFirmwareUpdateMsg;
#define START_FW_UPDATE_MSG_SIZE 6

//--------------------------------------------------------------------------------
typedef struct
{
	MsgPacketHeader header;
	UCHAR baseAddressLSB;
	UCHAR baseAddressMSB;
	UCHAR firstDataByte;
	
} FisicaFirmwareDataMsg;

//--------------------------------------------------------------------------------
typedef struct
{
	MsgPacketHeader header;
	UCHAR checksumLSB;
	UCHAR checksumMSB;
	
} FisicaFirmwareDataCompleteMsg;
#define FW_DATA_COMPLETE_MSG_SIZE 3


#pragma options align=reset

#endif // __FISICA_MSG_1_0_H__