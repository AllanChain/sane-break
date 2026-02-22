<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="54"/>
        <source>Failed to connect to Flatpak response</source>
        <extracomment>Error message when requesting autostart in Flatpak got no response</extracomment>
        <translation>无法获取 Flatpak 回复</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="63"/>
        <source>Failed to read desktop entry packaged with app</source>
        <translation>无法读取应用自带的自启动文件</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="77"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="87"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="93"/>
        <source>Autostart desktop entry not writable</source>
        <extracomment>Error message when failed to write Linux desktop entry for autostart</extracomment>
        <translation>自动启动文件无法被写入</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="111"/>
        <source>The request to autostart was cancelled</source>
        <extracomment>Error message when requesting autostart in Flatpak got cancelled by user</extracomment>
        <translation>自动启动请求被拒绝</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="45"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="62"/>
        <source>Disallowed by policy</source>
        <translation>被系统策略禁止</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="48"/>
        <source>Disallowed by user</source>
        <translation>被用户设置禁止</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="82"/>
        <source>Unknown error in MSIX autostart</source>
        <translation>MSIX 自启动遇到未知错误</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="102"/>
        <source>Permission denied</source>
        <extracomment>Error message when requesting autostart got permission denied</extracomment>
        <translation>权限被拒绝</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="44"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="105"/>
        <source>Unknown error</source>
        <extracomment>Error message when requesting autostart got unknown error</extracomment>
        <translation>未知错误</translation>
    </message>
</context>
<context>
    <name>BreakReminder</name>
    <message>
        <location filename="../../src/app/break-window.ui" line="210"/>
        <source>Lock Screen</source>
        <translation>锁屏</translation>
    </message>
    <message>
        <location filename="../../src/app/break-window.ui" line="264"/>
        <source>Exit Force Break</source>
        <translation>退出强制休息</translation>
    </message>
</context>
<context>
    <name>BreakWindow</name>
    <message>
        <location filename="../../src/app/break-window.cpp" line="149"/>
        <source>Break will end at: %1</source>
        <translation>休息将在 %1 结束</translation>
    </message>
    <message>
        <location filename="../../src/app/break-window.cpp" line="151"/>
        <source>Break has ended</source>
        <translation>休息已结束</translation>
    </message>
</context>
<context>
    <name>DayTimelineWidget</name>
    <message>
        <location filename="../../src/app/widgets/timeline-view.cpp" line="49"/>
        <source>Active</source>
        <translation>活跃</translation>
    </message>
    <message>
        <location filename="../../src/app/widgets/timeline-view.cpp" line="50"/>
        <source>Break</source>
        <translation>休息</translation>
    </message>
    <message>
        <location filename="../../src/app/widgets/timeline-view.cpp" line="51"/>
        <source>Pause</source>
        <translation>暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/widgets/timeline-view.cpp" line="53"/>
        <source>Meeting</source>
        <translation>会议</translation>
    </message>
    <message>
        <location filename="../../src/app/widgets/timeline-view.cpp" line="54"/>
        <source>Focus</source>
        <translation>专注</translation>
    </message>
    <message>
        <location filename="../../src/app/widgets/timeline-view.cpp" line="56"/>
        <source>Postpone</source>
        <translation>推迟</translation>
    </message>
