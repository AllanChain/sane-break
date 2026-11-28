# CLI Control

Sane Break includes a command-line interface for querying break status and controlling a running Sane Break instance from keyboard shortcuts, scripts, and desktop status bars.

Running `sane-break` with no arguments (or with Qt GUI flags like `-platform wayland`) starts the desktop application. Running `sane-break` with a subcommand sends that command over a local socket (Unix domain socket on Linux and macOS, named pipe on Windows) to the running instance.

All subcommands return a single-line JSON object on `stdout` (exit code `0`) or a single-line JSON error object on `stderr` (exit code `1`). `--help` and `--version` output human-readable text.

## Subcommand Overview

| Command | Description |
| --- | --- |
| `sane-break status` | Return the current break and mode status as JSON. |
| `sane-break break-now` | Start the next scheduled break immediately. |
| `sane-break big-break-now` | Promote the next break to a big break (if enabled) and start it immediately. |
| `sane-break pause` | Pause breaks via the `external-control` pause reason. |
| `sane-break resume` | Clear the `external-control` pause reason. |
| `sane-break enable-break` | Clear **all** active pause reasons and resume the schedule. |
| `sane-break postpone <time>` | Postpone the upcoming break by `<time>`. |
| `sane-break meeting start --for <duration> [--reason <text>]` | Enter Meeting Mode for `<duration>`. |
| `sane-break meeting extend <time>` | Extend an active meeting by `<time>`. |
| `sane-break meeting end [--break-now \| --break-later <duration>]` | End Meeting Mode and either start a break now or schedule the next break. |
| `sane-break focus start --for <duration> [--reason <text>]` | Enter Focus Mode for `<duration>`. |
| `sane-break focus end` | End Focus Mode and start the closing break. |

