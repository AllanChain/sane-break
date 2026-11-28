# Hook Scripts

Sane Break can execute a custom command or script whenever a break or pause lifecycle event occurs, and even let your script dynamically override the message, colors, background image, or visible widgets of each break window right before it appears.

The script path is configured under **Preferences -> General -> Lifecycle hook script** (stored as `hook-script` in `SaneBreak.ini`).

::: info Platform Availability
Lifecycle hook scripts are available in native builds on Linux, macOS, and Windows. They are disabled in sandboxed builds (Linux Flatpak and the Mac App Store / Microsoft Store packages).
:::

## How Hook Scripts Are Invoked

When an event fires, Sane Break splits the configured **Lifecycle hook script** command string into an executable and any fixed arguments, appends the **event name** as the final positional argument, and sets `SANE_BREAK_*` environment variables.

For example, when the hook script is set to `/home/alice/.config/SaneBreak/hook.sh`, Sane Break invokes it with the event name as the first argument.

```bash
/home/alice/.config/SaneBreak/hook.sh <event-name>
```

Standard error (`stderr`) from the script is forwarded to Sane Break's console output for debugging.

## Lifecycle Events and Environment Variables

Every hook invocation receives `SANE_BREAK_EVENT` set to the event name, plus event-specific environment variables.

| Event Name | Execution Mode | When It Fires | Extra Environment Variables |
| --- | --- | --- | --- |
| `break-customize` | Synchronous (1s timeout, JSON `stdin` -> `stdout`) | Right before a break window is created. | `SANE_BREAK_TYPE`, `SANE_BREAK_DURATION` |
| `break-start` | Asynchronous | When a break starts (at the same time the start bell plays). | `SANE_BREAK_TYPE`, `SANE_BREAK_DURATION` |
| `break-prompt` | Asynchronous | Each time the break enters the Phase 1 flashing prompt. | `SANE_BREAK_TYPE`, `SANE_BREAK_DURATION` |
| `break-fullscreen` | Asynchronous | Each time the break expands to the Phase 2 full-screen overlay. | `SANE_BREAK_TYPE`, `SANE_BREAK_DURATION` |
| `break-end` | Asynchronous | When the break exits (either completed or aborted by a pause/postpone). | `SANE_BREAK_TYPE`, `SANE_BREAK_DURATION`, `SANE_BREAK_COMPLETED` |
| `pause-start` | Asynchronous | When Sane Break enters the paused state. | `SANE_BREAK_PAUSE_REASONS` |
| `pause-end` | Asynchronous | When Sane Break leaves the paused state and resumes scheduling. | `SANE_BREAK_PAUSE_REASONS` |

### Environment Variable Reference

- `SANE_BREAK_EVENT` holds the event name (`break-customize`, `break-start`, `break-prompt`, `break-fullscreen`, `break-end`, `pause-start`, or `pause-end`).
- `SANE_BREAK_TYPE` holds `"small"` or `"big"`.
- `SANE_BREAK_DURATION` holds the total scheduled duration of the break in seconds (including any postpone extension), as an integer string (for example, `"20"` or `"60"`).
- `SANE_BREAK_COMPLETED` is set on `break-end` only. It is `"1"` if the break countdown finished normally, or `"0"` if the break was exited early (for example, postponed or interrupted by an environmental pause).
- `SANE_BREAK_PAUSE_REASONS` is set on `pause-start` and `pause-end` only. It holds a comma-separated list of active pause reason IDs (`idle`, `on-battery`, `app-open`, `sleep`, `unknown-monitor`, `external-control`, and `screen-lock`, as described in the [CLI status field reference](/cli#field-reference)).

## Dynamic Break Customization (`break-customize`)

Unlike the other events (which run asynchronously in the background with `stdout` discarded), `break-customize` runs **synchronously** before the break window is created.

1. Sane Break writes the current break window configuration as a single-line JSON object to the script's `stdin` and closes `stdin`.
2. The script has up to **1000 ms (1 second)** to exit with code `0` and print a JSON object of overrides to `stdout` (up to 64 KB).
3. If the script outputs nothing, exits with a non-zero code, or only handles other events, Sane Break uses the configured preferences unchanged.

### Input and Output JSON Schema

Sane Break sends the following JSON structure on `stdin` during `break-customize`.

```json
{
  "message": {
    "prompt": "Time for a small break",
    "fullScreen": "Roll your shoulders and look away from the screen"
  },
  "show": {
    "buttons": true,
    "clock": false,
    "countdown": true,
    "endTime": false,
    "progressBar": true
  },
  "theme": {
    "background": "#ff2e3440",
    "backgroundImage": "",
    "countDownColor": "#ffeceff4",
    "flash": "#64ebcb8b",
    "messageColor": "#ffeceff4"
  }
}
```

The script can return a partial JSON object on `stdout` containing only the fields to override for that specific break.

- `message.prompt` (string) overrides the text shown in the Phase 1 flashing banner.
- `message.fullScreen` (string) overrides the text shown in the Phase 2 full-screen overlay.
- `theme.background`, `theme.flash`, `theme.messageColor`, and `theme.countDownColor` (strings) accept valid Qt color strings such as `#RRGGBB` or `#AARRGGBB`.
- `theme.backgroundImage` (string) accepts an absolute path or `file://` URL to an existing image file, or `""` to clear the background image for that break.
- `show.progressBar`, `show.countdown`, `show.clock`, `show.endTime`, and `show.buttons` (booleans) override widget visibility for that break.

## Example Hook Script

The following bash script demonstrates both dynamic break customization (picking a random wallpaper and message on `break-customize`) and desktop notification pausing on `break-fullscreen` and `break-end`.

```bash
#!/usr/bin/env bash
set -euo pipefail

event="${1:-${SANE_BREAK_EVENT:-}}"

case "$event" in
  break-customize)
    # Pick a random wallpaper from ~/Pictures/BreakWallpapers
    wallpaper=$(find "$HOME/Pictures/BreakWallpapers" -type f \( -name '*.jpg' -o -name '*.png' \) 2>/dev/null | shuf -n 1 || true)
    jq -n \
      --arg img "$wallpaper" \
      --arg msg "Stretch your eyes ($SANE_BREAK_TYPE break for ${SANE_BREAK_DURATION}s)" \
      '{
        message: { fullScreen: $msg },
        theme: (if $img != "" then { backgroundImage: $img } else {} end)
      }'
    ;;

  break-fullscreen)
    # Enable Do Not Disturb when the full-screen break begins (e.g., dunst)
    dunstctl set-paused true 2>/dev/null || true
    ;;

  break-end)
    # Restore notifications when the break ends
    dunstctl set-paused false 2>/dev/null || true
    ;;
esac
```

Make the script executable so Sane Break can run it.

```bash
chmod +x ~/.config/SaneBreak/hook.sh
```
