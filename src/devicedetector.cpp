#include "devicedetector.h"

#include <linux/videodev2.h>
#include <libv4l2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <cstring>
#include <iostream>

DeviceDetector::DeviceDetector() = default;
DeviceDetector::~DeviceDetector() = default;

std::vector<CaptureDevice> DeviceDetector::detectDevices()
{
    std::vector<CaptureDevice> devices;

    // Escanear /dev/video*
    DIR *dir = opendir("/dev");
    if (!dir) {
        std::cerr << "Error abriendo /dev" << std::endl;
        return devices;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        
        // Buscar archivos video*
        if (name.find("video") == 0) {
            std::string path = "/dev/" + name;
            
            if (isVideoDevice(path)) {
                CaptureDevice device;
                device.path = path;
                device.name = name + " - " + getDeviceInfo(path);
                devices.push_back(device);
            }
        }
    }

    closedir(dir);
    return devices;
}

bool DeviceDetector::isVideoDevice(const std::string &path)
{
    int fd = v4l2_open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return false;
    }

    v4l2_capability cap;
    std::memset(&cap, 0, sizeof(cap));

    if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        v4l2_close(fd);
        return false;
    }

    // Verificar que sea un dispositivo de captura de video
    bool isCapture = (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) != 0;
    bool isStreaming = (cap.capabilities & V4L2_CAP_STREAMING) != 0;

    v4l2_close(fd);
    return isCapture && isStreaming;
}

std::string DeviceDetector::getDeviceInfo(const std::string &path)
{
    int fd = v4l2_open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return "Unknown";
    }

    v4l2_capability cap;
    std::memset(&cap, 0, sizeof(cap));

    if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        v4l2_close(fd);
        return "Unknown";
    }

    std::string info = reinterpret_cast<const char*>(cap.card);
    v4l2_close(fd);
    return info;
}
