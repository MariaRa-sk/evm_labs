#include <iostream>
#include <libusb.h>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

string getDeviceClassName(uint8_t deviceClass) {
    switch (deviceClass) {
        case LIBUSB_CLASS_PER_INTERFACE: return "Interface Specific";
        case LIBUSB_CLASS_AUDIO: return "Audio";
        case LIBUSB_CLASS_COMM: return "Communications";
        case LIBUSB_CLASS_HID: return "Human Interface Device";
        case LIBUSB_CLASS_PHYSICAL: return "Physical";
        case LIBUSB_CLASS_PRINTER: return "Printer";
        case LIBUSB_CLASS_IMAGE: return "Image";
        case LIBUSB_CLASS_MASS_STORAGE: return "Mass Storage";
        case LIBUSB_CLASS_HUB: return "Hub";
        case LIBUSB_CLASS_DATA: return "CDC Data";
        case LIBUSB_CLASS_SMART_CARD: return "Smart Card";
        case LIBUSB_CLASS_CONTENT_SECURITY: return "Content Security";
        case LIBUSB_CLASS_VIDEO: return "Video";
        case LIBUSB_CLASS_PERSONAL_HEALTHCARE: return "Personal Healthcare";
        case LIBUSB_CLASS_DIAGNOSTIC_DEVICE: return "Diagnostic Device";
        case LIBUSB_CLASS_WIRELESS: return "Wireless";
        case LIBUSB_CLASS_APPLICATION: return "Application";
        case LIBUSB_CLASS_VENDOR_SPEC: return "Vendor Specific";
        default: return "Unknown";
    }
}

void print_device(libusb_device* dev) {
    libusb_device_descriptor desc{};
    int result = libusb_get_device_descriptor(dev, &desc);
    if (result < 0) {
        cerr << "Ошибка получения дескриптора устройства: " << libusb_error_name(result) << endl;
        return;
    }

    cout << "---------------------------------------------------" << endl;
    
    string className = getDeviceClassName(desc.bDeviceClass);
    cout << "Класс устройства:    " << className << " (0x" << hex << setw(2) << setfill('0') 
         << static_cast<int>(desc.bDeviceClass) << ")" << dec << endl;
    
    cout << "Vendor ID:           0x" << hex << setw(4) << setfill('0') 
         << desc.idVendor << dec << endl;
    
    cout << "Product ID:          0x" << hex << setw(4) << setfill('0') 
         << desc.idProduct << dec << endl;

    libusb_device_handle* handle = nullptr;
    result = libusb_open(dev, &handle);
    
    if (result == 0 && handle != nullptr) {
        if (desc.iSerialNumber > 0) {
            unsigned char serial[256];
            int length = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial, sizeof(serial));
            if (length > 0) {
                cout << "Серийный номер:      " << serial << endl;
            } 
            else {
                cout << "Серийный номер:     <не удалось прочитать>" << endl;
            }
        } 
        else {
            cout << "Серийный номер:     <отсутствует>" << endl;
        }
        libusb_close(handle);
    } 
    else {
        cout << "Серийный номер:      <не удалось открыть устройство>" << endl;
        cout << "Примечание:          " << libusb_error_name(result) << endl;
    }
    
    cout.flush();
}

int main(void) 
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    libusb_context* ctx = nullptr;
    libusb_device** devs = nullptr;

    int result = libusb_init(&ctx);
    if (result < 0) {
        cerr << "Ошибка инициализации libusb: " << libusb_error_name(result) << endl;
        return 1;
    }

    int count = libusb_get_device_list(ctx, &devs);    
    if (count < 0) {
        cerr << "Ошибка получения списка устройств: " << libusb_error_name(result) << endl;
        libusb_exit(ctx);
        return 1;
    }

    cout << "Найдено USB устройств: " << count << "\n" << endl;

    for (int i = 0; i < count; ++i) {
        print_device(devs[i]);
    }

    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
    return 0;
}