</context>
<context>
    <name>FocusUI</name>
    <message>
        <location filename="../../src/app/focus-window.ui" line="14"/>
        <source>Focus Mode</source>
        <translation>专注模式</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="20"/>
        <source>&lt;h2&gt;Focus Mode&lt;/h2&gt;</source>
        <translation>&lt;h2&gt;专注模式&lt;/h2&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="27"/>
        <source>Briefly describe what are you working on:</source>
        <translation>简要说明正在进行的工作：</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="55"/>
        <source>Focus duration</source>
        <translation>专注时长</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="62"/>
        <source> min</source>
        <translation> 分钟</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="125"/>
        <source>The break schedule will be temporarily switched to:</source>
        <translation>休息计划会被暂时调整为：</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="142"/>
        <source>A &lt;b&gt;%1&lt;/b&gt; seconds small break every &lt;b&gt;%2&lt;/b&gt; minutes</source>
        <translation>每 &lt;b&gt;%2&lt;/b&gt; 分钟一次 &lt;b&gt;%1&lt;/b&gt; 秒的小休息</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="159"/>
        <source>1 in every &lt;b&gt;%2&lt;/b&gt; breaks is a &lt;b&gt;%1&lt;/b&gt; seconds big break</source>
        <translation>每 &lt;b&gt;%2&lt;/b&gt; 次休息是一次 &lt;b&gt;%1&lt;/b&gt; 秒的大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="188"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/app/focus-window.ui" line="201"/>
        <source>Take a %1 second break to enter focus mode</source>
        <translation>休息 %1 秒以进入专注模式</translation>
    </message>
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/app/widgets/language-select.cpp" line="67"/>
        <location filename="../../src/app/widgets/language-select.cpp" line="94"/>
        <source>Default</source>
        <translation>默认语言</translation>
    </message>
    <message>
        <location filename="../../src/app/widgets/language-select.cpp" line="79"/>
        <source>English</source>
        <comment>current language</comment>
        <extracomment>This is the name of the current language in the current language</extracomment>
        <translation>简体中文</translation>
    </message>
</context>
<context>
    <name>MeetingPrompt</name>
    <message>
        <location filename="../../src/app/meeting-prompt.ui" line="14"/>
        <source>Meeting Mode Ended</source>
        <translation>会议模式结束</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-prompt.ui" line="20"/>
        <source>&lt;h2&gt;Meeting Mode Ended&lt;/h2&gt;</source>
        <translation>&lt;h2&gt;会议模式结束&lt;/h2&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-prompt.ui" line="30"/>
        <source>Your meeting/presentation mode has ended. Would you like to take a break?</source>
        <translation>您会议或演示模式已经结束。准备好休息一下了吗？</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-prompt.ui" line="63"/>
        <source>Break in %1 minutes</source>
        <translation>%1 分钟后休息</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-prompt.ui" line="70"/>
        <source>Not yet (extend %1 minutes)</source>
        <translation>不是现在（延长 %1 分钟）</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-prompt.ui" line="53"/>
        <source>Break now (%1s)</source>
        <translation>现在休息 (%1s)</translation>
    </message>
</context>
<context>
    <name>MeetingUI</name>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="14"/>
        <source>Meeting Mode</source>
        <translation>会议模式</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="20"/>
        <source>&lt;h2&gt;Meeting/Presentation Mode&lt;/h2&gt;</source>
        <translation>&lt;h2&gt;会议/演示 模式&lt;/h2&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="27"/>
        <source>Briefly describe this meeting/presentation:</source>
        <translation>简要说明这次会议/演示的内容：</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="55"/>
        <source>It will end at</source>
        <translation>预计结束于</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="116"/>
        <source>No break reminders will show before &lt;b&gt;%1&lt;/b&gt;</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; 之前不会有休息提示</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="151"/>
        <source>We&apos;ll check in with you before showing a break reminder.</source>
        <translation>在显示休息提示前，我们会先与您确认。</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="178"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/app/meeting-window.ui" line="191"/>
        <source>Enter Meeting/Presentation Mode</source>
        <translation>进入会议/演示模式</translation>
    </message>
</context>
<context>
    <name>PostponeUI</name>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="14"/>
        <source>Postpone</source>
        <translation>推迟</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="20"/>
        <source>&lt;h2&gt;Postponing this break&lt;/h2&gt;</source>
        <translation>&lt;h2&gt;推迟此次休息&lt;/h2&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="27"/>
        <source>Postponing this break by &lt;b&gt;%1&lt;/b&gt; minutes will:</source>
        <translation>推迟此次休息 &lt;b&gt;%1&lt;/b&gt; 分钟将：</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="57"/>
        <source>Shorten your next work session by &lt;b&gt;%1&lt;/b&gt; minutes;</source>
        <translation>缩短下次工作时段&lt;b&gt;%1&lt;/b&gt;分钟；</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="92"/>
        <source>Extend the break by &lt;b&gt;%1&lt;/b&gt; seconds.</source>
        <translation>延长 &lt;b&gt;%1&lt;/b&gt; 秒休息时间。</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="122"/>
        <source>Postpone by</source>
        <translation>推迟时长</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="129"/>
        <source> min</source>
        <translation> 分钟</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="187"/>
        <source>You won&apos;t be able to postpone this break again. It will start automatically at the scheduled time.</source>
        <translation>你将无法再次推迟这次休息。它将在预定时间自动开始。</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="222"/>
        <source>You can still start the break early. If you do, the adjustments above will be recalculated based on the actual time.</source>
        <translation>你仍可提前开始休息。若提前开始，上述调整将根据实际时间重新计算。</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="249"/>
        <source>Keep original schedule</source>
        <translation>保持原计划</translation>
    </message>
    <message>
        <location filename="../../src/app/postpone-window.ui" line="262"/>
        <source>Do the adjustments</source>
        <translation>应用修改</translation>
    </message>
