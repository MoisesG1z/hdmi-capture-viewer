#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <string>
#include <thread>
#include <atomic>
#include <memory>

class VideoWidget;

class VideoCapture
{
public:
    VideoCapture();
    ~VideoCapture();

    // Inicia la captura desde el dispositivo especificado
    bool start(const std::string &devicePath, VideoWidget *widget);
    
    // Detiene la captura
    void stop();
    
    // Verifica si está capturando
    bool isCapturing() const;

private:
    void captureThread();
    bool setupDevice();
    bool initBuffers();
    void cleanupBuffers();

    std::string devicePath;
    VideoWidget *videoWidget;
    
    std::atomic<bool> running{false};
    std::unique_ptr<std::thread> captureThreadObj;
    
    int deviceFd{-1};
    static constexpr int BUFFER_COUNT = 4;
};

#endif // VIDEOCAPTURE_H
