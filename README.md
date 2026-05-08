[![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/burakenez/gd32e51x-cmake-vscode)](https://github.com/burakenez/gd32e51x-cmake-vscode/tags/)

# GD32E51x Demo Suites CMake Visual Studio Code Integration

🚀 Welcome to the **GD32E51x Demo Suites CMake Visual Studio Code** repository! This project integrates the CMake build system with Visual Studio Code for **_[GD32E51x Demo Suites V1.4.0](https://www.gd32mcu.com/en/download)_** and **_[GD32E51x Firmware Library V1.4.0](https://www.gd32mcu.com/en/download)_**, running entirely inside a container — **no local toolchain installation needed**.

> 🐳 The development environment is fully containerized. The compiler, debugger, CMake, Ninja, and OpenOCD are all pre-installed inside a Docker/Podman container that VS Code manages automatically.

---

## 📋 Table of Contents

- [✨ Features](#-features)
- [🛠️ Requirements](#️-requirements)
- [🔧 Getting Started](#-getting-started)
  - [1. Install WSL 2](#1-️-install-wsl-2-windows-only)
  - [2. Install Podman](#2--install-podman)
  - [3. Install VS Code and the Dev Containers Extension](#3--install-vs-code-and-the-dev-containers-extension)
  - [4. Clone the Repository](#4--clone-the-repository)
  - [5. Open a Project in VS Code](#5--open-a-project-in-vs-code)
  - [6. Build the Project](#6--build-the-project)
  - [7. Flash and Debug](#7--flash-and-debug)
- [⌨️ VS Code Tasks](#️-vs-code-tasks)
- [📦 Drivers and Middlewares](#-drivers-and-middlewares)
- [📂 Folder Structure](#-folder-structure)
- [🐳 Toolchain Container](#-toolchain-container)
- [🛟 Troubleshooting](#-troubleshooting)

---

## ✨ Features

- **📦 Zero-Setup Toolchain:** Everything runs inside a container — no manual GCC or OpenOCD installation required.
- **🔧 Pre-configured Toolchain:** ARM GNU GCC (`gcc-arm-none-eabi`) and GigaDevice-patched **xpack-openocd 0.12.0-6** with full GD32 flash algorithm support.
- **🐞 Robust Debug Configuration:** Pre-configured `launch.json` with live watch, SVD peripheral views, and one-click GDB debugging via OpenOCD.
- **🧩 Rich Extension Support:** Extensions like `ms-vscode.cmake-tools`, `marus25.cortex-debug`, `mcu-debug.peripheral-viewer`, and `mcu-debug.memory-view` are automatically installed inside the container — peripheral viewers, RTOS views, memory views, and GNU map file tools provide a comprehensive debugging experience.
- **⚙️ Works on Windows (WSL2) and Linux:** The container abstracts away the host OS. Native macOS support (build + debug) is on the roadmap — contributions and feedback are welcome.

---

## 🛠️ Requirements

| Tool | Purpose |
|---|---|
| [VS Code](https://code.visualstudio.com/) | IDE |
| [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) | Opens the project inside the container |
| [Podman Desktop](https://podman-desktop.io/) | Container engine (Docker works too) |
| WSL 2 *(Windows only)* | Linux environment required by Dev Containers on Windows |
| [Git](https://git-scm.com/downloads) | Clone the repository |

---

## 🔧 Getting Started

### 1. 🖥️ Install WSL 2 *(Windows only)*

> 🐧 **Linux users can skip this step.**

Open **PowerShell as Administrator** and run:

```powershell
wsl --install
```

This installs WSL 2 with Ubuntu by default. **Restart your computer** when prompted.

Verify the installation:

```powershell
wsl --list --verbose
```

The Ubuntu distribution should show **Version 2**. If it shows Version 1, upgrade it:

```powershell
wsl --set-version Ubuntu 2
```

---

### 2. 🐳 Install Podman

**Windows (inside WSL Ubuntu)** / **Linux:**
```bash
sudo apt update && sudo apt install -y podman
systemctl --user enable --now podman.socket
```

Verify Podman is working:
```bash
podman --version
```

#### Tell VS Code to use Podman

Open VS Code settings (`Ctrl+,`), search for **dockerPath**, and set `dev.containers.dockerPath` to `podman`.

> 💡 If you prefer Docker, install [Docker Desktop](https://www.docker.com/products/docker-desktop/) instead and skip the VS Code setting above — Dev Containers detects it automatically.

---

### 3. 🧩 Install VS Code and the Dev Containers Extension

1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Open VS Code, go to the **Extensions** panel (`Ctrl+Shift+X`), search for **Dev Containers**, and install:
   - **Dev Containers** (`ms-vscode-remote.remote-containers`)

---

### 4. 📥 Clone the Repository

```bash
git clone https://github.com/burakenez/gd32e51x-cmake-vscode.git
```

> ⚠️ Avoid long directory paths — they can cause build failures on Windows.

---

### 5. 📂 Open a Project in VS Code

Opening a project is a two-step process: first start the container from the repository root, then open the specific project inside it.

**Step 1 — Start the container**

1. Open VS Code.
2. Go to **File → Open Folder** and select the **repository root** (the folder that contains `.devcontainer/`).
3. VS Code detects `.devcontainer/devcontainer.json` and shows a notification:
   > *"Folder contains a Dev Container configuration file. Reopen folder to develop in a container."*
4. Click **Reopen in Container**.

VS Code builds the container image *(first time only, a few minutes)* and opens the repository root inside it. All tools — compiler, debugger, CMake, Ninja — are available immediately.

**Step 2 — Open the project**

Once inside the container, go to **File → Open Folder** and select the project you want to work on:
```
Projects/<BoardName>/<ProjectName>
```

VS Code reopens that folder **inside the same running container**. The project's `.vscode/launch.json` and `tasks.json` become active, and `${workspaceFolder}` resolves correctly to the project directory.

> ⚡ On subsequent opens VS Code reconnects to the existing container instantly.

---

### 6. 🔨 Build the Project

Inside the container, build using any of these methods:

- Click the **Build** button in the VS Code status bar.
- Press `Ctrl+Shift+B` and select **Build**.
- Press `Ctrl+Shift+P`, type **CMake: Build**, and press Enter.

Output files are placed in:
```
Projects/<BoardName>/<ProjectName>/Build/Debug/Application/
```

| Output file | Description |
|---|---|
| `Application.elf` | ELF with debug symbols — used for debugging |
| `Application.bin` | Raw binary — ready to flash |
| `Application.hex` | Intel HEX format |
| `Application.map` | Memory map |
| `Application.list` | Assembly listing |

---

### 7. 🐞 Flash and Debug

#### 🔌 Connect the debug probe (GD-Link)

**On Linux:** The USB device is directly accessible — no extra setup needed.

**On Windows (WSL 2):** USB devices are not automatically shared with WSL. Use [usbipd-win](https://github.com/dorssel/usbipd-win) to attach the probe:

**Step 1 — Install usbipd-win on Windows** *(one-time)*

Open **PowerShell as Administrator**:
```powershell
winget install usbipd
```

**Step 2 — Install client tools inside WSL** *(one-time)*

Open a WSL terminal:
```bash
sudo apt install linux-tools-generic hwdata usbutils
sudo update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20
```

**Step 3 — Grant USB device access** *(one-time, in WSL terminal)*

WSL 2 creates USB device nodes owned by `root:root` with mode `664`, which blocks write access from inside the devcontainer. Fix this permanently with a udev rule:

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="28e9", MODE="0666"' | sudo tee /etc/udev/rules.d/99-gdlink.rules && sudo udevadm control --reload-rules
```

> 💡 If your WSL 2 distro doesn't run udevd (check with `systemctl is-active systemd-udevd`), apply the permission manually after each `usbipd attach`:
> ```bash
> sudo chmod a+rw $(ls /dev/bus/usb/*/*)
> ```

**Step 4 — Attach the GD-Link to WSL** *(each session)*

Open **PowerShell as Administrator** and list USB devices:
```powershell
usbipd list
```

Find the GD-Link entry — known VID:PID values are `28e9:0189`, `28e9:058f`, `28e9:0497`, `28e9:068f`, `28e9:f002`. Then bind *(first time only)* and attach:
```powershell
usbipd bind --busid <BUSID>
usbipd attach --wsl --busid <BUSID>
```

Verify it is visible inside WSL:
```bash
lsusb
```

You should see a line containing `GDMicroelectronics` or `GD-Link`.

> ⚠️ **Attach the GD-Link to WSL _before_ opening the devcontainer.** The container captures USB device nodes at startup — devices attached after the container starts will not be visible inside it. If you forget, see [GD-Link not detected after plugging in](#gd-link-not-detected-after-plugging-in) in the Troubleshooting section.

**Step 5 — Detach when done**

```powershell
usbipd detach --busid <BUSID>
```

#### ▶️ Start a debug session

1. Go to **Run and Debug** (`Ctrl+Shift+D`) in VS Code.
2. Select **Debug with OpenOCD** from the dropdown.
3. Press `F5` or click **Start Debugging**.

To flash without debugging, use the **Flash MCU** task (`Ctrl+Shift+B` → **Flash MCU**).

---

## ⌨️ VS Code Tasks

All tasks are available via `Ctrl+Shift+B`:

| Task | Description |
|---|---|
| **Build** | Compile the project with CMake + Ninja (Debug preset) |
| **Build and Flash** | Build then immediately flash the firmware |
| **Flash MCU** | Flash the last built `.elf` via OpenOCD |
| **Reset MCU** | Reset the target MCU via OpenOCD |
| **Mass Erase MCU** | Erase the entire flash memory |
| **OpenOCD Server** | Start a standalone OpenOCD server for external GDB clients |
| **Kill OpenOCD** | Terminate any running OpenOCD process |

---

## 📦 Drivers and Middlewares

### 1. **CMSIS**
- **Version:** `v6.3.0`
- **Path:** `Drivers/CMSIS`

### 2. **CMSIS/GD/GD32E51x**
- **Version:** `V1.4.0 (2026-02-09)`
- **Path:** `Drivers/CMSIS/GD/GD32E51x`
- **Source:** `GD32E51x Demo Suites V1.4.0`

### 3. **GD32E51x_usbhs_library**
- **Version:** `V1.4.0 (2026-02-10)`
- **Path:** `Drivers/GD32E51x_usbhs_library`
- **Source:** `GD32E51x Demo Suites V1.4.0`

### 4. **GD32E51x_usbd_library**
- **Version:** `V1.4.0 (2026-02-10)`
- **Path:** `Drivers/GD32E51x_usbd_library`
- **Source:** `GD32E51x Demo Suites V1.4.0`

### 5. **GD32E51x_standard_peripheral**
- **Version:** `V1.4.0 (2026-02-09)`
- **Path:** `Drivers/GD32E51x_standard_peripheral`
- **Source:** `GD32E51x Demo Suites V1.4.0`

### 6. **FreeRTOS**
- **Version:** `V11.3.0`
- **Path:** `Middlewares/FreeRTOS`
- **Source:** `FreeRTOS-Kernel`

### 7. **FatFs**
- **Version:** `R0.16`
- **Path:** `Middlewares/FatFs`
- **Source:** `FatFs by ChaN`

### 8. **lwip**
- **Version:** `STABLE-2.2.1`
- **Path:** `Middlewares/lwip`
- **Source:** `lwIP`

---

## 📂 Folder Structure

```plaintext
├── .devcontainer
│   ├── Containerfile                  # Container image definition
│   └── devcontainer.json              # VS Code Dev Container configuration
├── Drivers                            # Shared drivers (CMSIS, BSP, standard peripheral, USB)
├── Middlewares                        # Shared middleware libraries (FreeRTOS, FatFs, lwIP, ...)
├── Projects                           # Demo suite board projects
│   └── <BoardName>
│       └── <ProjectName>
│           ├── .vscode
│           │   ├── launch.json        # Debug configuration (OpenOCD + GDB + SVD)
│           │   ├── settings.json      # Workspace settings
│           │   └── tasks.json         # Build, flash, reset, and erase tasks
│           ├── Application
│           │   ├── Core
│           │   │   ├── Inc
│           │   │   │   ├── gd32e51x_it.h      # Interrupt handler declarations
│           │   │   │   ├── gd32e51x_libopt.h  # Library options
│           │   │   │   └── systick.h                                  # SysTick timer declarations
│           │   │   └── Src
│           │   │       ├── gd32e51x_it.c      # Interrupt service routines
│           │   │       ├── main.c                                     # Application entry point
│           │   │       ├── system_gd32e51x.c  # Clock and core initialisation
│           │   │       └── systick.c                                  # SysTick timer implementation
│           │   ├── Startup
│           │   │   └── startup_gd32e51x.s     # Reset handler and vector table
│           │   └── CMakeLists.txt                                     # Application build rules
│           ├── cmake
│           │   ├── arm-none-eabi-gcc.cmake  # ARM GCC toolchain settings
│           │   └── project.cmake            # Project-wide CMake configuration
│           ├── Drivers
│           │   ├── BSP/GD32E517R_START              # Board support package for GD32E517R_START
│           │   ├── CMSIS                                 # CMSIS for Cortex-M / GD32E51x
│           │   └── GD32E51x_standard_peripheral  # Standard peripheral library
│           ├── .clang-format                # Code style rules
│           ├── CMakeLists.txt               # Top-level CMake build file
│           ├── CMakePresets.json            # Debug / Release preset definitions
│           ├── gd32e51x_flash.ld  # Linker script (memory regions)
│           └── GD32E51x.svd             # SVD file for peripheral register views
├── Examples                           # Firmware library examples (one project per example)
│   └── <Peripheral>                   # e.g. ADC, CAN, TIMER, USART, ...
│       └── <ExampleName>              # Same project structure as Projects/<BoardName>/<ProjectName>
├── Template                           # Bare-minimum template project (firmware library)
│   └── ...                            # Same project structure as Projects/<BoardName>/<ProjectName>
└── README.md
```

---

## 🐳 Toolchain Container

The container is defined by `.devcontainer/Containerfile` and built **locally** the first time you open the project in VS Code — no registry login or pre-built image required.

### Included tools

| Tool | Version | Purpose |
|---|---|---|
| `gcc-arm-none-eabi` | system apt | ARM C/C++ compiler |
| `gdb-multiarch` | system apt | GDB debugger (symlinked as `arm-none-eabi-gdb`) |
| `binutils-multiarch` | system apt | `objdump` and `nm` required by cortex-debug |
| `cmake` + `ninja-build` | system apt | Build system |
| `xpack-openocd` | 0.12.0-6 | Installed to `/opt/xpack-openocd-0.12.0-6/` with GD32 flash algorithm support |

### Automatically installed VS Code extensions

| Extension | Purpose |
|---|---|
| `ms-vscode.cmake-tools` | CMake integration |
| `ms-vscode.cpptools` | C/C++ IntelliSense and debugging |
| `marus25.cortex-debug` | ARM Cortex-M debug adapter (OpenOCD, J-Link, ...) |
| `mcu-debug.peripheral-viewer` | SVD-based peripheral register viewer |
| `mcu-debug.memory-view` | Live memory inspector |
| `mcu-debug.rtos-views` | FreeRTOS task and queue viewer |
| `ms-vscode.hexeditor` | Hex file viewer |
| `dan-c-underwood.arm` | ARM assembly syntax highlighting |
| `zixuanwang.linkerscript` | Linker script syntax highlighting |
| `trond-snekvik.gnu-mapfiles` | GNU map file viewer |

### WSL2 USB fix

> 🔧 xpack-openocd's bundled `libhidapi-hidraw` cannot access `/dev/hidraw*` inside Docker+WSL2 containers. The `Containerfile` replaces it with `libhidapi-libusb` (same API, accesses the GD-Link via `/dev/bus/usb` instead).

> 🔧 The devcontainer uses `--device=/dev/bus/usb:/dev/bus/usb` to pass USB devices into the container. Device nodes are captured at container startup — **attach the GD-Link before opening the container**. If the board is replugged or attached after startup, rebuild the container (`Ctrl+Shift+P` → **Dev Containers: Rebuild Container**).

> ⏱️ The first container build takes a few minutes (downloads xpack-openocd from GitHub). Subsequent opens reuse the cached image instantly.

---

## 🛟 Troubleshooting

### GD-Link not detected after plugging in

The container captures USB device nodes at startup. If you attached the GD-Link after the container was already running:

1. In PowerShell, attach the board to WSL first:
   ```powershell
   usbipd attach --wsl --busid <BUSID>
   ```
2. In VS Code, rebuild the container:
   `Ctrl+Shift+P` → **Dev Containers: Rebuild Container**

### Container stuck during rebuild

If VS Code appears frozen while building or rebuilding the container (spinning indefinitely with no progress):

`Ctrl+Shift+P` → **Developer: Reload Window**

VS Code will reconnect and continue where it left off.

### Cortex-Debug extension error after container open

After extensions are installed inside the container, you may see:

> *"Cannot activate the 'Cortex-Debug' extension because it depends on the 'debug-tracker-vscode' extension, which is not loaded. Would you like to reload the window to load the extension?"*

Click **Reload Window**. This is a one-time ordering issue on first install — it will not appear again after the reload.