</context>
<context>
    <name>PrefWindow</name>
    <message>
        <location filename="../../src/app/pref-window.ui" line="126"/>
        <source>Pause</source>
        <translation>暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="161"/>
        <source>General</source>
        <translation>通用</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="183"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1763"/>
        <location filename="../../src/app/pref-window.ui" line="1801"/>
        <source>Bell before breaks</source>
        <translation>休息前提示音</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1718"/>
        <location filename="../../src/app/pref-window.ui" line="1843"/>
        <source>Bell after breaks</source>
        <translation>休息后提示音</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="914"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <extracomment>Tooltip for &quot;Flash break prompt for&quot; config</extracomment>
        <translation>休息提醒弹出后允许继续工作的时长（秒），超时则强制休息。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="75"/>
        <source>Schedule</source>
        <translation>休息计划</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="94"/>
        <source>Reminder</source>
        <translation>提示窗口</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="142"/>
        <source>Sound</source>
        <translation>声音</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="379"/>
        <location filename="../../src/app/pref-window.ui" line="625"/>
        <source>Length of work time between breaks</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (small break)</extracomment>
        <translation>两次休息之间的工作时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="382"/>
        <location filename="../../src/app/pref-window.ui" line="429"/>
        <location filename="../../src/app/pref-window.ui" line="628"/>
        <location filename="../../src/app/pref-window.ui" line="675"/>
        <source>Break every</source>
        <translation>休息提示间隔</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="426"/>
        <location filename="../../src/app/pref-window.ui" line="672"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (big break)</extracomment>
        <translation>每这么多次休息开始大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="924"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <extracomment>Tooltip for &quot;Confirm break after&quot; config</extracomment>
        <translation>休息提示开始后，在给定时间内持续处于空闲状态，则将自动进入强制休息状态，不会因为用户输入而重新计时。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="927"/>
        <source>Confirm break after</source>
        <translation>确认休息时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1028"/>
        <source>Close windows right after small break ends</source>
        <translation>小休息结束后立即关闭窗口</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1035"/>
        <source>Close windows right after big break ends</source>
        <translation>大休息结束后立即关闭窗口</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1061"/>
        <source>Flash speed</source>
        <translation>闪烁速度</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1116"/>
        <location filename="../../src/app/pref-window.ui" line="1149"/>
        <source>(One line per message)</source>
        <translation>（每行一条提示文本）</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1123"/>
        <source>Big break prompt messages:</source>
        <translation>大休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1133"/>
        <source>Small break prompt messages:</source>
        <translation>小休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1012"/>
        <source>Max number of force break exits</source>
        <translation>退出强制休息次数上限</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="110"/>
        <source>Interface</source>
        <translation>界面</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1019"/>
        <source> times</source>
        <translation> 次</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1158"/>
        <source>&lt;h3&gt;Components to Show&lt;/h3&gt;

&lt;p&gt;User interface components to show in the prompt and full-screen window.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;显示组件&lt;/h3&gt;

