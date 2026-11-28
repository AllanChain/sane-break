<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted, nextTick } from 'vue'

type Mode = 'auto' | 'interactive'
type Phase = 'work' | 'prompt' | 'grace' | 'locked'

const TICK_MS = 50
const IDLE_TO_EXPAND_MS = 2000
const GRACE_LOCK_SEC = 10
const BREAK_TOTAL_SEC = 12
const PROMPT_FORCE_SEC = 10
const POST_BREAK_WORK_MS = 1800

const AUTO_LINE_1 = 'Finishing this sentence while the top prompt flashes...'
const AUTO_LINE_2 =
  '\n\nWait, one quick edit! If I keep typing nonstop until the prompt timer hits 0s, Force Break locks in...'

const mode = ref<Mode>('auto')
const phase = ref<Phase>('prompt')
const editorText = ref('')
const isInputActive = ref(false)
const idleMs = ref(0)
const isForcedLock = ref(false)

const breakRemainingSec = ref(BREAK_TOTAL_SEC)
const graceRemainingSec = ref(GRACE_LOCK_SEC)
const promptRemainingSec = ref(PROMPT_FORCE_SEC)

const editorRef = ref<HTMLTextAreaElement | null>(null)

let tickTimer: ReturnType<typeof setInterval> | null = null
let inputResetTimer: ReturnType<typeof setTimeout> | null = null
let workElapsedMs = 0

// Auto-typer state (drives the same break state machine as interactive mode)
let autoStage: 'line1' | 'waitGrace' | 'line2' | 'done' = 'line1'
let autoCharIdx = 0
let autoGraceElapsedMs = 0

const stepIndex = computed(() => {
  if (phase.value === 'prompt' || phase.value === 'work') return 1
  if (phase.value === 'grace') return 2
  return 3
})

const progressBarPercent = computed(() =>
  phase.value === 'prompt'
    ? 100
    : Math.max(0, Math.min(100, (breakRemainingSec.value / BREAK_TOTAL_SEC) * 100))
)

const hudPill = computed(() => {
  if (phase.value === 'work') {
    return {
      tone: 'active',
      state: 'Break completed',
      detail: 'Window closed · Next work session started'
    }
  }
  if (phase.value === 'prompt') {
    const forceSec = Math.ceil(promptRemainingSec.value)
    if (isInputActive.value || idleMs.value < 250) {
      return {
        tone: 'active',
        state: 'Typing',
        detail: `Force break in ${forceSec}s (or pause 2s)`
      }
    }
    return {
      tone: 'waiting',
      state: `Paused ${(idleMs.value / 1000).toFixed(1)}s / 2.0s`,
      detail: `Force break in ${forceSec}s`
    }
  }
  if (phase.value === 'grace') {
    return {
      tone: 'grace',
      state: `Grace period (${Math.ceil(graceRemainingSec.value)}s)`,
      detail: 'Any input shrinks it right back'
    }
  }
  return {
    tone: 'locked',
    state: isForcedLock.value ? 'Force break locked' : 'Break locked in',
    detail: isForcedLock.value
      ? 'Prompt timer expired · Time to rest'
      : 'Confirmed after 10s idle · Time to rest'
  }
})

function resetCycleState() {
  phase.value = 'prompt'
  isForcedLock.value = false
  idleMs.value = 0
  workElapsedMs = 0
  promptRemainingSec.value = PROMPT_FORCE_SEC
  breakRemainingSec.value = BREAK_TOTAL_SEC
  graceRemainingSec.value = GRACE_LOCK_SEC
}

function recordActivity() {
  isInputActive.value = true
  idleMs.value = 0

  if (inputResetTimer) clearTimeout(inputResetTimer)
  inputResetTimer = setTimeout(() => {
    isInputActive.value = false
  }, 350)

  if (phase.value === 'grace') {
    phase.value = 'prompt'
    breakRemainingSec.value = BREAK_TOTAL_SEC
    graceRemainingSec.value = GRACE_LOCK_SEC
  }
}

