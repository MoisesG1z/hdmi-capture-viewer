# HDMI Capture Viewer

Aplicación de escritorio para Linux que permite ver dispositivos de captura HDMI en tiempo real con rendimiento optimizado a 60 FPS.

## Características

- 📺 Detección automática de dispositivos de captura HDMI
- ⚡ Reproducción en tiempo real a 60 FPS sin lag
- 🎥 Captura de video mediante V4L2 (Video4Linux2)
- 🖼️ Interfaz gráfica moderna con Qt6
- 🔧 Baja latencia y uso eficiente de GPU

## Tecnología

- **Lenguaje**: C++
- **UI Framework**: Qt6
- **Captura de video**: V4L2 (Video4Linux2)
- **Renderizado**: OpenGL con Qt
- **Sistema operativo**: Linux

## Requisitos

- Linux (Ubuntu 20.04+ / Fedora 35+ / Arch)
- Qt6
- ffmpeg
- libv4l-dev
- GCC 11+ o Clang 12+
- CMake 3.20+

## Instalación

### 1. Instalación Automática
```bash
git clone https://github.com/MoisesG1z/hdmi-capture-viewer.git
cd hdmi-capture-viewer
chmod +x install.sh
./install.sh
```

### 2. Instalación Manual

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake \
  qt6-base-dev qt6-multimedia-dev libqt6opengl6-dev \
  libv4l-dev ffmpeg libavcodec-dev libavformat-dev libswscale-dev \
  libglm-dev pkg-config
```

**Fedora/RHEL:**
```bash
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y \
  cmake qt6-qtbase-devel qt6-qtmultimedia-devel \
  libv4l-devel ffmpeg-devel glm-devel pkgconfig
```

**Arch:**
```bash
sudo pacman -S base-devel cmake qt6-base qt6-multimedia libv4l ffmpeg glm
```

### 3. Compilación
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 4. Ejecución
```bash
./hdmi-capture-viewer
```

## Permiso de acceso a dispositivos

Agrega tu usuario al grupo `video` para acceder a `/dev/video*`:

```bash
sudo usermod -a -G video $USER
# Cierra sesión y vuelve a iniciar
```

## Uso

1. Conecta tu dispositivo HDMI de captura
2. Abre la aplicación
3. Selecciona el dispositivo de la lista
4. Haz clic en "Iniciar"
5. ¡Disfruta de video en tiempo real a 60 FPS!

## Estructura del Proyecto

```
hdmi-capture-viewer/
├── src/
│   ├── main.cpp                 # Punto de entrada
│   ├── mainwindow.h/cpp         # Ventana principal
│   ├── devicedetector.h/cpp     # Detección de dispositivos V4L2
│   ├── videocapture.h/cpp       # Captura de video optimizada
│   └── videowidget.h/cpp        # Widget de renderizado OpenGL
├── CMakeLists.txt              # Configuración de compilación
├── install.sh                  # Script de instalación
├── README.md                   # Este archivo
├── INSTALL.md                  # Guía detallada de instalación
├── DEVELOPMENT.md              # Guía de desarrollo
└── LICENSE                     # Licencia MIT
```

## Optimizaciones para 60 FPS

- ✅ Uso de buffers circulares para evitar copias innecesarias
- ✅ Renderizado con OpenGL/GPU
- ✅ Thread separado para captura de video
- ✅ Sincronización eficiente con VSyncc
- ✅ Gestión de memoria pre-asignada

## Solución de Problemas

### No se encuentra dispositivo
```bash
# Verifica dispositivos disponibles
ls -la /dev/video*
v4l2-ctl --list-devices
```

### Permiso denegado a /dev/video*
```bash
# Agrega usuario al grupo video
sudo usermod -a -G video $USER
```

### Error al compilar
```bash
# Limpia build anterior
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## Licencia

MIT - Ver LICENSE para detalles

## Contribuciones

Las contribuciones son bienvenidas. Por favor, abre un issue o pull request.
