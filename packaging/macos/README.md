# MacOS note

## Compiling icon files

Reference:
- https://praeclarum.org/2025/09/12/app-icons.html
- https://successfulsoftware.net/2025/09/26/updating-application-icons-for-macos-26-tahoe-and-liquid-glass/

```bash
xcrun actool packaging/macos/icon.icon --compile packaging/macos \
    --output-partial-info-plist /tmp/ignore-it.plist --app-icon icon \
    --minimum-deployment-target 11.0 --platform macosx --target-device mac
```