&lt;p&gt;在提示小窗和全屏窗口中显示的用户界面组件。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1183"/>
        <location filename="../../src/app/pref-window.ui" line="1229"/>
        <source>Time when break ends</source>
        <translation>休息结束时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1204"/>
        <location filename="../../src/app/pref-window.ui" line="1250"/>
        <source>Countdown timer</source>
        <translation>倒计时</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1176"/>
        <location filename="../../src/app/pref-window.ui" line="1222"/>
        <source>Progress bar</source>
        <translation>进度条</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1190"/>
        <location filename="../../src/app/pref-window.ui" line="1236"/>
        <source>Clock</source>
        <translation>时钟</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1197"/>
        <location filename="../../src/app/pref-window.ui" line="1243"/>
        <source>Buttons</source>
        <translation>按钮</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1259"/>
        <source>&lt;h3&gt;Color Theme&lt;/h3&gt;
All color are in &lt;code&gt;#AARRGGBB&lt;/code&gt; format.</source>
        <translation>&lt;h3&gt;颜色主题&lt;/h3&gt;
所有颜色的格式均为 &lt;code&gt;#AARRGGBB&lt;/code&gt;。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1343"/>
        <source>Big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1312"/>
        <source>Message</source>
        <translation>提示文本</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1350"/>
        <source>Small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1319"/>
        <source>Background</source>
        <translation>背景</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1326"/>
        <source>Count down</source>
        <translation>倒计时</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1425"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;暂停条件&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1636"/>
        <source>Long pause threshold</source>
        <translation>长时间暂停阈值</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1690"/>
        <source>Short pause threshold</source>
        <translation>短时间暂停阈值</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1855"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: Use &lt;i&gt;Browse&lt;/i&gt; to select a custom sound file. A copy of the file will be saved so it can always be played.</source>
        <translation>&lt;strong&gt;提示&lt;/strong&gt;: 使用 &lt;i&gt;浏览&lt;/i&gt; 来选取自定义的声音文件。文件会被复制一份以保证可以被播放。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1949"/>
        <source>Let&apos;s say you&apos;ve configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <extracomment>Tooltip for &quot;Auto screen lock&quot; config</extracomment>
        <translation>假设设置为 2 分钟，则如果休息持续时间短于 2 分钟，只要休息
结束后用户没有活动，屏幕会在休息提示开始后的 2 分钟自动锁
定。如果休息持续超过 2 分钟，则屏幕必然会自动锁定。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1954"/>
        <source>Automatic screen lock after break start</source>
        <translation>休息开始一段时间后自动锁屏</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1900"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;帮我们翻译！&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="2071"/>
        <source>&lt;div&gt;
&lt;span style=&quot;font-size: 26px; font-weight: bold&quot;&gt;Sane Break&lt;/span&gt;
v%1
&lt;/div&gt;
&lt;p&gt;Remind breaks without mindless skips. &lt;/p&gt;
&lt;p&gt;2024-2026, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break developers&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</source>
        <translation>&lt;div&gt;
