#include <iostream>
#include <libusb-1.0/libusb.h>
#include <vector>
using namespace std;
 
class CServo
{
        public:
                enum EMode
                {
                        BULK,
                        INTERUPT
                };
               
        private:
                libusb_context *usb_ctx;
                libusb_device_handle* usb_handle;
                EMode mode; //0 = bulk 1 = interrupt
                int host_device, device_host;
        public:        
                //! opens a serial
                CServo(int vendor, int serial, int host_device, int device_host, EMode mode): usb_ctx(0), usb_handle(0), host_device(host_device), device_host(device_host), mode(mode)
                {
                        if (libusb_init(&usb_ctx) < 0)
                        {
                                throw "Couldn't init libusb !";
                        }
                       
                        libusb_device **devs;
                        size_t cnt = libusb_get_device_list(usb_ctx, &devs);
                        for(size_t i = 0; i < cnt; i++)
                        {
                                libusb_device *dev = devs[i];
                                libusb_device_descriptor desc;
                               
                                if (libusb_get_device_descriptor(dev, &desc) < 0)
                                {
                                        continue;
                                }
                               
                                if (desc.idVendor == vendor)
                                {
                                        //same vendor
                                        if (desc.iSerialNumber == serial)
                                        {
                                                //same serial
                                                if (libusb_open(dev, &usb_handle) < 0)
                                                {
                                                        throw "Couldn't open usb device !";
                                                }
                                                else
                                                {
                                                        if (libusb_kernel_driver_active(usb_handle, 0) == 1)
                                                        {
                                                                cout << "Deatch" << endl;
                                                                libusb_detach_kernel_driver(usb_handle, 0);
                                                        }
                                                       
                                                        //all okay
                                                        /*
                                                        if (libusb_set_configuration(usb_handle, 1) < 0)
                                                        {
                                                                throw "Couldn't set configuration of usb device !";
                                                        }
                                                        */
                                                       
                                                        if (libusb_claim_interface(usb_handle, 0) < 0)
                                                        {
                                                                throw "Couldn't claim usb device !";
                                                        }
                                                       
                                                        break;
                                                }
                                        }
                                }
                        }
                       
                        libusb_free_device_list(devs, 1);
                }
               
                ~CServo()
                {
                        libusb_release_interface(usb_handle, 0);
                        libusb_close(usb_handle);
                        libusb_exit(usb_ctx);
                }
               
                int write(unsigned char* data, int size, int timeout = 0)
                {
                        if (usb_handle != 0)
                        {
                                //bulk write ?
                                int wrote = 0;
                                int r = 0;
                                if (mode == BULK)
                                {
                                        r = libusb_bulk_transfer(usb_handle, LIBUSB_ENDPOINT_OUT|host_device, data, size, &wrote, timeout);
                                }
                                else
                                {
                                        r = libusb_interrupt_transfer(usb_handle, LIBUSB_ENDPOINT_OUT|host_device, data, size, &wrote, timeout);
                                }
                                if (r < 0)
                                {
                                        return r;
                                }
                                if (wrote == size)
                                {
                                        return size;
                                }
                        }
                        return -1;                     
                }
               
                int read(unsigned char* data, int size, int timeout = 0)
                {
                        if (usb_handle != 0)
                        {
                                //bulk read ?
                                int read = 0;
                                int r = 0;
                                if (mode == BULK)
                                {
                                        r = libusb_bulk_transfer(usb_handle, LIBUSB_ENDPOINT_IN|device_host, data, size, &read, timeout);
                                }
                                else
                                {
                                        r = libusb_interrupt_transfer(usb_handle, LIBUSB_ENDPOINT_IN|device_host, data, size, &read, timeout);
                                }
                                if (r < 0)
                                {      
                                        return r;
                                }
                                return read;
                        }
                        return -1;                     
                }
               
                //! following returns a list of serials
                static vector<int> findServos(int vendor)
                {
                        vector<int> result;
       
                        libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
                        libusb_context *ctx = NULL; //a libusb session
       
                        int r; //for return values
                        ssize_t cnt; //holding number of devices in list
                        r = libusb_init(&ctx); //initialize a library session
       
                        if(r < 0)
                        {
                                throw "Couldn't init libusb !";
                        }
       
                        libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
                        cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
                       
                        for(size_t i = 0; i < cnt; i++)
                        {
                                libusb_device *dev = devs[i];
                                libusb_device_descriptor desc;
                               
                                r = libusb_get_device_descriptor(dev, &desc);
                                if (r < 0)
                                {
                                        //device death ?
                                        continue;
                                }
                               
                                if (desc.idVendor == vendor)
                                {
                                        //found !! add to list !
                                        result.push_back(desc.iSerialNumber);
                                }
                        }
                       
                        libusb_free_device_list(devs, 1); //free the list, unref the devices in it
                        libusb_exit(ctx); //close the session
                       
                        return result;
                }
};
 