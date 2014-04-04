

#define LED_A																13
#define LED_B																12

#define BOOTLOADER_BUTTON_PIN           24 //TODO this is temporary                                                /**< Button used to enter SW update mode. */
#define LED_ERR													LED_B
#define LED_STATUS											LED_A

#define ADVERTISING_LED_PIN_NO               LED_A                                                   /**< Is on when device is advertising. */
#define CONNECTED_LED_PIN_NO                 LED_A                                                   /**< Is on when device has connected. */
#define ASSERT_LED_PIN_NO                    LED_B                                                   /**< Is on when application has asserted. */

#define DEVICE_NAME                          "DfuTarg"                                               /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                    "NordicSemiconductor"                                   /**< Manufacturer. Will be passed to Device Information Service. */
