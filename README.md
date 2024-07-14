<p align="center">
  <img src="./resources/images/icon.svg" width="150" height="150">
</p>
<h1 align="center">Sane Break</h1>

Sane break is a polite break reminder to keep you sane.

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

- Polite: First a blinking small window, you can finish the current task, and it grows fullscreen when you are AFK, forcing you to look away.
- Small: No fancy stuff, just a small app.
- Cross-platform: Supporting KDE Wayland, macOS, and Windows.

## 🎬 Demo video

https://github.com/user-attachments/assets/b6a15675-8122-4848-ba4e-44a687179394

## Compiling from source

For Linux users, currently the only way to install Sane Break is to compile it from source.
Make sure you have `layer-shell-qt` and `kidletime` for Qt6 installed.
```bash
git clone https://github.com/AllanChain/sane-break
cd sane-break
mkdir build
cd build
cmake ..
make
```

## To do list

- Create AUR package
- Support non-KDE and X11 environments
- Auto postpone breaks when system idle for a long time
- Stop breaks while in screen sharing?
