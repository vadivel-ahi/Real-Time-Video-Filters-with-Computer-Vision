# Real-Time Video Filters with Computer Vision

A high-performance C++ application that applies custom-built image processing filters to live video streams using OpenCV. Implements advanced computer vision techniques including edge detection, face recognition, and depth estimation—all from scratch without relying on built-in filter functions.

![Demo](docs/images/demo_face_detect.jpg)
*Real-time face detection and selective colorization*

## 🎯 Key Features

- **Custom Filter Implementation**: All filters coded from scratch for deeper understanding
- **Real-Time Processing**: Optimized for live video feed with 30+ FPS performance
- **Machine Learning Integration**: Haar Cascade face detection and ONNX depth estimation
- **10+ Visual Effects**: Grayscale, sepia, blur, edge detection, emboss, and more

## 🛠️ Technical Highlights

### Performance Optimization
- **60% faster blur processing** using separable 1D convolution (0.0686s vs 0.2506s per frame)
- Efficient pointer-based pixel manipulation for minimal memory overhead
- Optimized Sobel edge detection using separable kernels

### Advanced Algorithms Implemented
- **Sobel Edge Detection**: Custom 3x3 gradient computation for both X and Y directions
- **Gaussian Blur**: Separable filter optimization (10 operations vs 25 for 2D kernel)
- **Gradient Magnitude**: Euclidean distance calculation from directional gradients
- **Color Quantization**: Discrete color palette reduction with blur preprocessing
- **Depth Estimation**: Integration with Depth Anything V2 neural network via ONNX Runtime

### Computer Vision Techniques
- Convolution operations from scratch
- Pixel-level manipulation and memory management
- Real-time facial recognition with bounding box visualization
- Selective colorization (color faces on grayscale backgrounds)
- Vignette effects and custom color space transformations

## 📋 Prerequisites

- **OpenCV 4.13.0**
- **C++11 or higher**
- **Visual Studio 2019/2022** (Windows) or **CMake** (cross-platform)
- **ONNX Runtime** (optional, for depth estimation)
- Webcam or video input device

## 🚀 Quick Start

### Windows (Visual Studio)

1. **Clone the repository**
```bash
   git clone https://github.com/yourusername/realtime-video-filters.git
   cd realtime-video-filters
```

2. **Install OpenCV**
   - Download OpenCV 4.13.0 from [opencv.org](https://opencv.org/releases/)
   - Extract to `C:\opencv`
   - Add `C:\opencv\build\x64\vc16\bin` to your PATH

3. **Download Haar Cascade Model**
```bash
   # The haarcascade file is included in the repo (527 KB)
   # Or download from OpenCV repository if needed
```

4. **Build the project**
```bash
   # Open Visual Studio
   # File > Open > Project/Solution
   # Select the .sln file
   # Build > Build Solution (F7)
```

5. **Run**
```bash
   # Run from Visual Studio (F5) or from command line:
   cd x64\Debug
   vidDisplay.exe
```

### Linux/Mac (CMake)
```bash
# Install OpenCV
sudo apt-get install libopencv-dev  # Ubuntu/Debian
brew install opencv                  # macOS

# Build
mkdir build && cd build
cmake ..
make

# Run
./vidDisplay
```

## 🎮 Usage

### Basic Controls

| Key | Function |
|-----|----------|
| `g` | Toggle standard grayscale |
| `h` | Toggle custom weighted grayscale (blue-dominant) |
| `p` | Toggle sepia tone with vignette |
| `b` | Toggle 5x5 Gaussian blur |
| `x` | Toggle Sobel X (vertical edges) |
| `y` | Toggle Sobel Y (horizontal edges) |
| `m` | Toggle gradient magnitude (all edges) |
| `l` | Toggle blur-quantize (10-level color palette) |
| `f` | Toggle face detection |
| `n` | Toggle negative filter |
| `e` | Toggle emboss effect |
| `k` | Toggle colorful face (color faces only) |
| `s` | Save current frame as `captured_frame.jpg` |
| `q` | Quit application |

### Loading Static Images
```bash
# Load an image instead of live video
vidDisplay.exe path/to/image.jpg

# Switch between image and video mode
# Press 'i' to load a new image
# Press 'v' to switch back to video mode
```

## 📊 Performance Metrics

| Filter | Processing Time (per frame) | FPS |
|--------|----------------------------|-----|
| Standard Grayscale | 0.0050s | 200 |
| 5x5 Blur (2D) | 0.2506s | 4 |
| 5x5 Blur (Separable) | 0.0686s | 14.5 |
| Sobel X/Y | 0.0120s | 83 |
| Face Detection | 0.0450s | 22 |

*Tested on Intel i7-10750H, 1920x1080 resolution*

## 🧠 How It Works

### Gaussian Blur Optimization

Traditional 2D convolution requires 25 multiplications per pixel:
```cpp
// 5x5 kernel = 25 operations
int kernel[5][5] = {
    {1,2,4,2,1},
    {2,4,8,4,2},
    ...
};
```

Optimized separable approach requires only 10:
```cpp
// Two 1x5 passes = 10 operations (5 + 5)
int kernel[5] = {1,2,4,2,1};
// Apply horizontally, then vertically
```

**Result**: 60% performance improvement

### Face Detection with Selective Colorization

1. Convert entire frame to grayscale
2. Use Haar Cascade to detect face regions
3. Restore original RGB values only within face bounding boxes
4. Draw border for clear differentiation

### Sobel Edge Detection

Uses separable filters for efficiency:
```
Sobel X = [1,2,1]ᵀ × [-1,0,1]
Sobel Y = [1,2,1] × [-1,0,1]ᵀ
```

Gradient magnitude: `√(Sx² + Sy²)`

## Project Structure
```
src/
├── vidDisplay.cpp      # Main application loop and UI
├── filter.cpp          # All filter implementations
├── filter.h            # Filter function declarations
├── faceDetect.cpp      # Haar Cascade face detection
└── faceDetect.h        # Face detection interface

models/
├── haarcascade_frontalface_alt2.xml  # Pre-trained face detection model
└── model_download.md                  # Instructions for ONNX depth model

docs/
├── project_report.pdf  # Detailed technical report
└── images/             # Demo images
```

## Future Enhancements

- [ ] Real-time depth-based background blur (portrait mode)
- [ ] Additional artistic filters (oil painting, watercolor)
- [ ] GPU acceleration with CUDA
- [ ] Multi-face tracking with unique IDs
- [ ] Video file output recording
- [ ] Parameter tuning UI sliders

## Technical Documentation

For detailed implementation notes, algorithm explanations, and performance analysis, see the [full project report](docs/project_report.pdf).

## Acknowledgments

- **OpenCV**: [https://opencv.org](https://opencv.org)
- **ONNX Runtime**: [https://onnxruntime.ai](https://onnxruntime.ai)
- Course: CS5330 Pattern Recognition & Computer Vision

## 📧 Contact

**Ahilesh Vadivel**  
📧 vadivel.a@northeastern.edu  
💼 [LinkedIn](www.linkedin.com/in/ahilesh-vadivel-a385ab205)  
🌐 [Portfolio](https://vadivel-ahi.github.io/).

---

⭐ If you found this project interesting, please consider giving it a star!
