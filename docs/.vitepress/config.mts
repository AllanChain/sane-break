import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'Sane Break',
  description: 'A gentle, cross-platform break reminder that helps you take meaningful rests without disrupting your workflow.',
  base: '/sane-break/',
  cleanUrls: true,
  vite: {
    server: {
      fs: {
        allow: ['..'],
      },
    },
  },
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/sane-break/icon.svg' }],
  ],
  themeConfig: {
    logo: '/icon.svg',
    nav: [
      { text: 'Installation', link: '/installation' },
      { text: 'User Guide', link: '/breaks-and-schedule' },
      { text: 'CLI', link: '/cli' },
      { text: 'Hooks', link: '/hooks' },
    ],
    sidebar: [
      {
        text: 'Getting Started',
        items: [
          { text: 'Installation', link: '/installation' },
        ],
      },
      {
        text: 'User Guide',
        items: [
          { text: 'Breaks and Schedules', link: '/breaks-and-schedule' },
          { text: 'Focus, Meetings, and Postponing', link: '/modes-and-postponing' },
          { text: 'Automatic Pause and Idle Reset', link: '/automatic-pause' },
          { text: 'Customization and Statistics', link: '/customization' },
        ],
      },
      {
        text: 'Automation',
        items: [
          { text: 'CLI Control', link: '/cli' },
          { text: 'Hook Scripts', link: '/hooks' },
        ],
      },
    ],
    socialLinks: [
      { icon: 'github', link: 'https://github.com/AllanChain/sane-break' },
    ],
    search: {
      provider: 'local',
    },
    footer: {
      message: 'Released under the GPL-3.0-or-later License.',
      copyright: 'Copyright © 2024-2026 Sane Break developers',
    },
  },
})