&lt;span style=&quot;font-size: 26px; font-weight: bold&quot;&gt;Sane Break&lt;/span&gt;
v%1
&lt;/div&gt;
&lt;p&gt;拒绝无意识跳过的休息提示器 &lt;/p&gt;
&lt;p&gt;2024-2026, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break developers&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="325"/>
        <location filename="../../src/app/pref-window.ui" line="571"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (small break)</extracomment>
        <translation>小休息持续多少秒</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="499"/>
        <location filename="../../src/app/pref-window.ui" line="745"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (big break)</extracomment>
        <translation>大休息持续多少秒</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1046"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;提示小窗外观&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1715"/>
        <location filename="../../src/app/pref-window.ui" line="1760"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>如果没有声音，请尝试重启 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1732"/>
        <location filename="../../src/app/pref-window.ui" line="1746"/>
        <location filename="../../src/app/pref-window.ui" line="1808"/>
        <location filename="../../src/app/pref-window.ui" line="1822"/>
        <source>Play</source>
        <translation>播放</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1434"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for &quot;Pause if idle for&quot; config</extracomment>
        <translation>在这么多时间内没有活动，Sane Break 将暂停计时</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1437"/>
        <source>Pause if idle for</source>
        <translation>空闲一段时间后暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="275"/>
        <location filename="../../src/app/pref-window.ui" line="521"/>
        <location filename="../../src/app/pref-window.ui" line="1706"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;小休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="335"/>
        <location filename="../../src/app/pref-window.ui" line="436"/>
        <location filename="../../src/app/pref-window.ui" line="581"/>
        <location filename="../../src/app/pref-window.ui" line="682"/>
        <location filename="../../src/app/pref-window.ui" line="1478"/>
        <location filename="../../src/app/pref-window.ui" line="1620"/>
        <location filename="../../src/app/pref-window.ui" line="1674"/>
        <source> min</source>
        <translation> 分钟</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="284"/>
        <location filename="../../src/app/pref-window.ui" line="530"/>
        <location filename="../../src/app/pref-window.ui" line="873"/>
        <location filename="../../src/app/pref-window.ui" line="962"/>
        <location filename="../../src/app/pref-window.ui" line="992"/>
        <source> sec</source>
        <translation> 秒钟</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="328"/>
        <location filename="../../src/app/pref-window.ui" line="502"/>
        <location filename="../../src/app/pref-window.ui" line="574"/>
        <location filename="../../src/app/pref-window.ui" line="748"/>
        <source>Break length</source>
        <translation>休息时长</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="391"/>
        <location filename="../../src/app/pref-window.ui" line="637"/>
        <location filename="../../src/app/pref-window.ui" line="1779"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;大休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="268"/>
        <source>&lt;h2&gt;Normal Schedule&lt;/h2&gt;</source>
        <translation>&lt;h2&gt;正常休息计划&lt;/h2&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="398"/>
        <location filename="../../src/app/pref-window.ui" line="644"/>
        <source>Enabled</source>
        <translation>启用</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="410"/>
        <location filename="../../src/app/pref-window.ui" line="656"/>
        <source> breaks</source>
        <translation> 次休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="511"/>
        <source>&lt;h2&gt;Focus Schedule&lt;/h2&gt;When you activate Focus Mode from the tray, breaks will follow this schedule instead. Use it for shorter breaks when you need to stay  focused.</source>
        <translation>&lt;h2&gt;专注休息计划&lt;/h2&gt;在托盘中启用专注模式将按此计划进行休息。您可以用它来减少休息时长以保持专注。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="757"/>
        <source>&lt;h2&gt;Postpone&lt;/h2&gt;</source>
        <translation>&lt;h2&gt;推迟&lt;/h2&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="764"/>
        <source>Maximum postpone time as a percentage of your work session (&lt;b&gt;%1&lt;/b&gt; min)</source>
        <translation>最大延迟时间占工作时段的百分比（&lt;b&gt;%1&lt;/b&gt; 分钟）</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="801"/>
        <source>Postponing breaks adds extra time to your next long break, proportional to how much time you postponed. You can adjust the multiplier for how much extra time is added.</source>
        <translation>推迟休息时间会为下次长休息增加额外时长，该时长与推迟的时长成正比。您可以调整系数数来控制额外增加的时长。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="857"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;%1&lt;/b&gt; seconds. If you stop your activity during that time, or choose to continue until the timer runs out, it transitions to full-screen. During the break, Sane Break locks into full-screen mode after &lt;b&gt;%2&lt;/b&gt; seconds of inactivity. If you resume activity before the lock, the prompt shrinks to a small window temporarily but re-expands to full-screen when you&apos;re idle again or the timer ends.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;提醒行为&lt;/h3&gt;
