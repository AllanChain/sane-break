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

Sane break is a polite and sane break reminder preventing mindless skips.

> Whether it's the health of your wrist or your eyes that concerns you, you need to take short breaks and it's all too easy to forget when you are busy.
>
> https://userbase.kde.org/RSIBreak

## 🤔 Why another break reminder?

So many break reminders just say that you should take a break at regular intervals.
However, You may be in the middle of writing down your idea, or you may be halfway through a line of code, and you don't want to be interrupted.
Thus you mindlessly click the "Postpone" or "Skip" button and continue working on the task at hand.
This way, the break reminder does no have any effect.
Even though a notification before break can be enabled, you just ignore those notifications for the most of the time.

I love the idea of [RSIBreak](https://userbase.kde.org/RSIBreak), which shows a small blinking window reminding you to have a break,
giving me some time to finish the current line and then take a break.
But RSIBreak only works for Linux with X11, not Wayland, nor Windows and macOS.
Besides, there are some other small inconveniences that I want to fix.
Therefore, I created Sane Break to poilitely remind myself to have a break and avoid mindless postponing or skipping.

## 🔮 Features

- No mindless skips, just a polite reminder
  - Just a small flashing window reminding you to have a break, and you can choose to finish the current task.
  - When you are away from keyboard, it will grow full screen and you are encouraged to look away.
  - If you continue working for too long, it will force fullscreen until break ends.
- Flexible: Automatic pause when idle, and postpone as you want.
- Small: No fancy stuff, just a small app.
- Cross-platform
  - Linux
    - X11 (Almost all)
    - Wayland (Compositors supporting [ext-idle-notify-v1](https://wayland.app/protocols/ext-idle-notify-v1))
  - macOS (Intel and ARM chips)
  - Windows
- Easy to develope: Written in C++, it is easy to integrate with system API.

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