function stepAutoTyper() {
  if (autoStage === 'line1') {
    recordActivity()
    if (autoCharIdx < AUTO_LINE_1.length) {
      editorText.value += AUTO_LINE_1[autoCharIdx++]
    } else if (promptRemainingSec.value <= PROMPT_FORCE_SEC - 3.5) {
      autoStage = 'waitGrace'
      autoGraceElapsedMs = 0
    }
  } else if (autoStage === 'waitGrace') {
    if (phase.value === 'grace') {
      autoGraceElapsedMs += TICK_MS
      if (autoGraceElapsedMs >= 2400) {
        autoStage = 'line2'
        autoCharIdx = 0
        recordActivity()
      }
    }
  } else if (autoStage === 'line2') {
    if (phase.value === 'prompt') {
      recordActivity()
      if (autoCharIdx < AUTO_LINE_2.length) {
        editorText.value += AUTO_LINE_2[autoCharIdx++]
      }
    } else if (phase.value === 'locked') {
      autoStage = 'done'
    }
  }
}

function stepBreakState() {
  const dt = TICK_MS / 1000

  if (phase.value === 'work') {
    workElapsedMs += TICK_MS
    if (workElapsedMs >= POST_BREAK_WORK_MS) {
      if (mode.value === 'auto') {
        startAutoDemo()
      } else {
        resetCycleState()
      }
    }
    return
  }

  if (phase.value === 'prompt') {
    if (!isInputActive.value) {
      idleMs.value = Math.min(IDLE_TO_EXPAND_MS, idleMs.value + TICK_MS)
      if (idleMs.value >= IDLE_TO_EXPAND_MS) {
        isForcedLock.value = false
        phase.value = 'grace'
        graceRemainingSec.value = GRACE_LOCK_SEC
        breakRemainingSec.value = BREAK_TOTAL_SEC
        return
      }
    }
    promptRemainingSec.value = Math.max(0, promptRemainingSec.value - dt)
    if (promptRemainingSec.value <= 0) {
      isForcedLock.value = true
      phase.value = 'locked'
      breakRemainingSec.value = BREAK_TOTAL_SEC
    }
  } else if (phase.value === 'grace') {
    graceRemainingSec.value = Math.max(0, graceRemainingSec.value - dt)
    breakRemainingSec.value = Math.max(0, breakRemainingSec.value - dt)
    if (graceRemainingSec.value <= 0) {
      isForcedLock.value = false
      phase.value = 'locked'
    }
  } else if (phase.value === 'locked') {
    breakRemainingSec.value = Math.max(0, breakRemainingSec.value - dt)
    if (breakRemainingSec.value <= 0) {
      phase.value = 'work'
      workElapsedMs = 0
    }
  }
}

function startLoop() {
  if (tickTimer) clearInterval(tickTimer)
  tickTimer = setInterval(() => {
    if (mode.value === 'auto') stepAutoTyper()
    stepBreakState()
  }, TICK_MS)
}

function startAutoDemo() {
  mode.value = 'auto'
  editorText.value = ''
  autoStage = 'line1'
  autoCharIdx = 0
  autoGraceElapsedMs = 0
  resetCycleState()
  recordActivity()
  startLoop()
}

function startInteractiveMode(focusEditor = true) {
  mode.value = 'interactive'
  if (phase.value === 'locked' || phase.value === 'work') {
    resetCycleState()
  }
  if (!editorText.value) {
    editorText.value = 'Keep typing to test Force Break, or pause for 2 seconds...'
  }
  recordActivity()
  startLoop()
  if (focusEditor) {
    nextTick(() => editorRef.value?.focus())
  }
}

function handleUserActivity() {
  if (mode.value === 'auto') startInteractiveMode(false)
  recordActivity()
}

function jumpToStep(step: 1 | 2 | 3) {
  startInteractiveMode(step === 1)
  resetCycleState()
  if (step === 2) {
    phase.value = 'grace'
    isInputActive.value = false
  } else if (step === 3) {
    phase.value = 'locked'
    isForcedLock.value = true
    isInputActive.value = false
  }
}

function handleExitForceBreak() {
  startInteractiveMode(true)
  resetCycleState()
  recordActivity()
}

