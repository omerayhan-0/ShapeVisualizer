# ShapeVisualizer

Qt/C++ tabanlı, dinamik plugin mimarisine sahip bir masaüstü görselleştirme uygulaması. UDP üzerinden gelen koordinat verilerini, çalışma anında yüklenen bağımsız eklentiler (plugin) aracılığıyla ekranda gerçek zamanlı olarak görselleştirir.

Qt/C++ based desktop visualization application with a dynamic plugin architecture. It visualizes coordinate data received over UDP in real time, through independently loaded plugins.

---

## 🇹🇷 Türkçe

### Proje Hakkında

ShapeVisualizer, bir **host uygulaması + ortak arayüz + bağımsız plugin'ler** şeklinde modüler olarak tasarlanmıştır. Host uygulaması (`MainWindow`), hangi şekil eklentilerinin (Circle, Square, vb.) var olduğunu bilmeden, `plugins/` klasörünü çalışma anında tarayarak bulduğu her plugin'i otomatik olarak yükler ve arayüze ekler.

### Mimari

```
Host (MainWindow)
 ├─ PluginManager   → .dylib/.dll/.so dosyalarını QPluginLoader ile yükler
 ├─ UdpWorker       → UDP dinleme, QMutex korumalı mailbox ile thread-safe veri aktarımı
 ├─ SecureLogger    → Hatalı plugin loglarını şifreli olarak kaydeder
 └─ IShapePlugin*   → Yüklenen her plugin, ortak arayüzü uygular
     ├─ CirclePlugin → CircleView
     └─ SquarePlugin → SquareView
```

Detaylı sınıf diyagramı için bkz. proje içindeki UML görseli.

### Gereksinimler

- **Qt 6.5+** (Core, Widgets, Network modülleri)
- **CMake 3.19+**
- **C++17** destekleyen bir derleyici (Clang, GCC, MSVC)
- **OpenSSL** (log şifreleme kütüphanesi için gerekli)

#### OpenSSL Kurulumu (Platforma Göre)

**macOS (Homebrew):**
```bash
brew install openssl
```
CMake, Homebrew'in OpenSSL'ini **otomatik olarak** bulur (`if(APPLE)` bloğu, `brew --prefix openssl` çıktısını kullanır) — ek bir parametre vermeniz genellikle gerekmez.

