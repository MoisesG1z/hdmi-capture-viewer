# HDMI Capture Viewer - Guía de Desarrollo

## Arquitectura

```
┌─────────────────────────────────────────────┐
│           MainWindow (Qt)                   │
│  ┌───────────────────────────────────────┐  │
│  │     VideoWidget (OpenGL Renderer)     │  │
│  │                                       │  │
│  │    GPU Rendering - 60 FPS Target      │  │
│  └───────────────────────────────────────┘  │
│                    ▲                         │
│                    │ updateFrame()           │
└────────────────────┼─────────────────────────┘
                     │
                     │ Thread
                     │
         ┌───────────┴──────────────┐
         │                          │
    ┌────▼──────────┐      ┌───────▼─────────┐
    │ VideoCapture  │      │ DeviceDetector  │
    │               │      │                 │
    │ V4L2 Stream   │      │ /dev/video*     │
    │ 1920x1080     │      │ Query Devices   │
    │ YUYV Format   │      │ Get Info        │
    │ 60 FPS        │      └─────────────────┘
    └────┬──────────┘
         │
         ▼
    ┌─────────────┐
    │  /dev/video*│ (HDMI Capture Device)
    └─────────────┘
```

## Flujo de Datos

1. **Detección de Dispositivos**
   - `DeviceDetector` escanea `/dev/video*`
   - Consulta V4L2 para verificar capacidades
   - Retorna lista de dispositivos de captura

2. **Captura de Video**
   - Usuario selecciona dispositivo
   - `VideoCapture` abre conexión V4L2
   - Configura resolución (1920x1080) y FPS (60)
   - Thread de captura obtiene frames constantemente

3. **Renderizado**
   - Frames en formato YUYV se envían a `VideoWidget`
   - Shader OpenGL convierte YUYV → RGB en GPU
   - `glDrawArrays()` renderiza en tiempo real
   - VSync sincroniza con pantalla (60 Hz)

## Componentes Principales

### `DeviceDetector`
- Escanea dispositivos V4L2 disponibles
- Filtra solo dispositivos de captura
- Extrae información del dispositivo (nombre, driver)

### `VideoCapture`
- Maneja la comunicación V4L2
- Thread separado para evitar bloqueos
- Buffers circulares para eficiencia
- Control de FPS

### `VideoWidget`
- Widget Qt basado en OpenGL
- Renderizado con shaders GLSL
- Conversión YUYV→RGB en GPU
- Sincronización con VSync

### `MainWindow`
- Interfaz de usuario principal
- Controles para iniciar/detener captura
- Estado de captura

## Optimizaciones para 60 FPS

### 1. Captura eficiente
```cpp
// Buffers pre-asignados (evita malloc en tiempo real)
static constexpr int BUFFER_COUNT = 4;

// Mapping de memoria (no copias)
v4l2_ioctl(fd, VIDIOC_QUERYBUF, &buf);
```

### 2. Renderizado en GPU
```glsl
// Conversión YUYV→RGB en el fragmento shader
// No en CPU (mucho más rápido)
float r = y + 1.402 * (v - 0.5);
float g = y - 0.344136 * (u - 0.5) - 0.714136 * (v - 0.5);
float b = y + 1.772 * (u - 0.5);
```

### 3. Thread separado
```cpp
// No bloquea UI
captureThreadObj = std::make_unique<std::thread>(
    &VideoCapture::captureThread, this);
```

### 4. Sincronización
```cpp
// Limita a 60 FPS
std::this_thread::sleep_for(std::chrono::milliseconds(16));
```

## Formatos de Video Soportados

| Formato | Ancho x Alto | FPS | Notas |
|---------|------------|-----|-------|
| YUYV    | 1920x1080  | 60  | Defecto |
| MJPEG   | 1920x1080  | 60  | (Futuro) |
| UYVY    | 1920x1080  | 60  | (Futuro) |

## Extensiones Futuras

### Corto plazo
- [ ] Soportar múltiples formatos (MJPEG, UYVY)
- [ ] Grabación a archivo (MP4, H.264)
- [ ] Ajustes de brillo/contraste
- [ ] Mostrar estadísticas (FPS actual, latencia)

### Mediano plazo
- [ ] Soporte para múltiples streams simultáneos
- [ ] Streaming por red (RTSP)
- [ ] Filtros en tiempo real
- [ ] Detección de rostros/objetos

### Largo plazo
- [ ] Interfaz web
- [ ] Integración con OBS
- [ ] Reconocimiento de patrones

## Testing

### Prueba manual sin dispositivo HDMI
```bash
# Crear dispositivo virtual con gst-launch
gst-launch-1.0 videotestsrc ! v4l2sink device=/dev/video10
```

### Verificar dispositivos
```bash
v4l2-ctl --list-devices
v4l2-ctl -d /dev/video0 --list-formats
ffmpeg -f v4l2 -list_formats all -i /dev/video0
```

### Monitoreo de rendimiento
```bash
# Ver FPS en ejecución
watch -n 1 'ps aux | grep hdmi-capture'

# Análisis de GPU (NVIDIA)
nvidia-smi --query-gpu=utilization.gpu,utilization.memory --format=csv -l 1
```

## Contribución

1. Fork el proyecto
2. Crea una rama (`git checkout -b feature/amazing-feature`)
3. Commit cambios (`git commit -m 'Add amazing feature'`)
4. Push a la rama (`git push origin feature/amazing-feature`)
5. Abre un Pull Request

## Licencia

MIT - Ver LICENSE para detalles
