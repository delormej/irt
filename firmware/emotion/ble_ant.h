#ifndef BLE_ANT_H__
#define BLE_ANT_H__

//
// Event callbacks needed for program flow and control.
//
typedef struct ant_ble_evt_handlers_s {
	void (*on_ble_connected)(void);
	void (*on_ble_disconnected)(void);
	void (*on_ble_timeout)(void);
	void (*on_ble_advertising)(void);
	void (*on_ant_channel_closed)(void);
	void (*on_ant_power_data)(void); // This will have a parameter.
	void (*on_set_resistance)(void); // This will have several params.
} ant_ble_evt_handlers_t;

void ble_ant_init(ant_ble_evt_handlers_t * ant_ble_evt_handlers);
void ble_ant_start(void);
void power_manage(void);

#endif // BLE_ANT_H__