**Windows:**
- En kolay yol: [vcpkg](https://github.com/microsoft/vcpkg) ile kurulum:
  ```powershell
  vcpkg install openssl:x64-windows
  ```
  Sonra CMake'i vcpkg toolchain dosyasıyla çalıştırın:
  ```powershell
  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg-yolu]/scripts/buildsystems/vcpkg.cmake
  ```
- Alternatif: [Shining Light Productions](https://slproweb.com/products/Win32OpenSSL.html) üzerinden hazır Windows kurulum paketi (Win64 OpenSSL) indirilebilir. Kurulum sonrası CMake bulamazsa `-DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"` gibi bir parametre ile yönlendirin.

**Linux (Debian/Ubuntu tabanlı):**
```bash
sudo apt update && sudo apt install libssl-dev
```
**Linux (Fedora/RHEL tabanlı):**
```bash
sudo dnf install openssl-devel
```

#### `externalLibs/` İçindeki SecureLogger Kütüphanesi

`externalLibs/` klasöründe, log şifreleme için kullanılan `SecureLogger` kütüphanesinin **iki platforma özel derlenmiş** hâli bulunuyor:

- `libSecureLogLib.a` → macOS (Apple Silicon/ARM64) için
- `libSecureLogLib_win.lib` → Windows için (MinGW-w64 ile derlenmiştir)

`CMakeLists.txt`, derleme yapılan platforma göre (`if(WIN32)` / `else()`) doğru dosyayı **otomatik olarak** seçer.

**⚠️ Önemli not:** Windows kütüphanesi **MinGW** derleyicisiyle üretilmiştir. Eğer Qt Creator'da **MSVC (Visual Studio)** tabanlı bir Kit kullanıyorsanız, bu dosya **uyumsuz olabilir** ve link hatası alabilirsiniz — bu durumda MinGW tabanlı bir Kit'e geçmeniz ya da kütüphanenin MSVC ile derlenmiş bir sürümünün temin edilmesi gerekir. Linux için şu an ayrı bir derleme bulunmamaktadır; bu, projenin bilinen bir kısıtıdır.

### Kurulum ve Derleme

```bash
git clone https://github.com/omerayhan-0/ShapeVisualizer.git
cd ShapeVisualizer
cmake -S . -B build
cmake --build build
```

Ya da Qt Creator ile: `CMakeLists.txt` dosyasını açıp normal şekilde derleyin/çalıştırın.

### Proje Yapısı

```
ShapeVisualizer/
├── CMakeLists.txt          # Ana proje yapılandırması, plugin'leri otomatik tarar
├── main.cpp
├── mainwindow.h/.cpp       # Host uygulaması: plugin yükleme, UDP dağıtımı, grid arayüzü
├── pluginmanager.h/.cpp    # QPluginLoader ile .dylib/.dll/.so yükleme ve bellek yönetimi
├── udpworker.h/.cpp        # Thread-safe UDP dinleme (QMutex tabanlı mailbox)
├── shared/
│   └── IShapePlugin.h      # Point struct + tüm plugin'lerin uyması gereken arayüz
├── plugins/
│   ├── circle/             # Örnek plugin: daire çizer
│   └── square/             # Örnek plugin: kare çizer
├── externalLibs/           # SecureLogger statik kütüphanesi (.a + header)
├── tools/
│   └── udp_test_sender.py  # Test amaçlı UDP koordinat gönderme scripti
└── logs/                   # Çalışma anında otomatik oluşur, şifreli hata logları
```

### Plugin Sistemi

Yeni bir plugin eklemek için:
1. `plugins/` altında yeni bir klasör açın (örn. `plugins/triangle/`)
2. `IShapePlugin` arayüzünü uygulayan bir sınıf yazın (`pluginName()`, `getWidget()`, `handleMessage(Point)`)
3. Kendi `CMakeLists.txt`'nizi ekleyin (mevcut `circle`/`square` örneklerine bakın)

Host uygulaması **kodunda hiçbir plugin ismi sabit yazılı değildir** — CMake ve `MainWindow`, `plugins/` klasörünü otomatik tarar. Yeni bir plugin klasörü eklemek, hiçbir host kodu değişikliği gerektirmez.

Arayüzde, her başarıyla yüklenen plugin için sağ panelde bir açma/kapama düğmesi otomatik oluşturulur; açık olan plugin'ler sol taraftaki grid alanında eşit paylaşımlı olarak gösterilir.

### UDP Veri Formatı

Uygulama, `25001` portundan aşağıdaki formatta (little-endian, 2 adet 32-bit float) UDP paketleri dinler:

```c
struct Point {
    float x;
    float y;
};
```

### Test Scripti

`tools/udp_test_sender.py`, test amaçlı rastgele koordinatlar üretip uygulamaya UDP ile gönderir:

```bash
python3 tools/udp_test_sender.py
```

### Şifreli Log Sistemi

Bir plugin yüklenemezse (bozuk dosya, geçersiz format vb.), hata mesajı **düz metin olarak değil**, `externalLibs/` altındaki statik `SecureLogger` kütüphanesi aracılığıyla **şifrelenerek** proje kökündeki `logs/log.enc` dosyasına yazılır. Bu dosyanın konumu, çalışma anında (`findLogRoot()`) dinamik olarak, kaynak kökte bulunan `CMakeLists.txt` ve `plugins/` işaretlerine göre tespit edilir — sabit bir yol varsayılmaz, bu sayede farklı bilgisayarlarda da doğru çalışır.

### Platform Desteği

Plugin dosya uzantısı (`.dylib` / `.dll` / `.so`), derleme anında `#ifdef Q_OS_WIN` / `Q_OS_MAC` / `#else` ile otomatik seçilir. Şu ana kadar macOS üzerinde tam olarak test edilmiştir; Windows/Linux derlemesi teorik olarak desteklenir ancak ayrıca doğrulanmamıştır.

---

## 🇬🇧 English

### About

ShapeVisualizer is designed as a modular **host application + shared interface + independent plugins** system. The host application (`MainWindow`) has no compile-time knowledge of which shape plugins (Circle, Square, etc.) exist — it scans the `plugins/` directory at runtime and automatically loads and displays whatever it finds.

### Architecture

```
Host (MainWindow)
 ├─ PluginManager   → loads .dylib/.dll/.so files via QPluginLoader
 ├─ UdpWorker       → UDP listening, thread-safe data transfer via a QMutex-protected mailbox
 ├─ SecureLogger    → encrypts and logs plugin-loading errors
 └─ IShapePlugin*   → every loaded plugin implements this common interface
     ├─ CirclePlugin → CircleView
     └─ SquarePlugin → SquareView
```

See the UML diagram included in the project for full class relationships.

### Requirements

- **Qt 6.5+** (Core, Widgets, Network modules)
- **CMake 3.19+**
- A **C++17**-capable compiler (Clang, GCC, MSVC)
- **OpenSSL** (required by the log-encryption library)

#### Installing OpenSSL (per platform)

**macOS (Homebrew):**
```bash
brew install openssl
```
CMake **automatically** locates Homebrew's OpenSSL (an `if(APPLE)` block uses the output of `brew --prefix openssl`) — you generally don't need to pass anything manually.

**Windows:**
- Easiest path: install via [vcpkg](https://github.com/microsoft/vcpkg):
  ```powershell
  vcpkg install openssl:x64-windows
  ```
  Then configure CMake with the vcpkg toolchain file:
  ```powershell
  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg-path]/scripts/buildsystems/vcpkg.cmake
  ```
- Alternative: download a prebuilt Windows installer (Win64 OpenSSL) from [Shining Light Productions](https://slproweb.com/products/Win32OpenSSL.html). If CMake can't find it afterward, point it explicitly: `-DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"`.

**Linux (Debian/Ubuntu-based):**
```bash
sudo apt update && sudo apt install libssl-dev
```
**Linux (Fedora/RHEL-based):**
```bash
sudo dnf install openssl-devel
```

#### SecureLogger Library in `externalLibs/`

The `externalLibs/` folder contains **two platform-specific prebuilt versions** of the `SecureLogger` library used for encrypted logging:

- `libSecureLogLib.a` → for macOS (Apple Silicon/ARM64)
- `libSecureLogLib_win.lib` → for Windows (built with MinGW-w64)

`CMakeLists.txt` automatically selects the correct file based on the build platform (`if(WIN32)` / `else()`).

**⚠️ Important:** The Windows library was built with **MinGW**. If your Qt Creator Kit uses **MSVC (Visual Studio)** instead, this file may be **incompatible** and produce a link error — in that case, switch to a MinGW-based Kit, or obtain an MSVC-compiled version of the library. No separate build is currently available for Linux; this is a known limitation of the project.

### Build

```bash
git clone https://github.com/omerayhan-0/ShapeVisualizer.git
cd ShapeVisualizer
cmake -S . -B build
cmake --build build
```

Or open `CMakeLists.txt` directly in Qt Creator and build/run normally.

### Project Structure

```
ShapeVisualizer/
├── CMakeLists.txt          # Main project config; auto-discovers plugins
├── main.cpp
├── mainwindow.h/.cpp       # Host app: plugin loading, UDP dispatch, grid UI
├── pluginmanager.h/.cpp    # Loads .dylib/.dll/.so via QPluginLoader, memory management
├── udpworker.h/.cpp        # Thread-safe UDP listening (QMutex-based mailbox)
├── shared/
│   └── IShapePlugin.h      # Point struct + the interface every plugin must implement
├── plugins/
│   ├── circle/             # Example plugin: draws a circle
│   └── square/             # Example plugin: draws a square
├── externalLibs/           # SecureLogger static library (.a + header)
├── tools/
│   └── udp_test_sender.py  # Sends test UDP coordinates for development/testing
└── logs/                   # Auto-created at runtime; encrypted error logs
```

### Plugin System

To add a new plugin:
1. Create a new folder under `plugins/` (e.g. `plugins/triangle/`)
2. Implement a class conforming to `IShapePlugin` (`pluginName()`, `getWidget()`, `handleMessage(Point)`)
3. Add your own `CMakeLists.txt` (see the existing `circle`/`square` examples)

The host application contains **no hardcoded plugin names anywhere** — CMake and `MainWindow` both scan the `plugins/` directory automatically. Adding a new plugin folder requires zero changes to host code.

In the UI, an open/close toggle button is automatically generated in the right-hand panel for every successfully loaded plugin; active plugins are displayed in the grid area on the left, sharing space equally.

### UDP Data Format

The application listens on port `25001` for UDP packets in the following format (little-endian, two 32-bit floats):

```c
struct Point {
    float x;
    float y;
};
```

### Test Script

`tools/udp_test_sender.py` generates random test coordinates and sends them to the application over UDP:

```bash
python3 tools/udp_test_sender.py
```

### Encrypted Logging

When a plugin fails to load (corrupt file, invalid format, etc.), the error is **not written as plain text** — it is encrypted via the static `SecureLogger` library in `externalLibs/` and written to `logs/log.enc` at the project root. This path is resolved dynamically at runtime (`findLogRoot()`) by walking up from the executable's location until both `CMakeLists.txt` and `plugins/` are found — no fixed path depth is assumed, so it works correctly across different machines.

### Platform Support

The plugin file extension (`.dylib` / `.dll` / `.so`) is selected automatically at compile time via `#ifdef Q_OS_WIN` / `Q_OS_MAC` / `#else`. Fully tested on macOS to date; Windows/Linux builds are theoretically supported but not yet separately verified.
