#include "videocapture.h"
#include "videowidget.h"

#include <linux/videodev2.h>
#include <libv4l2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cstring>
#include <iostream>
#include <chrono>

VideoCapture::VideoCapture() = default;

VideoCapture::~VideoCapture()
{
    stop();
}

bool VideoCapture::start(const std::string &devicePath, VideoWidget *widget)
{
    if (running) {
        return false;
    }

    this->devicePath = devicePath;
    this->videoWidget = widget;

    if (!setupDevice()) {
        return false;
    }

    if (!initBuffers()) {
        v4l2_close(deviceFd);
        return false;
    }

    running = true;
    captureThreadObj = std::make_unique<std::thread>(&VideoCapture::captureThread, this);

    return true;
}

void VideoCapture::stop()
{
    running = false;
    
    if (captureThreadObj && captureThreadObj->joinable()) {
        captureThreadObj->join();
    }

    cleanupBuffers();
    
    if (deviceFd >= 0) {
        v4l2_close(deviceFd);
        deviceFd = -1;
    }
}

bool VideoCapture::isCapturing() const
{
    return running;
}

bool VideoCapture::setupDevice()
{
    deviceFd = v4l2_open(devicePath.c_str(), O_RDWR);
    if (deviceFd < 0) {
        std::cerr << "Error abriendo dispositivo: " << devicePath << std::endl;
        return false;
    }

    // Configurar formato de video
    v4l2_format fmt;
    std::memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 1920;
    fmt.fmt.pix.height = 1080;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (v4l2_ioctl(deviceFd, VIDIOC_S_FMT, &fmt) < 0) {
        std::cerr << "Error configurando formato" << std::endl;
        v4l2_close(deviceFd);
        return false;
    }

    // Configurar FPS a 60
    v4l2_streamparm parm;
    std::memset(&parm, 0, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.timeperframe.numerator = 1;
    parm.parm.capture.timeperframe.denominator = 60;

    if (v4l2_ioctl(deviceFd, VIDIOC_S_PARM, &parm) < 0) {
        std::cerr << "Warning: No se pudo configurar 60 FPS" << std::endl;
    }

    return true;
}

bool VideoCapture::initBuffers()
{
    v4l2_requestbuffers req;
    std::memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (v4l2_ioctl(deviceFd, VIDIOC_REQBUFS, &req) < 0) {
        std::cerr << "Error solicitando buffers" << std::endl;
        return false;
    }

    // Mapear buffers
    for (unsigned int i = 0; i < req.count; ++i) {
        v4l2_buffer buf;
        std::memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (v4l2_ioctl(deviceFd, VIDIOC_QUERYBUF, &buf) < 0) {
            std::cerr << "Error consultando buffer" << std::endl;
            return false;
        }

        // Poner en cola
        if (v4l2_ioctl(deviceFd, VIDIOC_QBUF, &buf) < 0) {
            std::cerr << "Error poniendo buffer en cola" << std::endl;
            return false;
        }
    }

    // Iniciar streaming
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_ioctl(deviceFd, VIDIOC_STREAMON, &type) < 0) {
        std::cerr << "Error iniciando streaming" << std::endl;
        return false;
    }

    return true;
}

void VideoCapture::cleanupBuffers()
{
    if (deviceFd >= 0) {
        v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_ioctl(deviceFd, VIDIOC_STREAMOFF, &type);
    }
}

void VideoCapture::captureThread()
{
    while (running) {
        v4l2_buffer buf;
        std::memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        // Obtener frame del dispositivo
        if (v4l2_ioctl(deviceFd, VIDIOC_DQBUF, &buf) < 0) {
            std::cerr << "Error obteniendo buffer" << std::endl;
            break;
        }

        // Enviar frame al widget
        if (videoWidget) {
            videoWidget->updateFrame(buf.m.offset, buf.bytesused);
        }

        // Devolver buffer a la cola
        if (v4l2_ioctl(deviceFd, VIDIOC_QBUF, &buf) < 0) {
            std::cerr << "Error reencolando buffer" << std::endl;
            break;
        }

        // Limitar a 60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
