/* https://github.com/bennypowers/gnome-shell-extension-firefox-pip/blob/main/extension.js
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import { Extension } from "resource:///org/gnome/shell/extensions/extension.js";
import GLib from "gi://GLib";

/**
 * Makes Sane Break's Wayland fallback window behave like a fullscreen overlay on GNOME.
 *
 * The app creates a transparent always-on-top window and relies on this extension to
 * stretch it across the monitor. The extension watches for newly created windows,
 * waits until the Sane Break break window gets its final title, then defers the
 * resize/restack work until the compositor has produced the first frame. A short
 * timeout fallback is kept because some windows may never emit the expected actor
 * signal in practice.
 */
const SANE_WINDOW_NAME = "Break reminder - Sane Break";

export default class SaneBreakGNOME extends Extension {
  _windowAddedHandle = null;
  _pendingSourceIds = new Set();

  enable() {
    this._windowAddedHandle = global.display.connect("window-created", (_, window) =>
      this.onCreated(window),
    );
  }

  disable() {
    if (this._windowAddedHandle) {
      global.display.disconnect(this._windowAddedHandle);
      this._windowAddedHandle = null;
    }
    for (const sourceId of this._pendingSourceIds) GLib.Source.remove(sourceId);
    this._pendingSourceIds.clear();
  }

  onCreated(window) {
    window.connect("notify::title", () => {
      const title = window.get_title();
      if (title !== SANE_WINDOW_NAME) return;
      // The window is identified by title only after the Qt surface is fully named.
      this._scheduleWindowWorkaround(window);
    });
  }

  _scheduleWindowWorkaround(window) {
    let applied = false;
    const actor = window.get_compositor_private();

    const applyOnce = () => {
      if (applied) return GLib.SOURCE_REMOVE;
      applied = true;
      this._applyWindowWorkaround(window);
      return GLib.SOURCE_REMOVE;
    };

    if (actor) {
      // On recent GNOME releases, move/resize can be ignored if it runs before the
      // compositor has drawn the first frame for the new window.
      actor.connect("first-frame", () => {
        const sourceId = GLib.idle_add(GLib.PRIORITY_DEFAULT_IDLE, () => {
          this._pendingSourceIds.delete(sourceId);
          return applyOnce();
        });
        this._pendingSourceIds.add(sourceId);
      });
      // Keep a bounded fallback in case the actor never emits first-frame.
      const sourceId = GLib.timeout_add(GLib.PRIORITY_DEFAULT, 250, () => {
        this._pendingSourceIds.delete(sourceId);
        return applyOnce();
      });
      this._pendingSourceIds.add(sourceId);
      return;
    }

    const sourceId = GLib.timeout_add(GLib.PRIORITY_DEFAULT, 250, () => {
      this._pendingSourceIds.delete(sourceId);
      return applyOnce();
    });
    this._pendingSourceIds.add(sourceId);
  }

  _applyWindowWorkaround(window) {
    // GNOME does not expose layer-shell here, so the shell extension forcefully
    // stretches the transparent Wayland workaround window to cover the monitor.
    const [w, h] = global.display.get_size();
    window.stick();
    window.move_resize_frame(false, 0, 0, w, h);
    window.raise_and_make_recent_on_workspace(window.get_workspace());
    window.make_above();
  }
}