&lt;p&gt;Sane Break 会显示一个持续 &lt;b&gt;%1&lt;/b&gt; 秒的小型提示窗。若您在此期间停止活动，或选择继续超过这个时间，界面将转换至全屏模式。休息期间，若持续 &lt;b&gt;%2&lt;/b&gt; 秒无操作，Sane Break 会锁定全屏状态。若在锁定前恢复活动，提示窗会暂时缩成小窗口，但当再次无操作或到时间后，界面将重新转换至全屏。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="917"/>
        <source>Show break prompt for</source>
        <translation>提示窗口显示时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="982"/>
        <source>Tray icon will start flashing this many seconds before the break. Set to 0 to disable.</source>
        <extracomment>Tooltip for &quot;Flash tray icon before break&quot; config</extracomment>
        <translation>闪烁托盘图标会在休息开始前几秒钟开始闪烁。设为 0 以禁用。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="985"/>
        <source>Flash tray icon before break</source>
        <translation>休息开始前闪烁托盘图标</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1167"/>
        <source>&lt;h4&gt;Small break&lt;/h4&gt;</source>
        <translation>&lt;h4&gt;小休息&lt;/h4&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1213"/>
        <source>&lt;h4&gt;Big break&lt;/h4&gt;</source>
        <translation>&lt;h4&gt;大休息&lt;/h4&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1361"/>
        <source>Background image</source>
        <translation>背景图片</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1385"/>
        <source>Clear</source>
        <translation>清除</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1401"/>
        <source>&lt;p&gt;To preview break windows, save all changes first. The preview begins with a 2-second flashing prompt, followed by a 4-second full-screen break. A small break window will appear first, then a big break window.&lt;/p&gt;</source>
        <translation>&lt;p&gt;预览休息窗口前，请先保存所有设置。预览时，会先显示2秒提示小窗，随后进入4秒全屏窗口。先显示小休息窗口，再显示大休息窗口。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1411"/>
        <source>Preview Break Windows</source>
        <translation>预览休息窗口</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1496"/>
        <source>Pause on battery</source>
        <translation>电池供电时暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1503"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>当以下程序运行时暂停休息提示：</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1510"/>
        <source>(Write one program name per line)</source>
        <translation>（每一行写一个程序名）</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1517"/>
        <source>&lt;h3&gt;External Monitors&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;外接显示器&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1524"/>
        <source>Pause when unknown monitor is connected</source>
        <translation>在连接未知显示器时暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1546"/>
        <source>Mark currently connected monitors as known</source>
        <translation>将当前连接的显示器标记为已知</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1566"/>
        <source>Remove selected</source>
        <translation>删除选中</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1575"/>
        <source>&lt;h3&gt;Reset Break Schedule After Pause&lt;/h3&gt;
&lt;p&gt;Sane Break will adjust the break schedule after pausing:&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;If the pause is short, Sane Break will reset the time to the next break.&lt;/li&gt;
  &lt;li&gt;If it is longer, Sane Break will reset the entire break cycle, and the next break will be the first small break in the cycle.&lt;/li&gt;
&lt;/ul&gt;</source>
        <translation>&lt;h3&gt;暂停后重置计时&lt;/h3&gt;
&lt;p&gt;Sane Break 会在暂停后调整计时：&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;短时间暂停后，Sane Break 会重置到下一次休息的时间&lt;/li&gt;
  &lt;li&gt;长时间暂停后，Sane Break 会重置整个休息循环，下一次休息是循环中的第一个小休息&lt;/li&gt;
&lt;/ul&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1378"/>
        <location filename="../../src/app/pref-window.ui" line="1739"/>
        <location filename="../../src/app/pref-window.ui" line="1753"/>
        <location filename="../../src/app/pref-window.ui" line="1815"/>
        <location filename="../../src/app/pref-window.ui" line="1829"/>
        <source>Browse</source>
        <translation>浏览</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1893"/>
        <source>Configuration file</source>
        <translation>配置文件</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1923"/>
        <source>Database file</source>
        <translation>数据库文件</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1969"/>
        <source>Launch Sane Break at start up</source>
        <translation>开机自动启动 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="2095"/>
        <source>Get the Source Code</source>
        <translation>获取源代码</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="2124"/>
        <source>Help Us Translate!</source>
        <translation>帮助我们翻译！</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="2157"/>
        <source>Revert to Last Saved</source>
        <translation>恢复上次设置</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1871"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1910"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>改变语言将在重启应用后生效。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="2115"/>
        <source>Third-party Libraries Licenses</source>
        <translation>第三方库许可</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="2164"/>
        <source>Defaults</source>
        <translation>恢复默认</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="2187"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
