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

Inspired by the design of RSIBreak, Sane Break employs a two-phase system:

- Phase 1: A small, unobtrusive reminder that you can acknowledge or ignore.
- Phase 2: If you continue working for too long, the reminder grows fullscreen, ensuring you take a break.

Key improvemnets compared with RSIBreak:

- Automatic transition to the fullscreen phase when you stop working.
- No skip or postpone buttons to avoid mindless dismissals.
- Cross-platform support for Linux (X11 and Wayland), macOS (Intel and ARM), and Windows

Sane Break aims to strike a balance between respecting your workflow and ensuring you take the breaks you need.

For more details about the motivation behind Sane Break, you can refer to [No break reminder works (for me)](https://allanchain.github.io/blog/post/sane-break-intro/).

## 🎬 Demo video

https://github.com/user-attachments/assets/b6a15675-8122-4848-ba4e-44a687179394

## Compiling from source

For Linux users, you are strongly encouraged to install Sane Break by comping from source.
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

### What if I really want to skip break?

Generally, you can quit the app via tray icon right away after the break starts.
However, if you really want to quit when Sane Break is in fullscreen, you can kill the app via system command line. For example:
- KDE: Enable KRunner with <kbd>Alt</kbd> + <kbd>Space</kbd> and run `killall sane-break`
- macOS: Enable Spotlight with <kbd>Cmd</kbd> + <kbd>Space</kbd> and run `killall sane-break`
- Windows: Press <kbd>Win</kbd> + <kbd>R</kbd> and run `taskkill /IM sane-break /F`

## To do list

- Create AUR package
- Auto start settings
- Stop breaks while in screen sharing?
