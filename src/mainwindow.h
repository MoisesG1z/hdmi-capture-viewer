#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <memory>

class VideoWidget;
class DeviceDetector;
class VideoCapture;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDeviceSelected(int index);
    void onStartCapture();
    void onStopCapture();
    void updateDeviceList();

private:
    void setupUI();
    void connectSignals();

    QComboBox *deviceCombo;
    QPushButton *startBtn;
    QPushButton *stopBtn;
    VideoWidget *videoWidget;
    
    std::unique_ptr<DeviceDetector> deviceDetector;
    std::unique_ptr<VideoCapture> videoCapture;
};

#endif // MAINWINDOW_H
