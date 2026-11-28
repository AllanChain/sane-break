import DefaultTheme from 'vitepress/theme'
import type { EnhanceAppContext } from 'vitepress'
import HomeBreakDemo from './HomeBreakDemo.vue'
import './custom.css'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }: EnhanceAppContext) {
    app.component('HomeBreakDemo', HomeBreakDemo)
  }
}
