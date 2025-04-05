<p align="center">
  <img src="./resources/images/icon.svg" width="150" height="150">
</p>
<h1 align="center">Sane Break</h1>
<p align=center>
  <a href="https://github.com/AllanChain/sane-break/releases">
    <img src="https://img.shields.io/github/v/release/AllanChain/sane-break" alt="GitHub release">
  </a>
  <img src="https://img.shields.io/badge/built_with-Qt_6-Qt" alt="Built with Qt 6">
  <a href="https://hosted.weblate.org/engage/sane-break/">
    <img src="https://hosted.weblate.org/widget/sane-break/sane-break/svg-badge.svg" alt="Translation status" />
  </a>
</p>

Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow.

<a href="https://apps.apple.com/us/app/sane-break/id6740942634">
  <img height="80" alt="Download on the Mac App Store" src="https://github.com/user-attachments/assets/2b6f067b-d275-4a2f-ba13-4fb9c6bd63fe">
</a>
&nbsp;
<a href="https://flathub.org/apps/io.github.AllanChain.sane-break">
    <img height="80" alt="Get it on Flathub" src="https://flathub.org/api/badge?locale=en">
</a>
&nbsp;
<a href="https://github.com/AllanChain/sane-break/releases">
  <img height="80" alt="Get it on GitHub" src="https://github.com/user-attachments/assets/16c6c35f-e657-4040-8b08-34282ea4ad43">
</a>

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
- [Cross-platform support](#installation)
- modern framework and UI style built with Qt6

## 🎬 Demo video

https://github.com/user-attachments/assets/35002673-ce83-4848-bd5e-6cc6dbbd4c8c

<details>
  <summary>📷 <b>Screenshots</b></summary>

![flashing](https://github.com/user-attachments/assets/1ec11caf-3b3d-4429-881d-4d909ec4c3c7)
![fullscreen](https://github.com/user-attachments/assets/59164505-3446-4ef7-b0e0-5ffe0a607c44)
![macos](https://github.com/user-attachments/assets/2f28406e-0601-4786-ab5d-194a7cfae050)

</details>

## Installation

### macOS

Download macOS `.dmg` file from the [release page](https://github.com/AllanChain/sane-break/releases). It is a universal bundle and both x86 and ARM version of macOS 12 or newer are supported.

### Windows

Download the Windows package (`sane-break-windows.zip`) from the [release page](https://github.com/AllanChain/sane-break/releases) and extract it. Put the `.exe` executable anywhere you like. x86 version of Windows 10 or newer is supported. Older versions may be supported too.

### Linux

Sane Break requires Qt>=6.4 for X11 support and Qt>=6.6 for Wayland support.
It works best on KDE Wayland, and X11 is also fully supported.
Sane Break will also work with Wayland compositors with [wlr layer shell](https://wayland.app/protocols/wlr-layer-shell-unstable-v1) and [idle notify](https://wayland.app/protocols/ext-idle-notify-v1) protocols support.
Although GNOME Wayland does not support these protocols, some special workaround are made so that Sane Break will work with GNOME.

#### Precompiled packages

##### Flatpak

Sane Break can be installed from Flathub with

```bash
flatpak install flathub io.github.AllanChain.sane-break
```

Note that if you are using GNOME, please download the GNOME shell extension for Sane Break from the [release page](https://github.com/AllanChain/sane-break/releases) and run:
```bash
gnome-extensions install sane-break-gnome-extension.zip
gnome-extensions enable sane-break@allanchain.github.io
```
You may need to logout and login from the Wayland session to make the extension work.

##### .deb packages

If you are using Ubuntu 24.10 (and above) or Debian Trixie (and above), you can install the `.deb` package directly from the [release page](https://github.com/AllanChain/sane-break/releases) since `v0.6.0`. And make sure to enable the GNOME shell extension with:
```bash
gnome-extensions enable sane-break@allanchain.github.io
```

The difference between Wayland Ubuntu Oracular build and Debian Trixie build is the layer shell support. If you are using a Debian-based distro and a Wayland compositor with layer shell protocol support (e.g. KDE Neon), you probably need the Debian Trixie build. Otherwise, the Ubuntu Oracular build is good to go.

#### Compiling from source

Assume all required packages described below is installed, you can use the following commands to compile and install Sane Break.

```bash
git clone https://github.com/AllanChain/sane-break
cd sane-break
mkdir build
cd build
cmake ..
cmake --build . --parallel
sudo cmake --install .
```

The above command will automatically enable X11 or Wayland support based on libraries found.
You can disable X11 or Wayland support using the option `-DAUTO_ENABLE_X11=OFF` or `-DAUTO_ENABLE_WAYLAND=OFF` during `cmake`.

To build with translations, install `qt6-tools` package (name may vary across distros).

##### Arch-based distros

For Arch-based distros, it's recommended to directly install the AUR package:
```bash
yay -S sane-break
```

If you want to build it manually, make sure the following packages are installed:
```bash
yay -S qt6-base qt6-multimedia
yay -S libxss # X support
yay -S layer-shell-qt # Wayland support
```

If you want to enable just X11 or Wayland support, you are strongly encouraged to install Sane Break by compiling from source.

##### Debian-based distros

For KDE users (e.g. KDE Neon), just install
```bash
apt install qt6-base-dev qt6-multimedia-dev
apt install libxss-dev # X support
apt install qt6-wayland-dev liblayershellqtinterface-dev # Wayland support
```

For GNOME users, `liblayershellqtinterface-dev` is optional, and please make sure to enable the GNOME shell extension `sane-break@allanchain.github.io` after building and installing Sane Break.

##### Others

For Guix (see also [#23](https://github.com/AllanChain/sane-break/issues/23)):
```bash
guix shell --container make cmake gcc-toolchain pkg-config vulkan-headers \
  qtbase@6 qtmultimedia \
  libxkbcommon libxscrnsaver libx11 \  # X support
  layer-shell-qt qtwayland@6 wayland   # Wayland support
```

## FAQ

### How to exit?

Right-click the menu icon and hover over the "Postpone" menu. The "Quit" menu is there. This design is intended to make users think twice about whether they want to postpone breaks or really quit the software.

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

## Contributing

### Translations

<a href="https://hosted.weblate.org/engage/sane-break/">
<img src="https://hosted.weblate.org/widget/sane-break/sane-break/open-graph.png" alt="Translation status" />
</a>