</context>
<context>
    <name>PreferenceWindow</name>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="509"/>
        <location filename="../../src/app/pref-window.cpp" line="510"/>
        <location filename="../../src/app/pref-window.cpp" line="511"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="174"/>
        <location filename="../../src/app/pref-window.cpp" line="216"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>每 %n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="508"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n 秒</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="507"/>
        <source>Disabled</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="518"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>在中键托盘图标后，立刻开始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="523"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>在双击托盘图标后，立刻开始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="601"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="602"/>
        <source>Discard</source>
        <translation>放弃</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="603"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="640"/>
        <source>Select Sound File</source>
        <translation>选取声音文件</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="641"/>
        <source>Sound Files (*.mp3 *.wav *.ogg *.flac *.m4a)</source>
        <translation>声音文件 (*.mp3 *.wav *.ogg *.flac *.m4a)</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="651"/>
        <location filename="../../src/app/pref-window.cpp" line="672"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="652"/>
        <source>Failed to save a copy of the selected sound file.</source>
        <translation>保存所选的声音文件失败。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="661"/>
        <source>Select Background Image</source>
        <translation>选择背景图片</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="662"/>
        <source>Image Files (*.png *.jpg *.jpeg *.bmp *.webp)</source>
        <translation>图片文件 (*.png *.jpg *.jpeg *.bmp *.webp)</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="673"/>
        <source>Failed to save a copy of the selected image file.</source>
        <translation>保存所选的图片文件失败。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="483"/>
        <source>Setting autostart failed</source>
        <translation>设置开机自启失败</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="598"/>
        <source>The preferences have been modified.</source>
        <translation>偏好设置已被修改。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="599"/>
        <source>Do you want to save your changes?</source>
        <translation>是否保存修改？</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="59"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>托盘不可用，将使用普通窗口</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="63"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>窗口合成器不支持 layer shell 协议，窗口位置会有问题</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="66"/>
        <source>You need to install the shell extension if you are using GNOME. Otherwise, Sane break is almost unusable</source>
        <translation>如果你在使用 GNOME，那么需要安装对应的 shell 扩展，否则 Sane Break 不能正常使用</translation>
    </message>
    <message>
        <location filename="../../src/app/main.cpp" line="66"/>
        <source>Another instance of Sane Break is running.</source>
        <translation>另一个 Sane Break 实例正在运行。</translation>
    </message>
    <message>
        <location filename="../../src/app/main.cpp" line="67"/>
        <source>Please quit the old instance before starting a new one.</source>
        <translation>请先退出旧的实例，再运行新的。</translation>
    </message>
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <location filename="../../src/app/app.cpp" line="154"/>
        <source>No further postpones are allowed.</source>
        <translation>不能再次推迟。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="147"/>
        <location filename="../../src/app/app.cpp" line="156"/>
        <source>OK</source>
        <translation>好的</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="144"/>
        <source>Cannot postpone during focus mode.</source>
        <translation>专注模式不允许推迟休息。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="145"/>
        <source>End focus mode first if you want to postpone.</source>
        <translation>如需推迟，先退出专注模式。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="153"/>
        <source>You have already postponed this break once.</source>
        <translation>这次休息您已经推迟过一次了。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="184"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation>确定退出 Sane Break？</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="185"/>
        <source>You can postpone the breaks instead.</source>
        <translation>你可以推迟休息而非退出。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="187"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="188"/>
        <source>Postpone</source>
        <translation>推迟</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="189"/>
        <source>Yes</source>
        <translation>是的</translation>
    </message>
</context>
<context>
    <name>SanePreferences</name>
    <message>
        <location filename="../../src/core/preferences.cpp" line="64"/>
        <source>Time for a small break</source>
        <translation>小休息时间</translation>
    </message>
    <message>
        <location filename="../../src/core/preferences.cpp" line="67"/>
        <source>Time for a big break</source>
        <translation>大休息时间</translation>
    </message>
</context>
<context>
    <name>StatsWindow</name>
    <message>
        <location filename="../../src/app/stats-window.ui" line="20"/>
        <source>Statistics</source>
        <translation>统计</translation>
    </message>
    <message>
        <location filename="../../src/app/stats-window.ui" line="32"/>
        <source>%1 breaks (%2 completed)</source>
        <translation>休息 %1 次（完成 %2 次）</translation>
    </message>
    <message>
        <location filename="../../src/app/stats-window.ui" line="39"/>
        <source>Break time: %1</source>
        <translation>休息时间：%1</translation>
    </message>
    <message>
        <location filename="../../src/app/stats-window.ui" line="46"/>
        <source>%1 postponed</source>
        <translation>推迟 %1 次</translation>
    </message>
    <message>
        <location filename="../../src/app/stats-window.ui" line="53"/>
        <source>Active usage: %1</source>
        <translation>活跃使用时长：%1</translation>
    </message>
    <message>
        <location filename="../../src/app/stats-window.ui" line="60"/>
        <source>Total usage: %1</source>
        <translation>总计使用时长：%1</translation>
    </message>
    <message>
        <location filename="../../src/app/stats-window.cpp" line="30"/>
        <source>%1h %2m</source>
        <translation>%1 小时 %2 分钟</translation>
    </message>
    <message>
        <location filename="../../src/app/stats-window.cpp" line="31"/>
        <source>%1m</source>
        <translation>%1 分钟</translation>
    </message>