onMounted(startAutoDemo)
onUnmounted(() => {
  if (tickTimer) clearInterval(tickTimer)
  if (inputResetTimer) clearTimeout(inputResetTimer)
})
</script>

<template>
  <div class="sb-demo">
    <!-- Toolbar: Phase Stepper + Mode Switch -->
    <div class="sb-toolbar">
      <div class="sb-steps" role="tablist" aria-label="Break phases">
        <button
          type="button"
          class="sb-step"
          :class="{ active: stepIndex === 1 }"
          @click="jumpToStep(1)"
        >
          <span class="sb-step-num">1</span>
          <span>Flashing Prompt</span>
        </button>
        <span class="sb-step-arrow" aria-hidden="true">→</span>
        <button
          type="button"
          class="sb-step"
          :class="{ active: stepIndex === 2 }"
          @click="jumpToStep(2)"
        >
          <span class="sb-step-num">2</span>
          <span>Grace Period (10s)</span>
        </button>
        <span class="sb-step-arrow" aria-hidden="true">→</span>
        <button
          type="button"
          class="sb-step"
          :class="{ active: stepIndex === 3 }"
          @click="jumpToStep(3)"
        >
          <span class="sb-step-num">3</span>
          <span>Locked / Force Break</span>
        </button>
      </div>

      <div class="sb-mode-switch">
        <button
          type="button"
          class="sb-mode-btn"
          :class="{ active: mode === 'auto' }"
          @click="startAutoDemo()"
        >
          ▶ Auto Demo
        </button>
        <button
          type="button"
          class="sb-mode-btn"
          :class="{ active: mode === 'interactive' }"
          @click="startInteractiveMode(true)"
        >
          ⌨ Try It Live
        </button>
      </div>
    </div>

    <!-- Mini Desktop Viewport -->
    <div
      class="sb-desktop"
      @mousemove="mode === 'interactive' ? recordActivity() : undefined"
    >
      <div class="sb-note-window">
        <div class="sb-window-titlebar">
          <div class="sb-window-dots">
            <span></span>
            <span></span>
            <span></span>
          </div>
          <div class="sb-window-title">Notes</div>
          <div class="sb-window-spacer"></div>
        </div>

        <div class="sb-note-body">
          <textarea
            ref="editorRef"
            v-model="editorText"
            class="sb-textarea"
            spellcheck="false"
            aria-label="Interactive demo editor"
            placeholder="Keep typing to trigger Force Break (10s), or stop for 2s..."
            @input="handleUserActivity"
            @keydown="handleUserActivity"
            @focus="mode === 'auto' ? startInteractiveMode(false) : undefined"
          ></textarea>
        </div>
      </div>

      <!-- Sane Break Window (Matches src/app/break-window.ui) -->
      <Transition name="sb-break-fade">
        <div
          v-if="phase !== 'work'"
          class="sb-break-window"
          :class="phase === 'prompt' ? 'is-prompt' : 'is-fullscreen'"
        >
          <div class="sb-qprogressbar">
            <div
              class="sb-qprogressbar-chunk"
              :style="{ width: `${progressBarPercent}%` }"
            ></div>
          </div>

          <div class="sb-break-center">
            <div class="sb-break-label">Time for a small break</div>

            <template v-if="phase === 'grace' || phase === 'locked'">
              <div class="sb-countdown-label">{{ Math.ceil(breakRemainingSec) }}</div>

              <div class="sb-action-buttons" :class="{ visible: phase === 'locked' }">
                <div class="sb-btn-group">
                  <button
                    type="button"
                    class="sb-circle-btn"
                    title="Lock Screen"
                    @click.stop="startAutoDemo()"
                  >
                    <span class="sb-bi-icon" aria-hidden="true">&#xF47A;</span>
                  </button>
                  <span class="sb-btn-caption">Lock Screen</span>
                </div>

                <div class="sb-btn-group">
                  <button
                    type="button"
                    class="sb-circle-btn"
                    title="Pause Media"
                    @click.stop="startAutoDemo()"
                  >
                    <span class="sb-bi-icon" aria-hidden="true">&#xF4C3;</span>
                  </button>
                  <span class="sb-btn-caption">Pause Media</span>
                </div>

                <div class="sb-btn-group">
                  <button
                    type="button"
                    class="sb-circle-btn"
                    title="Exit Force Break"
                    @click.stop="handleExitForceBreak()"
                  >
                    <span class="sb-bi-icon sb-bi-exit" aria-hidden="true">&#xF117;</span>
                  </button>
                  <span class="sb-btn-caption">Exit Force Break</span>
                </div>
              </div>
            </template>
          </div>
        </div>
      </Transition>

      <!-- Fixed-Width Visual HUD Pill -->
      <div class="sb-hud-pill" :class="hudPill.tone">
        <span class="sb-hud-dot"></span>
        <strong class="sb-hud-state">{{ hudPill.state }}</strong>
        <span class="sb-hud-sep">·</span>
        <span class="sb-hud-detail">{{ hudPill.detail }}</span>
      </div>
    </div>
  </div>
