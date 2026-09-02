#include "mainwindow.h"
#include "videowidget.h"
#include "devicedetector.h"
#include "videocapture.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      deviceDetector(std::make_unique<DeviceDetector>()),
      videoCapture(std::make_unique<VideoCapture>())
{
    setupUI();
    connectSignals();
    updateDeviceList();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    // Widget central
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Panel de control
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    QLabel *deviceLabel = new QLabel("Dispositivo:", this);
    deviceCombo = new QComboBox(this);
    deviceCombo->setMinimumWidth(300);
    
    startBtn = new QPushButton("Iniciar", this);
    stopBtn = new QPushButton("Detener", this);
    stopBtn->setEnabled(false);

    controlLayout->addWidget(deviceLabel);
    controlLayout->addWidget(deviceCombo);
    controlLayout->addWidget(startBtn);
    controlLayout->addWidget(stopBtn);
    controlLayout->addStretch();

    // Widget de video
    videoWidget = new VideoWidget(this);
    videoWidget->setMinimumSize(640, 480);

    // Layout principal
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(videoWidget, 1);

    setCentralWidget(centralWidget);
    statusBar()->showMessage("Listo");
}

void MainWindow::connectSignals()
{
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartCapture);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::onStopCapture);
    connect(deviceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDeviceSelected);
}

void MainWindow::updateDeviceList()
{
    deviceCombo->clear();
    
    auto devices = deviceDetector->detectDevices();
    for (const auto &device : devices) {
        deviceCombo->addItem(QString::fromStdString(device.name),
                            QString::fromStdString(device.path));
    }
    
    if (devices.empty()) {
        statusBar()->showMessage("No se encontraron dispositivos de captura");
    } else {
        statusBar()->showMessage(QString("Se encontraron %1 dispositivo(s)").arg(devices.size()));
    }
}

void MainWindow::onDeviceSelected(int index)
{
    if (index >= 0) {
        QString devicePath = deviceCombo->currentData().toString();
        // Preparar dispositivo seleccionado
    }
}

void MainWindow::onStartCapture()
{
    QString devicePath = deviceCombo->currentData().toString();
    if (!devicePath.isEmpty()) {
        if (videoCapture->start(devicePath.toStdString(), videoWidget)) {
            startBtn->setEnabled(false);
            stopBtn->setEnabled(true);
            statusBar()->showMessage("Capturando...");
        } else {
            statusBar()->showMessage("Error al iniciar captura");
        }
    }
}

void MainWindow::onStopCapture()
{
    videoCapture->stop();
    startBtn->setEnabled(true);
    stopBtn->setEnabled(false);
    statusBar()->showMessage("Captura detenida");
}
