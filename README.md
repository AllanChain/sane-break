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

All too often, I'm deeply focused on a task when the break reminder appears. Almost instinctively, I click "skip" or "postpone" without a second thought. Before I know it, hours have passed without a single break.

Many existing solutions demand immediate action, often leading to mindless skips or postpones. This defeats the purpose of taking regular, meaningful breaks. Instead, Sane Break provides a gentle initial nudge, allowing you to wrap up your current task before enforcing a break if necessary.

Sane Break employs a two-phase system:

- Phase 1: A small, unobtrusive reminder that you can acknowledge or ignore.
- Phase 2: The reminder grows to full screen, ensuring you take a break.

Transition to phase 2 happens when you stop working on your computer, or ignore the reminder and keep working for too long.

This design is an enhanced version of the [RSIBreak](https://apps.kde.org/rsibreak/) design, but Sane Break has some other improvements:

- No big skip or postpone buttons to avoid mindless dismissals.
- [Cross-platform support](#platform-support)

Sane Break aims to strike a balance between respecting your workflow and ensuring you take the breaks you need.

For more details about the motivation behind Sane Break, you can refer to [No break reminder works (for me)](https://allanchain.github.io/blog/post/sane-break-intro/).

## 🎬 Demo video

https://github.com/user-attachments/assets/00512521-73ce-4e76-a4e0-522492a6a7ed

## Platform support

### Linux

KDE 6 (on Arch-based distros) support is prioritized. Others may work too.

Since this program only supports Qt6 and uses [wlr layer shell](https://wayland.app/protocols/wlr-layer-shell-unstable-v1) and [idle notify](https://wayland.app/protocols/ext-idle-notify-v1) protocols, in theory it supports:
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

If you want to enable just X11 or Wayland support, or you are using other distros (which I didn't test), you are strongly encouraged to install Sane Break by comping from source.
It will automatically enable X11 or Wayland support based on libraries found.
For Wayland support, make sure you have `layer-shell-qt` for Qt6 installed.

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

### How to start Sane Break on startup?

- Linux: Whatever way you like
- macOS: Open "System Preferences" and search for "Login Items". Click "+" and add Sane Break to login items
- Windows: Use <kbd>Win</kbd> + <kbd>R</kbd> and type `shell:startup`. Put `sane-break.exe` or its shortcut there

### What if I really want to skip break?

Generally, you can quit the app via tray icon right away after the break starts.
However, if you really want to quit when Sane Break is in fullscreen, you can kill the app via system command line. For example:
- KDE: Enable KRunner with <kbd>Alt</kbd> + <kbd>Space</kbd> and run `killall sane-break`
- macOS: Enable Spotlight with <kbd>Cmd</kbd> + <kbd>Space</kbd> and run `killall sane-break`
- Windows: Press <kbd>Win</kbd> + <kbd>R</kbd> and run `taskkill /IM sane-break /F`

## To do list

- Auto start settings
- Stop breaks while in screen sharing?
