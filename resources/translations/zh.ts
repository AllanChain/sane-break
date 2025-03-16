<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="51"/>
        <source>Failed to connect to Flatpak response</source>
        <translation>无法获取 Flatpak 回复</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="73"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="79"/>
        <source>File not writable</source>
        <translation>文件不可写入</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="96"/>
        <source>The request to autostart was cancelled.</source>
        <translation>自动启动请求被拒绝。</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="25"/>
        <source>Permission denied</source>
        <translation>权限被拒绝</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="42"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="28"/>
        <source>Unknown error</source>
        <translation>未知错误</translation>
    </message>
</context>
<context>
    <name>BreakWindow</name>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="72"/>
        <source>Time for a big break</source>
        <translation>大休息时间</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="73"/>
        <source>Time for a small break</source>
        <translation>小休息时间</translation>
    </message>
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="24"/>
        <location filename="../../src/gui/widgets/language-select.cpp" line="60"/>
        <source>Default</source>
        <translation>默认语言</translation>
    </message>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="35"/>
        <source>English</source>
        <comment>current language</comment>
        <translation>简体中文</translation>
    </message>
</context>
<context>
    <name>PrefWindow</name>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="71"/>
        <source>Break</source>
        <translation>休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="90"/>
        <source>Sound</source>
        <translation>声音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="106"/>
        <source>Pause</source>
        <translation>暂停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="122"/>
        <source>General</source>
        <translation>通用</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="144"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="340"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <translation>休息提醒弹出后允许继续工作的时长（秒），超时则强制休息。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="350"/>
        <source>Length of work time between breaks</source>
        <translation>两次休息之间的工作时间</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="353"/>
        <source>Break every</source>
        <translation>休息提示间隔</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="333"/>
        <source>Small break length</source>
        <translation>小休息时长</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="438"/>
        <source>Big break length</source>
        <translation>大休息时长</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="218"/>
        <source>Every so many breaks will be a big break</source>
        <translation>每这么多次休息开始大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="221"/>
        <source>Big break every</source>
        <translation>每几次为大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="343"/>
        <source>Flash break prompt for</source>
        <translation>窗口最长闪烁时间</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="253"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <translation>休息提示开始后，在给定时间内持续处于空闲状态，则将自动进入强制休息状态，不会因为用户输入而重新计时。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="256"/>
        <source>Confirm break after</source>
        <translation>确认休息时间</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="505"/>
        <source>Let&apos;s say you&apos;ve configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <translation>假设设置为 2 分钟，则如果休息持续时间短于 2 分钟，只要休息
结束后用户没有活动，屏幕会在休息提示开始后的 2 分钟自动锁
定。如果休息持续超过 2 分钟，则屏幕必然会自动锁定。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="510"/>
        <source>Automatic screen lock after break start</source>
        <translation>休息开始一段时间后自动锁屏</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="530"/>
        <source>Please grant accessibility permission and restart app to use auto lock screen</source>
        <translation>请授予辅助功能权限并重启应用以使用自动锁屏功能</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="878"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;帮我们翻译！&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="890"/>
        <source>Enable autostart</source>
        <translation>开机自动启动</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="944"/>
        <source>&lt;div&gt;
&lt;span style=&quot;font-size: 26px; font-weight: bold&quot;&gt;Sane Break&lt;/span&gt;
v%1
&lt;/div&gt;
&lt;p&gt;Remind breaks without mindless skips. &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break developers&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</source>
        <translation>&lt;div&gt;
&lt;span style=&quot;font-size: 26px; font-weight: bold&quot;&gt;Sane Break&lt;/span&gt;
v%1
&lt;/div&gt;
&lt;p&gt;拒绝无意识跳过的休息提示器 &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break developers&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="330"/>
        <source>Duration of small breaks (in seconds)</source>
        <translation>小休息持续多少秒</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="435"/>
        <source>Duration of big breaks (in seconds)</source>
        <translation>大休息持续多少秒</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="473"/>
        <source>Postpone minutes</source>
        <translation>推迟分钟数</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="571"/>
        <location filename="../../src/gui/pref-window.ui" line="595"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>如果没有声音，请尝试重启 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="574"/>
        <source>Bell after small break</source>
        <translation>小休息后提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="581"/>
        <location filename="../../src/gui/pref-window.ui" line="612"/>
        <location filename="../../src/gui/pref-window.ui" line="647"/>
        <location filename="../../src/gui/pref-window.ui" line="654"/>
        <source>Play</source>
        <translation>播放</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="598"/>
        <source>Bell before small break</source>
        <translation>小休息前提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="605"/>
        <source>Bell before big break</source>
        <translation>大休息前提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="626"/>
        <source>Bell after big break</source>
        <translation>大休息后提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="663"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:\sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;提示&lt;/strong&gt;：你可以这样使用本地文件 &lt;code&gt;file:/D:\sound.mp3&lt;/code&gt;  (Windows) 或 &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux)。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="705"/>
        <source>Reset break after paused for</source>
        <translation>暂停一段时间后重置计时</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="712"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <translation>在这么多时间内没有活动，Sane Break 将暂停计时</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="715"/>
        <source>Pause if idle for</source>
        <translation>空闲一段时间后暂停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="754"/>
        <source>Reset cycle after paused for</source>
        <translation>暂停一段时间后重置循环</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="814"/>
        <source>Pause on battery</source>
        <translation>电池供电时暂停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="821"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>当以下程序运行时暂停休息提示：</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="828"/>
        <source>(Write one program name per line)</source>
        <translation>（每一行写一个程序名）</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="851"/>
        <source>Configuration file</source>
        <translation>配置文件</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1031"/>
        <source>Revert to Last Saved</source>
        <translation>恢复上次设置</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="841"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="868"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>改变语言将在重启应用后生效。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1054"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
