#ifndef DEVICEDETECTOR_H
#define DEVICEDETECTOR_H

#include <string>
#include <vector>

struct CaptureDevice
{
    std::string name;
    std::string path;
    std::string driver;
};

class DeviceDetector
{
public:
    DeviceDetector();
    ~DeviceDetector();

    // Detecta todos los dispositivos de captura V4L2
    std::vector<CaptureDevice> detectDevices();

private:
    bool isVideoDevice(const std::string &path);
    std::string getDeviceInfo(const std::string &path);
};

#endif // DEVICEDETECTOR_H
