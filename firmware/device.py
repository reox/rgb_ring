import usb

MANUFACTURER = "reox.at"
PRODUCT = "http://reox.at/rgb_ring/"

def discover():
    my_device = None
    for bus in usb.busses():
        for dev in bus.devices:
            if dev.idVendor == 0x16c0 and dev.idProduct == 0x05dc:
                handle = dev.open()
                manufacturer = handle.getString(dev.iManufacturer, len(MANUFACTURER)+1)
                product = handle.getString(dev.iProduct, len(PRODUCT)+1)
                if manufacturer == MANUFACTURER and product == PRODUCT:
                    if my_device is not None:
                        raise Exception("More than one device found")
                    my_device = dev
                    my_handle = handle

    if my_device is None:
        raise Exception("No device found")

    return my_handle