</context>
<context>
    <name>PreferenceWindow</name>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="138"/>
        <location filename="../../src/gui/pref-window.cpp" line="160"/>
        <location filename="../../src/gui/pref-window.cpp" line="182"/>
        <location filename="../../src/gui/pref-window.cpp" line="183"/>
        <location filename="../../src/gui/pref-window.cpp" line="184"/>
        <location filename="../../src/gui/pref-window.cpp" line="245"/>
        <location filename="../../src/gui/pref-window.cpp" line="251"/>
        <location filename="../../src/gui/pref-window.cpp" line="258"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="140"/>
        <location filename="../../src/gui/pref-window.cpp" line="154"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>每 %n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="146"/>
        <location filename="../../src/gui/pref-window.cpp" line="165"/>
        <location filename="../../src/gui/pref-window.cpp" line="172"/>
        <location filename="../../src/gui/pref-window.cpp" line="181"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n 秒</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="152"/>
        <source>%n break(s)</source>
        <translation>
            <numerusform>%n 次</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="180"/>
        <source>Disabled</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="190"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>在中键托盘图标后，立刻开始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="194"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>在双击托盘图标后，立刻开始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="290"/>
        <source>Setting auto start failed</source>
        <translation>设置开机自启失败</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="342"/>
        <source>The preferences have been modified.</source>
        <translation>偏好设置已被修改。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="343"/>
        <source>Do you want to save your changes?</source>
        <translation>是否保存修改？</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="86"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>托盘不可用，将使用普通窗口</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="92"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>窗口合成器不支持 layer shell 协议，窗口位置会有问题</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="96"/>
        <source>Sane Break is not compiled with layer shell support, and window positioning will be bugged</source>
        <translation>Sane Break 在编译时为加入 layer shell 支持，窗口位置会有问题</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="100"/>
        <source>You need to install the shell extension if you are using GNOME. Otherwise, Sane break is almost unusable</source>
        <translation>如果你在使用 GNOME，那么需要安装对应的 shell 扩展，否则 Sane Break 不能正常使用</translation>
    </message>
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <location filename="../../src/app.cpp" line="140"/>
        <source>big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="140"/>
        <source>small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="152"/>
        <source>Next Break</source>
        <translation>下次休息</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="156"/>
        <source>Big Break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="165"/>
        <source>Postpone</source>
        <translation>推迟</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app.cpp" line="168"/>
        <location filename="../../src/app.cpp" line="353"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分钟</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="188"/>
        <location filename="../../src/app.cpp" line="194"/>
        <location filename="../../src/app.cpp" line="362"/>
        <source>Quit</source>
        <translation>退出</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="171"/>
        <source>Enable Break</source>
        <translation>启用休息</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="143"/>
        <source>Next break after %1</source>
        <translation>%1 后休息</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="147"/>
        <source>Next big break after %1</source>
        <translation>%1 后大休息</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="177"/>
        <location filename="../../src/app.cpp" line="267"/>
        <source>Enter Reading Mode</source>
        <translation>进入阅读模式</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="182"/>
        <source>Preferences</source>
        <translation>偏好设置</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app.cpp" line="191"/>
        <location filename="../../src/app.cpp" line="359"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>推迟 %n 分钟</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="228"/>
        <source>Paused on battery</source>
        <translation>因电池供电而暂停</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="230"/>
        <source>Paused on app running</source>
        <translation>因特定应用运行而暂停</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="232"/>
        <source>Paused on idle</source>
        <translation>因用户空闲而暂停</translation>
    </message>
    <message>
        <location filename="../../src/app.cpp" line="270"/>
        <source>Exit Reading Mode</source>
        <translation>退出阅读模式</translation>
    </message>
</context>
<context>
    <name>WelcomeWindow</name>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="107"/>
        <source>Welcome to Sane Break</source>
        <translation>欢迎使用 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="108"/>
        <source>&lt;h3 align=center&gt;Welcome to Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow. Sane Break will stay in the system tray and remind you to take breaks at regular intervals. To quit, go to &quot;Postpone&quot; in the tray menu.&lt;/p&gt;</source>
        <translation>&lt;h3 align=center&gt;欢迎使用 Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break 是一款跨平台的休息提醒器，旨在帮助你在不影响工作流程的情况下进行有意义的休息。Sane Break 会停留在系统托盘中，提醒你按时休息。若要退出，请进入托盘菜单中的 “推迟”。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="87"/>
        <location filename="../../src/gui/welcome.cpp" line="117"/>
        <source>Read More</source>
        <translation>了解更多</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="89"/>
        <location filename="../../src/gui/welcome.cpp" line="118"/>
        <source>Ignore</source>
        <translation>忽略</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="90"/>
        <location filename="../../src/gui/welcome.cpp" line="119"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="93"/>
        <location filename="../../src/gui/welcome.cpp" line="120"/>
        <source>OK</source>
        <translation>好的</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="115"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
</context>
</TS>
