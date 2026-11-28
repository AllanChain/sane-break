# Installation

Sane Break runs on Windows, macOS, and Linux.

## Windows

Sane Break is available on the [Microsoft Store](https://apps.microsoft.com/detail/9n1c3x5dwhxx?referrer=appbadge&mode=direct) and as a standalone archive on the [GitHub releases page](https://github.com/AllanChain/sane-break/releases).

For the standalone archive, download `sane-break-windows.zip`, extract it, and run `sane-break.exe` from any folder. Windows 10 (x86_64) and newer are supported.

To run Sane Break in portable mode, create an empty `SaneBreak.ini` file in the same directory as `sane-break.exe`. Sane Break stores its configuration in that file instead of `%APPDATA%`.

## macOS

Install Sane Break with Homebrew.

```bash
brew install allanchain/tap/sane-break
```

Sane Break is also available on the [Mac App Store](https://apps.apple.com/us/app/sane-break/id6740942634) and as `.dmg` installers (`sane-break-macos-arm64.dmg` for Apple Silicon and `sane-break-macos-x86_64.dmg` for Intel) on the [GitHub releases page](https://github.com/AllanChain/sane-break/releases).

## Linux

Sane Break supports both Wayland and X11 desktop environments.

- **KDE Plasma (Wayland), Sway, Hyprland, Niri, River, and all X11 desktops** work out of the box.
- **GNOME (Wayland)** requires the companion [GNOME Shell extension](#gnome-wayland-setup) to keep the break overlay above normal windows and track idle time.

### Flatpak (Flathub)

Install the Flatpak package from Flathub.

```bash
flatpak install flathub io.github.AllanChain.sane-break
```

Because Flatpak runs inside a sandbox, process monitoring (`Pause breaks when any of the following program is running`) and lifecycle hook scripts are unavailable in the Flatpak build. A native distro package or source build supports those features.

### Arch Linux (AUR)

Install the package from the AUR.

```bash
yay -S sane-break
```

### Fedora and RHEL (Copr)

Enable the Copr repository and install with `dnf`.

```bash
sudo dnf copr enable allanchain/sane-break
sudo dnf install sane-break
```

### Ubuntu and Debian (.deb)

Download the `.deb` package from the [GitHub releases page](https://github.com/AllanChain/sane-break/releases).

- **Debian Trixie (and newer) or KDE Neon** use the Debian Trixie build, which includes Wayland layer-shell support.
- **Ubuntu 24.10 (and newer)** uses the Ubuntu Oracular build.

### AppImage

An AppImage binary (`sane-break.AppImage`) is available on the [GitHub releases page](https://github.com/AllanChain/sane-break/releases). The AppImage bundles its own libraries and may not inherit system widget themes.

### GNOME Wayland Setup

On GNOME Wayland, Sane Break uses a companion GNOME Shell extension to position the break window on top and detect idle time.

If you installed Sane Break from `.deb`, `.rpm`, or source, enable the bundled extension and restart your Wayland session.

```bash
gnome-extensions enable sane-break@allanchain.github.io
```

If you installed the Flatpak or AppImage, download `sane-break-gnome-extension.zip` from the [GitHub releases page](https://github.com/AllanChain/sane-break/releases), install it, and log out and back in.

```bash
gnome-extensions install sane-break-gnome-extension.zip
gnome-extensions enable sane-break@allanchain.github.io
```

### i3wm Configuration

On i3, add a window rule to `~/.config/i3/config` so the break window floats and stays visible across all workspaces.

```txt
for_window [instance="sane-break"] floating enable, sticky enable

# Optional: avoid moving the Sane Break window into the scratchpad
bindsym $mod+x [floating instance="^(?!sane-break).*$"] move scratchpad
```

### Autostart on Linux Desktops Without Background Portal

The in-app **Launch Sane Break at start up** toggle uses the standard XDG autostart directory for native packages and the D-Bus Background portal for Flatpak. On desktop environments that do not implement the background portal for Flatpak (such as COSMIC), create `~/.config/autostart/io.github.AllanChain.sane-break.desktop` manually.

```ini
[Desktop Entry]
Type=Application
Name=Sane Break
Exec=flatpak run --command=sane-break io.github.AllanChain.sane-break
X-Flatpak=io.github.AllanChain.sane-break
```

## Building from Source

Sane Break uses CMake and Qt 6 (Qt 6.4 or newer for X11, Qt 6.6 or newer for Wayland). Install the required dependencies for your distribution before building.

- **Arch Linux** requires `qt6-base qt6-multimedia cli11 libxss layer-shell-qt qt6-tools`.
- **Debian and Ubuntu** require `qt6-base-dev qt6-multimedia-dev libcli11-dev libxss-dev qt6-wayland-dev liblayershellqtinterface-dev qt6-l10n-tools`.
- **GNU Guix** provides a build environment with `guix shell`.
  ```bash
  guix shell --container make cmake gcc-toolchain pkg-config vulkan-headers \
    qtbase@6 qtmultimedia cli11 libxkbcommon libxscrnsaver libx11 \
    layer-shell-qt qtwayland@6 wayland
  ```

Clone the repository and build.

```bash
git clone https://github.com/AllanChain/sane-break
cd sane-break
cmake -S . -B build
cmake --build build --parallel
sudo cmake --install build
```

CMake automatically enables X11 idle detection when `libxss` is found, Wayland `wlr-layer-shell-unstable-v1` and `ext-idle-notify-v1` support when `LayerShellQt` is found, and external CLI control when `CLI11` is found.
