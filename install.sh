#!/bin/bash

# Script de instalación rápida para hdmi-capture-viewer
# Uso: ./install.sh

set -e

echo "=== HDMI Capture Viewer - Script de Instalación ==="
echo ""

# Detectar distribución
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    # Para distros basadas en Ubuntu/Debian (como Elementary)
    if [ -z "$OS" ] || [ "$OS" = "elementary" ]; then
        OS_LIKE=$(echo "$ID_LIKE" | cut -d' ' -f1)
        if [ "$OS_LIKE" = "ubuntu" ] || [ "$OS_LIKE" = "debian" ]; then
            OS="debian"
        fi
    fi
else
    echo "Error: No se pudo detectar el SO"
    exit 1
fi

echo "Sistema detectado: $OS"

# Instalar dependencias según el OS
case $OS in
    ubuntu|debian)
        echo "Instalando dependencias para Ubuntu/Debian..."
        sudo apt-get update
        sudo apt-get install -y \
            build-essential cmake git \
            qt6-base-dev qt6-multimedia-dev libqt6opengl6-dev libqt6openglwidgets6 \
            libv4l-dev ffmpeg libavcodec-dev libavformat-dev libswscale-dev \
            libglm-dev pkg-config
        ;;
    fedora|rhel|centos)
        echo "Instalando dependencias para Fedora/RHEL..."
        sudo dnf groupinstall -y "Development Tools"
        sudo dnf install -y \
            cmake qt6-qtbase-devel qt6-qtmultimedia-devel \
            libv4l-devel ffmpeg-devel glm-devel pkgconfig
        ;;
    arch)
        echo "Instalando dependencias para Arch..."
        sudo pacman -S --noconfirm base-devel cmake qt6-base qt6-multimedia libv4l ffmpeg glm
        ;;
    *)
        echo "Distribución no soportada: $OS"
        echo "Por favor, instala manualmente las dependencias para tu distribución."
        echo ""
        echo "Necesitas:"
        echo "  - GCC/Clang compiler"
        echo "  - CMake 3.20+"
        echo "  - Qt6 (base y multimedia)"
        echo "  - libv4l-dev"
        echo "  - FFmpeg development libraries"
        echo "  - GLM"
        exit 1
        ;;
esac

# Agregar usuario al grupo video
echo ""
echo "Agregando usuario al grupo video..."
sudo usermod -a -G video $USER
echo "Nota: Debes cerrar y reiniciar sesión para que los cambios tomen efecto"

# Compilar
echo ""
echo "Compilando..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "=== Instalación completada ==="
echo ""
echo "Para ejecutar la aplicación:"
echo "  cd build"
echo "  ./hdmi-capture-viewer"
echo ""
echo "O instalarla globalmente:"
echo "  cd build"
echo "  sudo make install"
echo "  hdmi-capture-viewer"
