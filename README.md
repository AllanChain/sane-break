<p align="center">
  <img src="./resources/images/icon.svg" width="150" height="150">
</p>
<h1 align="center">Sane Break</h1>
<p align=center>
  <a href="https://github.com/AllanChain/sane-break/releases">
    <img src="https://img.shields.io/github/v/release/AllanChain/sane-break" alt="GitHub release">
  </a>
  <img src="https://img.shields.io/badge/built_with-Qt_6-Qt" alt="Built with Qt 6">
</p>

Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow.

## 🤔 Why another break reminder?

Most break reminder apps send notifications before the break is scheduled, but that merely informs me that a break is _going to happen_, rather than telling me it's _happening right now_. Even with a warning, when the break actually happens I'm too often deeply focused on a task. Then, the program either forces the break, interrupting my work, or I can use a "skip" or "postpone" button. Accepting abrupt interruption is too harsh, so I tend to click "skip" or "postpone". Then, that becomes an almost instinctive habit, and before I know it, hours have passed without a single break.

In contrast, Sane Break allows me to take ownership of _when_ to stop and break. It starts by providing a gentle, non-intrusive reminder through a flashing window. The break officially begins as soon as I naturally pause after finishing my current task. This way, Sane Break offers the flexibility to wrap up my work and take a break on my terms, rather than forcing an abrupt interruption.

Sane Break aims to strike a balance between respecting your workflow and ensuring you take the breaks you need.

For more details about the motivation behind Sane Break, you can refer to [No break reminder works (for me)](https://allanchain.github.io/blog/post/sane-break-intro/).

## Two phase system

Sane Break uses a two-phase system:

- Phase 1: A small, unobtrusive reminder that encourages you to find a good stopping point.
- Phase 2: The reminder grows to full screen during the actual break.

Transition to phase 2 happens when you stop working on your computer, or when you ignore the reminder and keep working for too long (configurable).

This design was inspired by [RSIBreak](https://apps.kde.org/rsibreak/) and has similarities to [WorkRave](https://workrave.org/). Sane Break has some other improvements:

- No big buttons for skipping or postponing to avoid mindless dismissals.
- [Cross-platform support](#platform-support)
- modern framework and UI style built with Qt6

## 🎬 Demo video

https://github.com/user-attachments/assets/35002673-ce83-4848-bd5e-6cc6dbbd4c8c

<details>
  <summary>📷 <b>Screenshots</b></summary>

![flashing](https://github.com/user-attachments/assets/1ec11caf-3b3d-4429-881d-4d909ec4c3c7)
![fullscreen](https://github.com/user-attachments/assets/59164505-3446-4ef7-b0e0-5ffe0a607c44)
![macos](https://github.com/user-attachments/assets/2f28406e-0601-4786-ab5d-194a7cfae050)

</details>

## Platform support

### Linux

KDE 6 (on Arch-based distros) support is prioritized. Others may work too.

Sane break uses Qt6, [wlr layer shell](https://wayland.app/protocols/wlr-layer-shell-unstable-v1), and [idle notify](https://wayland.app/protocols/ext-idle-notify-v1) protocols. In theory it supports:
- **X11**: all.
- **Wayland**: compositors supporting wlr layer shell and idle notify protocols, and distros with layer-shell-qt version 6.

See [Compiling from source](#compiling-from-source) for details.

### macOS

Both x86 and ARM version of macOS 12 or newer are supported.

### Windows

x86 version of Windows 10 or newer is supported. Older versions may work too.

## Compiling from source

For Linux users with Arch-based distros, it's recommended to install the AUR package:
```bash
yay -S sane-break
```

If you want to enable just X11 or Wayland support, or you are using other distros (which I didn't test), you are strongly encouraged to install Sane Break by compiling from source.
It will automatically enable X11 or Wayland support based on libraries found.
For Wayland support, make sure you have `layer-shell-qt` for Qt6 installed.

Required packages for Arch Linux:
```bash
yay -S qt6-base qt6-multimedia
yay -S libxss # X support
yay -S layer-shell-qt # Wayland support
```

Required packages for KDE Neon (based on Ubuntu 24.04, so probably the same for other Debian-based distros):
```bash
apt install qt6-base-dev qt6-multimedia-dev
apt install libxss-dev # X support
apt install qt6-wayland-dev liblayershellqtinterface-dev # Wayland support
```

Required packages for Guix (see also [#23](https://github.com/AllanChain/sane-break/issues/23)):
```bash
guix shell --container make cmake gcc-toolchain pkg-config vulkan-headers \
  qtbase@6 qtmultimedia \
  libxkbcommon libxscrnsaver libx11 \  # X support
  layer-shell-qt qtwayland@6 wayland   # Wayland support
```

Then, use CMake to compile from source:

```bash
git clone https://github.com/AllanChain/sane-break
cd sane-break
mkdir build
cd build
cmake ..
cmake --build . --parallel
sudo cmake --install .
```

## FAQ

### How to exit?

Right-click the menu icon and hover over the "Postpone" menu. The "Quit" menu is there. This design is intended to make users think twice about whether they want to postpone breaks or really quit the software.

### How to start Sane Break on startup?

- Linux: Whatever way you like
- macOS: Open "System Preferences" and search for "Login Items". Click "+" and add Sane Break to login items
- Windows: Use <kbd>Win</kbd> + <kbd>R</kbd> and type `shell:startup`. Put `sane-break.exe` or its shortcut there

### Will Sane Break pause when I'm away?

Yes, Sane Break will automatically pause and stop prompting breaks if there is no user input for a configurable amount of time, determined by the "Pause if idle for" setting. Additionally, if the idle time extends beyond a certain threshold, Sane Break will reset the break timer and break cycle, starting from a small break. These thresholds can be configured using the "Reset break after paused for" and "Reset cycle after paused for" settings.

**Note for Wayland** (and possibly other platforms): Some applications, such as mpv, simulate user inputs to prevent the computer from sleeping automatically. Sane Break cannot distinguish between simulated and real inputs, so it will treat these simulations as active use. This means breaks can still occur while watching videos.

### Where are settings stored?

The default locations for settings are:
- On Linux and macOS: ` ~/.config/SaneBreak/SaneBreak.ini`
- On Windows: `%APPDATA%\SaneBreak\SaneBreak.ini`

Beside default locations, Qt also supports reading settings from [other locations](https://doc.qt.io/qt-6/qsettings.html#locations-where-application-settings-are-stored).

If you want to make Sane Break portable, especially on Windows, you can put an empty file named `SaneBreak.ini` next to the `.exe` file and Sane Break will automatically pick that file for settings.

### What if I really want to skip a break?

You can postpone or quit via tray icon within a short time even after the break starts.

However, if you really want to quit when Sane Break is set to fullscreen, you can kill the app via system command line. For example:
- KDE: Enable KRunner with <kbd>Alt</kbd> + <kbd>Space</kbd> and run `killall sane-break`
- macOS: Enable Spotlight with <kbd>Cmd</kbd> + <kbd>Space</kbd>, open Terminal, and run `killall sane-break`
- Windows: Press <kbd>Win</kbd> + <kbd>R</kbd> and run `taskkill /IM sane-break /F`
