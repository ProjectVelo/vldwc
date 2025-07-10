# VLDWC (Velo Desktop Window Compositor)

> **VLDWC**, or *Velo Desktop Window Compositor*, is the core compositing and display system for **VeloOS**. It’s designed for performance, minimalism, and smooth GPU-accelerated rendering using OpenGL and Vulkan backends.

---

## 🎯 Purpose

- Provide a **compositing window system** with modern UX
- Render UI directly using **GPU acceleration**
- Modular separation of **Display Server** and **Window Manager**
- Deep integration with **VeloOS GUI APIs** (`libveloui`, `libswipeui`)

---

## 🧩 System Architecture

```text
+----------------------------+
|  VeloOS Kernel (V4L, DRM) |
+----------------------------+
            │
     +------▼--------+
     |   VLDWC Core  |
     +---------------+
            │
+-----------┼-----------+
|                       |
▼                       ▼
Window Manager       Compositor
(vldwm)              (vldcomp)
````

---

## ⚙️ Modules Overview

| Module     | Description                             |
| ---------- | --------------------------------------- |
| `vldcomp`  | Core compositor engine                  |
| `vldwm`    | Window manager (position, size, layout) |
| `vldshm`   | Shared memory and IPC layer             |
| `vldgpu`   | GPU Backend (OpenGL/Vulkan)             |
| `vldinput` | Handles mouse, keyboard, touch input    |
| `vldtheme` | Theme engine (colors, fonts, UI skins)  |

---

## 🚀 Key Features

* True alpha blending and transparency
* **Double/Triple Buffering** for tear-free rendering
* Native **VLDUI Toolkit** for consistent UI design
* DPI-aware rendering with retina scaling
* Integrates with `vldlogin`, `vldsession`, and `vldpanel`

---

## 📦 File Structure

| Path                        | Contents                      |
| --------------------------- | ----------------------------- |
| `/system/display/vldwc/`    | Core binaries and libraries   |
| `/system/config/vldwc.conf` | Compositor configuration file |
| `/system/theme/`            | Themes, wallpapers, assets    |

---

## 🛠️ Example Config (`vldwc.conf`)

```ini
[display]
backend = "vulkan"
vsync = true
dpi_scale = 1.5
theme = "AetherDark"

[input]
mouse_accel = false
key_repeat = true

[performance]
buffering = "triple"
gpu_optimization = true
```

---

## 🔮 Roadmap

* ✅ Multi-monitor support (multi-head)
* ✅ Optional Wayland protocol support
* 🕓 Blur, shadows, rounded corners
* 🕓 GPU animation via VeloAnim API
* 🕓 Live theme editor and previewer

---

## 🎉 CLI Commands

```bash
vldwc --start                # Start the compositor
vldwc --reload               # Reload the configuration
vldwc --debug                # Show debug overlay
vldwc --backend=opengl       # Switch to OpenGL backend
```

---

## 💡 Developer Tips

* Use `vldwc-pipe` for ultra-low-latency IPC
* Build GUI apps with `libvldui` for native GPU-accelerated UI
* Package GUI apps as `.app` (Velo Application Bundle)

---

> “VLDWC isn’t just a compositor — it’s the heartbeat of VeloOS’s visual experience.”

---

📄 **Suggested File Location:**

* `/docs/specs/vldwc.md`
---