</context>
<context>
    <name>StatusTrayWindow</name>
    <message>
        <location filename="../../src/app/tray.cpp" line="56"/>
        <source>Postpone</source>
        <translation>推迟</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/tray.cpp" line="80"/>
        <location filename="../../src/app/tray.cpp" line="81"/>
        <source>%n more minutes</source>
        <translation>
            <numerusform>再加 %n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/tray.cpp" line="82"/>
        <source>%n more hour</source>
        <translation>
            <numerusform>再加 %n 小时</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="83"/>
        <source>Enable Break</source>
        <translation>启用休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="90"/>
        <source>Preferences</source>
        <translation>偏好设置</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="95"/>
        <source>Quit</source>
        <translation>退出</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="127"/>
        <location filename="../../src/app/tray.cpp" line="150"/>
        <source>big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="128"/>
        <location filename="../../src/app/tray.cpp" line="150"/>
        <source>small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="137"/>
        <source>Exit meeting (%1)</source>
        <translation>退出会议（%1）</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="101"/>
        <source>Next break after %1</source>
        <translation>%1 后休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="49"/>
        <source>Take a small break instead</source>
        <translation>改为进行小休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="59"/>
        <source>Meeting Mode</source>
        <translation>会议模式</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="62"/>
        <source>Focus Mode</source>
        <translation>专注模式</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="64"/>
        <source>End Focus &amp;&amp; Break</source>
        <translation>退出专注模式并休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="72"/>
        <source>Extend Meeting</source>
        <translation>延长会议</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="92"/>
        <source>Statistics</source>
        <translation>统计</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="103"/>
        <source>Next big break after %1</source>
        <translation>%1 后大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="125"/>
        <source>focus: %1 %2 (%3/%4)</source>
        <translation>专注：%1 %2 (%3/%4)</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="138"/>
        <source>Meeting mode — until %1 (%2 left)</source>
        <translation>会议模式持续到 %1 (剩余 %2)</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="142"/>
        <source>Paused on battery</source>
        <translation>因电池供电而暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="144"/>
        <source>Paused on app running</source>
        <translation>因特定应用运行而暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="146"/>
        <source>Paused on idle</source>
        <translation>因用户空闲而暂停</translation>
    </message>
</context>
<context>
    <name>WelcomeWindow</name>
    <message>
        <location filename="../../src/app/welcome.cpp" line="116"/>
        <source>Welcome to Sane Break</source>
        <translation>欢迎使用 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="117"/>
        <source>&lt;h3 align=center&gt;Welcome to Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow. Sane Break will stay in the system tray and remind you to take breaks at regular intervals. To quit, go to &quot;Postpone&quot; in the tray menu.&lt;/p&gt;</source>
        <translation>&lt;h3 align=center&gt;欢迎使用 Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break 是一款跨平台的休息提醒器，旨在帮助你在不影响工作流程的情况下进行有意义的休息。Sane Break 会停留在系统托盘中，提醒你按时休息。若要退出，请进入托盘菜单中的 “推迟”。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="96"/>
        <location filename="../../src/app/welcome.cpp" line="126"/>
        <source>Read More</source>
        <translation>了解更多</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="98"/>
        <location filename="../../src/app/welcome.cpp" line="127"/>
        <source>Ignore</source>
        <translation>忽略</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="99"/>
        <location filename="../../src/app/welcome.cpp" line="128"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="102"/>
        <location filename="../../src/app/welcome.cpp" line="129"/>
        <source>OK</source>
        <translation>好的</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="124"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
</context>
</TS>
