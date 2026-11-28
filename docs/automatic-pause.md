# Automatic Pause and Idle Reset

Sane Break monitors your activity and environment so it does not interrupt you while you are away from your desk, presenting on an external screen, or running specific applications. Pause behavior is configured under **Preferences -> Pause**.

## When Breaks Pause Automatically

Sane Break pauses the break schedule automatically under any of the following conditions and resumes once all active pause conditions clear.

- **Idle** pauses the countdown when there is no keyboard or mouse activity for the **Pause if idle for** duration (default **3 minutes**).
- **Screen Lock and System Sleep** pause breaks whenever your desktop session is locked or your computer goes to sleep.
- **Battery Power** pauses breaks while your laptop is unplugged when **Pause on battery** is enabled.
- **Monitored Programs** pause breaks whenever any application listed in **Pause breaks when any of the following program is running** is open.
- **Unknown External Monitors** pause breaks whenever an unrecognized display or projector is connected while **Pause when unknown monitor is connected** is enabled.

If an environmental pause condition triggers during an active break (such as unplugging from power with **Pause on battery** enabled, plugging in an unknown projector, or putting the computer to sleep), Sane Break closes the break window immediately and enters the paused state. Stepping away or locking the screen during a break does not cancel the break countdown.

Selecting **Enable Break** in the system tray menu overrides all current pause conditions and resumes the break schedule immediately.

## Video Playback and Presentations

Video players, presentation apps, and some games prevent the display from turning off while they run.

- When **When an app keeps the screen awake, count it as activity** is enabled (the default), Sane Break treats an active screen-awake signal as computer use. Breaks continue on schedule while you watch a video or view slides without touching the keyboard or mouse.
- When unchecked, only physical keyboard and mouse input counts as activity, so Sane Break pauses for idle during long videos.

Once a break actually starts, Sane Break always checks physical keyboard and mouse input so a background video cannot keep the flashing prompt from expanding to full screen.

::: info Platform Support on Linux
Screen-awake detection works across Windows, macOS, and modern Wayland desktops. On X11, detection depends on how individual applications inhibit the screensaver, so some media players may still be seen as either idle or active input regardless of this setting.
:::

## Pausing When Specific Programs Run

**Pause breaks when any of the following program is running** accepts one program name per line. Sane Break checks running process names every 5 seconds using a case-insensitive match (for example, `zoom`, `obs`, or `steam`).

This option is unavailable in the Linux Flatpak package because Flatpak apps cannot inspect host processes.

## Pausing on Unknown External Monitors

Enabling **Pause when unknown monitor is connected** prevents break pop-ups when you connect your laptop to conference room TVs or classroom projectors.

1. Connect the monitors you normally use at your desk.
2. Open **Preferences -> Pause** and click **Mark currently connected monitors as known**.
3. Whenever any connected display is not in your known list, Sane Break pauses automatically and resumes as soon as the unrecognized screen is disconnected.

## How Pause Duration Resets the Break Schedule

When all pause conditions clear and Sane Break resumes scheduling, it adjusts your countdown based on how long you were away (**Preferences -> Pause -> Reset Break Schedule After Pause**).

- **Brief pause (shorter than Short pause threshold)** continues the countdown from where it left off. If the pause lasted longer than the time remaining until the break would have finished, Sane Break refills the timer to a full work interval so a break does not trigger the moment you return.
- **Medium pause (longer than Short pause threshold, default 2 minutes)** refills the countdown to a full work interval while keeping your current progress toward the next Big Break.
- **Long pause (longer than Long pause threshold, default 5 minutes)** refills the countdown to a full work interval and resets the entire break cycle back to the first Small Break.