</template>

<style scoped>
.sb-toolbar {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 14px;
}

.sb-steps {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 6px;
}

.sb-step {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 6px 12px;
  border-radius: 999px;
  border: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-2);
  font-size: 13px;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s ease;
}

.sb-step:hover {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-text-1);
}

.sb-step.active {
  border-color: var(--vp-c-brand-1);
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand-1);
  font-weight: 600;
}

.sb-step-num {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background: rgba(127, 127, 127, 0.15);
  font-size: 11px;
  font-weight: 700;
}

.sb-step.active .sb-step-num {
  background: var(--vp-c-brand-1);
  color: var(--vp-c-white);
}

.sb-step-arrow {
  color: var(--vp-c-text-3);
  font-size: 13px;
}

.sb-mode-switch {
  display: inline-flex;
  padding: 3px;
  border-radius: 10px;
  background: var(--vp-c-bg-soft);
  border: 1px solid var(--vp-c-divider);
}

.sb-mode-btn {
  padding: 6px 14px;
  border-radius: 7px;
  font-size: 13px;
  font-weight: 600;
  color: var(--vp-c-text-2);
  cursor: pointer;
  transition: all 0.2s ease;
}

.sb-mode-btn.active {
  background: var(--vp-c-brand-3);
  color: #ffffff;
  box-shadow: 0 2px 6px rgba(22, 163, 74, 0.25);
}

