---
layout: home
markdownStyles: false

hero:
  name: Sane Break
  text: Gentle breaks without mindless skipping
  tagline: A gentle, cross-platform break reminder that helps you take meaningful rests without disrupting your workflow.
  image:
    src: /icon.svg
    alt: Sane Break
  actions:
    - theme: brand
      text: Install Sane Break
      link: /installation
    - theme: alt
      text: User Guide
      link: /breaks-and-schedule
---

<script setup lang="ts">
import { ref } from 'vue'
import { withBase } from 'vitepress'

type DialogTab = 'postpone' | 'focus' | 'meeting'
const activeDialogTab = ref<DialogTab>('postpone')

const dialogScreenshots: Record<DialogTab, { src: string; alt: string }> = {
  postpone: {
    src: '/screenshots/postpone-window.png',
    alt: 'Postpone dialog with proportional time trade-offs'
  },
  focus: {
    src: '/screenshots/focus-window.png',
    alt: 'Focus Mode dialog with custom schedule and reason'
  },
  meeting: {
    src: '/screenshots/meeting-window.png',
    alt: 'Meeting Mode dialog suppressing breaks until a scheduled end time'
  }
}
</script>

<section class="sb-home-section">
  <div class="sb-container">
    <HomeBreakDemo />
    <!-- Side-by-Side Comparison -->
    <div class="sb-comparison">
      <div class="sb-compare-card">
        <div class="sb-compare-header">
          <span class="sb-compare-tag">Traditional Break Reminders</span>
          <h3>Abrupt interruption breeds reflexive skipping</h3>
        </div>
        <ol class="sb-compare-list">
          <li>
            <strong>Blocks mid-sentence</strong>
            <span>
              A pop-up steals focus while you are in the middle of typing a thought or
              debugging a line of code.
            </span>
          </li>
          <li>
            <strong>One-click Skip button</strong>
            <span>
              A prominent Skip or Postpone button sits right on the reminder window for
              instant dismissal.
            </span>
          </li>
          <li>
            <strong>Automatic muscle memory</strong>
            <span>
              Dismissing the popup becomes an unconscious reflex, and hours pass with
              zero breaks taken.
            </span>
          </li>
        </ol>
      </div>
      <div class="sb-compare-card sb-compare-sane">
        <div class="sb-compare-header">
          <span class="sb-compare-tag sb-tag-sane">Sane Break</span>
          <h3>Respects your train of thought so you actually rest</h3>
        </div>
        <ol class="sb-compare-list">
          <li>
            <strong>Non-blocking top prompt</strong>
            <span>
              A click-through banner flashes gently at the top of the screen while you
              finish your current sentence.
            </span>
          </li>
          <li>
            <strong>Expands on a natural pause</strong>
            <span>
              Stopping input for 2 seconds expands the break to full screen, and
              resuming input during the grace period shrinks it right back.
            </span>
          </li>
          <li>
            <strong>Intentional friction</strong>
            <span>
              No skip button appears on the prompt. Postponing lives in the tray menu
              with proportional time trade-offs.
            </span>
          </li>
        </ol>
      </div>
    </div>
    <!-- Alternating Visual Feature Rows -->
    <div class="sb-feature-rows">
      <!-- Row 1: Focus, Meetings, and Mindful Postponing -->
      <div class="sb-feature-row">
        <div class="sb-feature-copy">
          <span class="sb-feature-eyebrow">Structured Flexibility</span>
          <h2 class="sb-feature-heading">
            Adapt breaks around deep work, live calls, and urgent deadlines
          </h2>
          <ul class="sb-feature-points">
            <li>
              <strong>Mindful Postponing</strong> allows a single delay per break and
              balances it by extending the upcoming rest and shortening the next work
              session.
            </li>
            <li>
              <strong>Focus Mode</strong> switches to a separate Pomodoro-style break
              schedule after an immediate entry break so you begin focused blocks rested.
            </li>
            <li>
              <strong>Meeting Mode</strong> suppresses all break prompts during calls and
              presentations, then checks in during the final minute.
            </li>
          </ul>
          <a :href="withBase('/modes-and-postponing')" class="sb-feature-link">
            Explore Focus, Meetings, and Postponing →
          </a>
        </div>
        <div class="sb-feature-visual">
          <div class="sb-visual-tabs" role="tablist" aria-label="Mode dialogs">
            <button
              type="button"
              class="sb-visual-tab"
              :class="{ active: activeDialogTab === 'postpone' }"
              @click="activeDialogTab = 'postpone'"
            >
              Postpone
            </button>
            <button
              type="button"
              class="sb-visual-tab"
              :class="{ active: activeDialogTab === 'focus' }"
              @click="activeDialogTab = 'focus'"
            >
              Focus Mode
            </button>
            <button
              type="button"
              class="sb-visual-tab"
              :class="{ active: activeDialogTab === 'meeting' }"
              @click="activeDialogTab = 'meeting'"
            >
              Meeting Mode
            </button>
          </div>
          <div class="sb-screenshot-stage">
            <img
              :src="withBase(dialogScreenshots[activeDialogTab].src)"
              :alt="dialogScreenshots[activeDialogTab].alt"
              class="sb-dialog-img"
            />
          </div>
        </div>
      </div>
      <!-- Row 2: Context-Aware Automatic Pause -->
      <div class="sb-feature-row is-reversed">
        <div class="sb-feature-copy">
          <span class="sb-feature-eyebrow">Context-Aware Schedule</span>
          <h2 class="sb-feature-heading">
            Pauses automatically when you step away or plug into a projector
          </h2>
          <ul class="sb-feature-points">
            <li>
              <strong>Idle, Screen Lock, and Sleep</strong> pause reminders while you are
              away and automatically reset your break cycle after longer absences.
            </li>
            <li>
              <strong>Video and Presentation Detection</strong> recognizes when a media
              player or slide deck keeps the display awake so breaks match your preference.
            </li>
            <li>
              <strong>Monitored Apps, Battery, and External Monitors</strong> pause breaks
              whenever listed programs run, on battery power, or when an unrecognized
              projector is connected.
            </li>
          </ul>
          <a :href="withBase('/automatic-pause')" class="sb-feature-link">
            Configure Automatic Pause →
          </a>
        </div>
        <div class="sb-feature-visual">
          <img
            :src="withBase('/screenshots/pref-pause.png')"
            alt="Automatic pause preferences window"
            class="sb-window-img"
          />
        </div>
      </div>
      <!-- Row 3: Weekly Timeline & Visual Customization -->
      <div class="sb-feature-row">
        <div class="sb-feature-copy">
          <span class="sb-feature-eyebrow">Local-Only Insights &amp; Themes</span>
          <h2 class="sb-feature-heading">
            Inspect your daily rest habits and customize every detail
          </h2>
          <ul class="sb-feature-points">
            <li>
              <strong>Weekly Activity Timeline</strong> plots normal work, breaks, focus
              sessions, meetings, pauses, and postpone markers across your active hours.
            </li>
            <li>
              <strong>Prompt Response Metrics</strong> track completed breaks, average
              prompt flash time before pausing, and force-break exits in a local SQLite
              database with zero network calls.
            </li>
            <li>
              <strong>Independent Small &amp; Big Themes</strong> customize colors,
              background images, visible widgets, and start or end sound bells.
            </li>
          </ul>
          <a :href="withBase('/customization')" class="sb-feature-link">
            View Customization and Statistics →
          </a>
        </div>
        <div class="sb-feature-visual">
          <img
            :src="withBase('/screenshots/stats-window.png')"
            alt="Weekly activity and break statistics window"
            class="sb-window-img"
          />
        </div>
      </div>
      <!-- Row 4: CLI Control & Lifecycle Hook Scripts -->
      <div class="sb-feature-row is-reversed">
        <div class="sb-feature-copy">
          <span class="sb-feature-eyebrow">Scriptable Automation</span>
          <h2 class="sb-feature-heading">
            Control breaks from keyboard shortcuts, status bars, and hook scripts
          </h2>
          <ul class="sb-feature-points">
            <li>
              <strong>JSON Status &amp; Subcommands</strong> query countdowns for Waybar
              or Polybar and trigger breaks, meetings, or focus sessions from hotkeys.
            </li>
            <li>
              <strong>Isolated Script Pause</strong> lets scripts run
              <code>sane-break pause</code> and <code>sane-break resume</code> without
              clearing active battery or app pauses.
            </li>
            <li>
              <strong>Lifecycle Hook Scripts</strong> run custom commands on break and
              pause events or dynamically override break wallpapers and messages via JSON.
            </li>
          </ul>
          <div class="sb-feature-links">
            <a :href="withBase('/cli')" class="sb-feature-link">CLI Reference →</a>
            <a :href="withBase('/hooks')" class="sb-feature-link">Hook Scripts →</a>
          </div>
        </div>
        <div class="sb-feature-visual is-bare">
          <div class="sb-terminal-card">
            <div class="sb-window-titlebar sb-terminal-titlebar">
              <div class="sb-window-dots">
                <span></span>
                <span></span>
                <span></span>
              </div>
              <div class="sb-window-title">sane-break — bash</div>
              <div class="sb-window-spacer"></div>
            </div>
            <pre class="sb-terminal-pre" v-pre><code><span class="sb-t-comment"># Query live status for Waybar / status bars</span>
<span class="sb-t-prompt">$</span> sane-break status
<span class="sb-t-json">{"mode":"normal","nextBreakType":"small","nextBreakSeconds":742,"isPostponing":false}</span>
&#10;<span class="sb-t-comment"># Start Meeting or Focus mode from a keyboard shortcut</span>
<span class="sb-t-prompt">$</span> sane-break meeting start --for 45m --reason "Weekly team sync"
&#10;<span class="sb-t-comment"># Dynamic wallpaper &amp; message override in hook.sh (break-customize)</span>
<span class="sb-t-json">{"message":{"fullScreen":"Look 20 feet away"},"theme":{"backgroundImage":"~/Pictures/forest.jpg"}}</span></code></pre>
          </div>
        </div>
      </div>
    </div>
  </div>
</section>
