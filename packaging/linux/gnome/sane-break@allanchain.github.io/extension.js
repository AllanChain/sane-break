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

const SANE_WINDOW_NAME = "Break reminder - Sane Break";

export default class SaneBreakGNOME extends Extension {
  _windowAddedHandle = null;

  enable() {
    this._windowAddedHandle = global.display.connect("window-created", (_, window) =>
      window.get_compositor_private().connect("realize", () => this.onCreated(window)),
    );
  }

  disable() {
    if (this._windowAddedHandle) global.display.disconnect(this._windowAddedHandle);
  }

  onCreated(metaWindow) {
    if (metaWindow.get_title() === SANE_WINDOW_NAME) {
      metaWindow.stick();
      metaWindow.raise_and_make_recent_on_workspace(metaWindow.get_workspace());
      metaWindow.make_above();
      const [w, h] = global.display.get_size();
      metaWindow.move_resize_frame(false, 0, 0, w, h);
    }
  }
}
