#ifndef IRT_EMOTION_H__
#define IRT_EMOTION_H__

#define DEVICE_NAME                     "E-Motion"                          /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "Inside Ride"            			/**< Manufacturer. Will be passed to Device Information Service. */
#define HW_REVISION                     0x01               					/**< Hardware revision for manufacturer's identification common page. */
#define MANUFACTURER_ID                 0xAAAA             				  	/**< Manufacturer ID for manufacturer's identification common page. */
#define MODEL_NUMBER                    0x5248								// Model 'HR' in ASCII /**< Model number for manufacturer's identification common page. */

/* VERSION is currently defined in make file.
#define SW_REVISION_MAJ					0			// Limited to 4 bits
#define SW_REVISION_MIN					0			// Limited to 4 bits
#define SW_REVISION_BLD					0			// Full byte
#define SW_REVISION                   	"0.0.0"		// major.minor.build
*/

/* DEVICE specific info */
#define ANT_DEVICE_NUMBER			(uint16_t)NRF_FICR->DEVICEID[1]
#define SERIAL_NUMBER				NRF_FICR->DEVICEID[1]


#endif	// IRT_EMOTION_H__
