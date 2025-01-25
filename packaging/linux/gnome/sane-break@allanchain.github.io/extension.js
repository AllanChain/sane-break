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