Every mutating subcommand returns the updated [`status` JSON snapshot](#status-json-schema) on success.

## Duration Syntax

Commands that accept a `<duration>` or `<time>` argument support positive integers with an optional unit suffix.

- `s` specifies seconds (for example, `600s`, `2700s`).
- `m` specifies minutes (for example, `10m`, `45m`).
- `h` specifies hours (for example, `1h`, `2h`).
- Bare numbers without a suffix are interpreted as **minutes** (for example, `10` means `10m`).

## Querying Status

```bash
sane-break status
```

### Status JSON Schema

```json
{
  "bigBreakEnabled": true,
  "focus": {
    "active": false,
    "cyclesRemaining": 0,
    "totalCycles": 0
  },
  "isBreaking": false,
  "isPostponing": false,
  "meeting": {
    "active": false,
    "remainingSeconds": 0,
    "totalSeconds": 0
  },
  "mode": "normal",
  "nextBigBreakSeconds": 3142,
  "nextBreakSeconds": 742,
  "nextBreakType": "small",
  "pauseReasons": []
}
```

#### Field Reference

- `mode` is one of `"normal"`, `"focus"`, `"paused"`, `"meeting"`, or `"break"`.
- `isBreaking` is `true` while the break prompt or full-screen break window is active.
- `nextBreakType` is `"small"` or `"big"`.
- `nextBreakSeconds` reports remaining seconds until the next break starts.
- `bigBreakEnabled` reports whether big breaks are enabled in the current schedule (normal or focus).
- `nextBigBreakSeconds` reports remaining seconds until the next big break (`0` when `bigBreakEnabled` is `false`).
- `pauseReasons` lists active pause reason IDs in schema order.
  - `"idle"` when user input has been idle past the idle pause threshold
  - `"on-battery"` when running on battery power with battery pause enabled
  - `"app-open"` when a monitored program is running
  - `"sleep"` while the system is suspended
  - `"unknown-monitor"` when an unrecognized external display is connected
  - `"external-control"` when paused via `sane-break pause`
  - `"screen-lock"` while the desktop session is locked
- `isPostponing` is `true` if the upcoming break has been postponed.
- `meeting` contains `active` (boolean), `remainingSeconds` (integer), and `totalSeconds` (integer).
- `focus` contains `active` (boolean), `cyclesRemaining` (integer), and `totalCycles` (integer).

## Triggering Breaks

`break-now` starts the next scheduled break right away.

```bash
sane-break break-now
```

`big-break-now` promotes the upcoming break to a big break (when big breaks are enabled in the current schedule) and starts it immediately.

```bash
sane-break big-break-now
```

## Pausing and Resuming

Sane Break separates CLI pause control from the system tray's **Enable Break** override so scripts can pause and resume without clobbering other pause conditions.

- `sane-break pause` adds the `"external-control"` pause reason.
- `sane-break resume` removes only the `"external-control"` pause reason. If another reason (such as `"on-battery"` or `"app-open"`) is still active, Sane Break stays paused until that reason also clears.
- `sane-break enable-break` clears **all** active pause reasons at once, matching the **Enable Break** action in the tray menu.

```bash
# Pause from a script
sane-break pause

# Resume external control pause
sane-break resume

# Force resume regardless of battery, monitored apps, or external control
sane-break enable-break
```

## Postponing

`postpone` delays the next break by the given duration.

```bash
sane-break postpone 10m
```

Postponing via CLI follows the same state rules as the GUI. A break can only be postponed once (`already-postponing`), and postponing is rejected during Focus Mode (`postpone-during-focus`) or Meeting Mode (`postpone-during-meeting`). `sane-break postpone` does not enforce the GUI slider's maximum postpone percentage (`postpone/max-minute-ratio`), allowing custom scripted durations while still applying the proportional next-session shrink and break-extension rules.

## Meeting Mode

`meeting start` enters Meeting Mode for a specified duration with an optional reason.

```bash
sane-break meeting start --for 45m --reason "Weekly team sync"
```

`meeting extend` adds time to an active meeting.

```bash
sane-break meeting extend 15m
```

`meeting end` exits Meeting Mode, promotes the next break to a Big Break (when big breaks are enabled), and either resets the work timer, starts a break immediately, or schedules the next break after a custom delay.

```bash
# End meeting and reset the break timer to a full normal work interval
sane-break meeting end

# End meeting and start a recovery break immediately
sane-break meeting end --break-now

# End meeting and schedule the next break in 10 minutes
sane-break meeting end --break-later 10m
```

## Focus Mode

`focus start` enters Focus Mode for a target duration with an optional reason. Sane Break converts the `--for` duration into focus cycles by dividing by your configured `focus/small-every` interval (rounding up) and starts the entry break immediately.

```bash
sane-break focus start --for 50m --reason "Drafting architecture doc"
```

`focus end` ends Focus Mode early, triggering the closing break before returning to the normal schedule.

```bash
sane-break focus end
```

## Error Responses

When a command is rejected by a running Sane Break instance, it exits with status `1` and writes a JSON object to `stderr` with `"error"` and `"message"` fields. If no running Sane Break instance can be reached over the local socket, `sane-break` exits with status `1` and writes a plain-text error message (`No running Sane Break instance is available.`) to `stderr`.

```json
{"error":"meeting-already-active","message":"Meeting mode is already active."}
```

Error codes include the following values.

- `cli-parse-error` indicates invalid flags or arguments.
- `expected-command` indicates that no subcommand was provided.
- `invalid-duration` indicates that a duration string could not be parsed (and includes an `"input"` field).
- `meeting-already-active` and `meeting-not-active` report invalid meeting state transitions.
- `focus-already-active`, `focus-not-active`, `focus-during-meeting`, and `focus-end-during-break` report invalid focus state transitions.
- `already-postponing`, `postpone-during-focus`, and `postpone-during-meeting` report when postponing is unavailable.

## Status Bar Integration Example (Waybar)

Because `sane-break status` outputs compact JSON, status bars such as Waybar can format it with `jq`.

```json
"custom/sane-break": {
  "exec": "sane-break status | jq -cr '{text: (if .mode == \"paused\" then \"⏸ Paused\" elif .mode == \"meeting\" then \"📅 \\(.meeting.remainingSeconds / 60 | ceil)m\" elif .mode == \"focus\" then \"🎯 \\(.nextBreakSeconds / 60 | ceil)m\" else \"☕ \\(.nextBreakSeconds / 60 | ceil)m\" end), tooltip: \"Mode: \\(.mode)\\nNext break: \\(.nextBreakType) in \\(.nextBreakSeconds)s\", class: .mode}'",
  "return-type": "json",
  "interval": 5,
  "on-click": "sane-break break-now",
  "on-click-right": "sane-break pause",
  "on-click-middle": "sane-break resume"
}
```