/* Mini Desktop Canvas */
.sb-desktop {
  position: relative;
  isolation: isolate;
  border-radius: 14px;
  overflow: hidden;
  border: 1px solid var(--vp-c-divider);
  background: linear-gradient(160deg, #dbeafe 0%, #e2e8f0 100%);
  box-shadow:
    0 20px 45px -12px rgba(15, 23, 42, 0.2),
    0 4px 16px -4px rgba(15, 23, 42, 0.1);
  height: 420px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}

.dark .sb-desktop {
  background: linear-gradient(160deg, #1e293b 0%, #0f172a 100%);
  box-shadow:
    0 20px 45px -12px rgba(0, 0, 0, 0.55),
    0 4px 16px -4px rgba(0, 0, 0, 0.35);
}

.sb-note-window {
  width: min(88%, 620px);
  height: 220px;
  margin-top: 36px;
  border-radius: 10px;
  overflow: hidden;
  background: #ffffff;
  border: 1px solid rgba(15, 23, 42, 0.12);
  box-shadow: 0 12px 32px rgba(15, 23, 42, 0.12);
  display: flex;
  flex-direction: column;
}

.dark .sb-note-window {
  background: #0f172a;
  border-color: rgba(255, 255, 255, 0.12);
  box-shadow: 0 16px 36px rgba(0, 0, 0, 0.45);
}

.sb-note-body {
  flex: 1;
  padding: 18px 22px;
  display: flex;
}

.sb-textarea {
  flex: 1;
  background: transparent;
  border: none;
  outline: none;
  resize: none;
  color: #1e293b;
  font-family: var(--vp-font-family-base);
  font-size: 18px;
  line-height: 1.55;
  font-weight: 500;
}

.dark .sb-textarea {
  color: #f1f5f9;
}

/* Sane Break Window */
.sb-break-window {
  position: absolute;
  top: 0;
  left: 50%;
  transform: translateX(-50%);
  background: #2e3440;
  color: #eceff4;
  transition:
    width 0.3s cubic-bezier(0.16, 1, 0.3, 1),
    height 0.3s cubic-bezier(0.16, 1, 0.3, 1),
    background-color 0.3s ease,
    opacity 0.25s ease;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  user-select: none;
}

.sb-break-fade-enter-from,
.sb-break-fade-leave-to {
  opacity: 0;
}

.sb-break-window.is-prompt {
  width: 320px;
  height: 92px;
  pointer-events: none;
  z-index: 2;
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.28);
  animation: sb-prompt-flash 1s infinite alternate ease-in-out;
}

@keyframes sb-prompt-flash {
  0% {
    background-color: rgba(46, 52, 64, 0.95);
  }
  100% {
    background-color: rgba(122, 109, 86, 0.92);
  }
}

.sb-break-window.is-fullscreen {
  width: 100%;
  height: 100%;
  z-index: 3;
  background-color: #2e3440;
}

.sb-qprogressbar {
  width: 100%;
  height: 4px;
  background: transparent;
  flex-shrink: 0;
}

.sb-break-window.is-fullscreen .sb-qprogressbar {
  height: 7px;
}

.sb-qprogressbar-chunk {
  height: 100%;
  background: #eceff4;
  transition: width 60ms linear;
}

.sb-break-center {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding-bottom: 24px;
}

.sb-break-window.is-prompt .sb-break-center {
  padding-bottom: 4px;
}

.sb-break-label {
  color: #eceff4;
  font-size: 16px;
  font-weight: 400;
  transition: font-size 0.25s ease;
}

.sb-break-window.is-fullscreen .sb-break-label {
  font-size: 28px;
  margin-bottom: 4px;
}

.sb-countdown-label {
  color: #eceff4;
  font-size: 68px;
  font-weight: 400;
  line-height: 1.1;
  font-variant-numeric: tabular-nums;
  margin: 4px 0 24px;
}

.sb-action-buttons {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 32px;
  opacity: 0;
  pointer-events: none;
  transform: translateY(6px);
  transition:
    opacity 0.25s ease,
    transform 0.25s ease;
}

.sb-action-buttons.visible {
  opacity: 1;
  pointer-events: auto;
  transform: translateY(0);
}

.sb-btn-group {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
}

.sb-circle-btn {
  width: 64px;
  height: 64px;
  border-radius: 50%;
  border: none;
  background: transparent;
  color: #eceff4;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: background-color 0.15s ease;
  padding: 0;
}

.sb-circle-btn:hover {
  background: rgba(236, 239, 244, 0.16);
}

.sb-bi-icon {
  font-family: 'bootstrap-icons';
  font-size: 42px;
  line-height: 1;
  color: #eceff4;
  display: block;
}

.sb-bi-exit {
  padding-top: 4px;
}

.sb-btn-caption {
  font-size: 12px;
  color: #eceff4;
}

/* Fixed-Width Visual HUD Pill (Consolidated via CSS variables) */
.sb-hud-pill {
  --hud-dot: #22c55e;
  --hud-text: #86efac;
  position: absolute;
  bottom: 14px;
  left: 50%;
  transform: translateX(-50%);
  z-index: 4;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  width: min(460px, calc(100% - 24px));
  padding: 7px 16px;
  border-radius: 999px;
  background: rgba(15, 23, 42, 0.88);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.16);
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.3);
  color: #f8fafc;
  font-size: 13px;
  font-variant-numeric: tabular-nums;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.sb-hud-pill.waiting,
.sb-hud-pill.grace {
  --hud-dot: #eab308;
  --hud-text: #fde047;
}

.sb-hud-pill.locked {
  --hud-dot: #38bdf8;
  --hud-text: #bae6fd;
}

.sb-hud-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  flex-shrink: 0;
  background: var(--hud-dot);
  box-shadow: 0 0 8px var(--hud-dot);
}

.sb-hud-state {
  color: var(--hud-text);
}

.sb-hud-sep {
  color: #64748b;
}

.sb-hud-detail {
  color: #e2e8f0;
}
</style>
