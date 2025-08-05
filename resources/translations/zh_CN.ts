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
        <location filename="../../src/lib/linux/auto-start.cpp" line="74"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="84"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="90"/>
        <source>Autostart desktop entry not writable</source>
        <extracomment>Error message when failed to write Linux desktop entry for autostart</extracomment>
        <translation>自动启动文件无法被写入</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="108"/>
        <source>The request to autostart was cancelled</source>
        <extracomment>Error message when requesting autostart in Flatpak got cancelled by user</extracomment>
        <translation>自动启动请求被拒绝</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="30"/>
        <source>Permission denied</source>
        <extracomment>Error message when requesting autostart got permission denied</extracomment>
        <translation>权限被拒绝</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="44"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="33"/>
        <source>Unknown error</source>
        <extracomment>Error message when requesting autostart got unknown error</extracomment>
        <translation>未知错误</translation>
    </message>
</context>
<context>
    <name>BreakReminder</name>
    <message>
        <location filename="../../src/app/break-window.ui" line="246"/>
        <source>Lock Screen</source>
        <translation>锁屏</translation>
    </message>
    <message>
        <location filename="../../src/app/break-window.ui" line="300"/>
        <source>Exit Force Break</source>
        <translation>退出强制休息</translation>
    </message>
</context>
<context>
    <name>BreakWindow</name>
    <message>
        <location filename="../../src/app/break-window.cpp" line="121"/>
        <source>Break will end at: %1</source>
        <translation>休息将在 %1 结束</translation>
    </message>
    <message>
        <location filename="../../src/app/break-window.cpp" line="123"/>
        <source>Break has ended</source>
        <translation>休息已结束</translation>
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
        <location filename="../../src/app/pref-window.ui" line="1304"/>
        <location filename="../../src/app/pref-window.ui" line="1342"/>
        <source>Bell before breaks</source>
        <translation>休息前提示音</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1273"/>
        <location filename="../../src/app/pref-window.ui" line="1370"/>
        <source>Bell after breaks</source>
        <translation>休息后提示音</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="592"/>
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
        <location filename="../../src/app/pref-window.ui" line="372"/>
        <source>Length of work time between breaks</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (small break)</extracomment>
        <translation>两次休息之间的工作时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="375"/>
        <location filename="../../src/app/pref-window.ui" line="412"/>
        <source>Break every</source>
        <translation>休息提示间隔</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="409"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (big break)</extracomment>
        <translation>每这么多次休息开始大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="494"/>
        <source>&lt;h3&gt;Postpone&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;推迟&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="595"/>
        <source>Flash break prompt for</source>
        <translation>窗口最长闪烁时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="602"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <extracomment>Tooltip for &quot;Confirm break after&quot; config</extracomment>
        <translation>休息提示开始后，在给定时间内持续处于空闲状态，则将自动进入强制休息状态，不会因为用户输入而重新计时。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="605"/>
        <source>Confirm break after</source>
        <translation>确认休息时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="676"/>
        <source>Close windows right after small break ends</source>
        <translation>小休息结束后立即关闭窗口</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="683"/>
        <source>Close windows right after big break ends</source>
        <translation>大休息结束后立即关闭窗口</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="709"/>
        <source>Flash speed</source>
        <translation>闪烁速度</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="764"/>
        <location filename="../../src/app/pref-window.ui" line="797"/>
        <source>(One line per message)</source>
        <translation>（每行一条提示文本）</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="771"/>
        <source>Big break prompt messages:</source>
        <translation>大休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="781"/>
        <source>Small break prompt messages:</source>
        <translation>小休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="660"/>
        <source>Max number of force break exits</source>
        <translation>退出强制休息次数上限</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="110"/>
        <source>Interface</source>
        <translation>界面</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="667"/>
        <source> times</source>
        <translation> 次</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="806"/>
        <source>&lt;h3&gt;Components to Show&lt;/h3&gt;

&lt;p&gt;User interface components to show in the prompt and full-screen window.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;显示组件&lt;/h3&gt;

&lt;p&gt;在提示小窗和全屏窗口中显示的用户界面组件。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="831"/>
        <location filename="../../src/app/pref-window.ui" line="877"/>
        <source>Time when break ends</source>
        <translation>休息结束时间</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="852"/>
        <location filename="../../src/app/pref-window.ui" line="898"/>
        <source>Countdown timer</source>
        <translation>倒计时</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="824"/>
        <location filename="../../src/app/pref-window.ui" line="870"/>
        <source>Progress bar</source>
        <translation>进度条</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="838"/>
        <location filename="../../src/app/pref-window.ui" line="884"/>
        <source>Clock</source>
        <translation>时钟</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="845"/>
        <location filename="../../src/app/pref-window.ui" line="891"/>
        <source>Buttons</source>
        <translation>按钮</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="907"/>
        <source>&lt;h3&gt;Color Theme&lt;/h3&gt;
All color are in &lt;code&gt;#AARRGGBB&lt;/code&gt; format.</source>
        <translation>&lt;h3&gt;颜色主题&lt;/h3&gt;
所有颜色的格式均为 &lt;code&gt;#AARRGGBB&lt;/code&gt;。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="991"/>
        <source>Big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="960"/>
        <source>Message</source>
        <translation>提示文本</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="998"/>
        <source>Small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="967"/>
        <source>Background</source>
        <translation>背景</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="974"/>
        <source>Count down</source>
        <translation>倒计时</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1038"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;暂停条件&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1191"/>
        <source>Long pause threshold</source>
        <translation>长时间暂停阈值</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1245"/>
        <source>Short pause threshold</source>
        <translation>短时间暂停阈值</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1457"/>
        <source>Let&apos;s say you&apos;ve configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <extracomment>Tooltip for &quot;Auto screen lock&quot; config</extracomment>
        <translation>假设设置为 2 分钟，则如果休息持续时间短于 2 分钟，只要休息
结束后用户没有活动，屏幕会在休息提示开始后的 2 分钟自动锁
定。如果休息持续超过 2 分钟，则屏幕必然会自动锁定。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1462"/>
        <source>Automatic screen lock after break start</source>
        <translation>休息开始一段时间后自动锁屏</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1440"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;帮我们翻译！&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1579"/>
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
        <location filename="../../src/app/pref-window.ui" line="318"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (small break)</extracomment>
        <translation>小休息持续多少秒</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="482"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (big break)</extracomment>
        <translation>大休息持续多少秒</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="519"/>
        <source>Postpone minutes</source>
        <translation>推迟分钟数</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="694"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;提示小窗外观&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1270"/>
        <location filename="../../src/app/pref-window.ui" line="1301"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>如果没有声音，请尝试重启 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1287"/>
        <location filename="../../src/app/pref-window.ui" line="1294"/>
        <location filename="../../src/app/pref-window.ui" line="1349"/>
        <location filename="../../src/app/pref-window.ui" line="1356"/>
        <source>Play</source>
        <translation>播放</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1047"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for &quot;Pause if idle for&quot; config</extracomment>
        <translation>在这么多时间内没有活动，Sane Break 将暂停计时</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1050"/>
        <source>Pause if idle for</source>
        <translation>空闲一段时间后暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="268"/>
        <location filename="../../src/app/pref-window.ui" line="1261"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;小休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="328"/>
        <location filename="../../src/app/pref-window.ui" line="419"/>
        <location filename="../../src/app/pref-window.ui" line="1091"/>
        <location filename="../../src/app/pref-window.ui" line="1175"/>
        <location filename="../../src/app/pref-window.ui" line="1229"/>
        <source> min</source>
        <translation> 分钟</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="277"/>
        <location filename="../../src/app/pref-window.ui" line="551"/>
        <location filename="../../src/app/pref-window.ui" line="640"/>
        <source> sec</source>
        <translation> 秒钟</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="321"/>
        <location filename="../../src/app/pref-window.ui" line="485"/>
        <source>Break length</source>
        <translation>休息时长</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1382"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;提示&lt;/strong&gt;：你可以这样使用本地文件 &lt;code&gt;file:/D:\sound.mp3&lt;/code&gt;  (Windows) 或 &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux)。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="384"/>
        <location filename="../../src/app/pref-window.ui" line="1320"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;大休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="393"/>
        <source> breaks</source>
        <translation> 次休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="535"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;%1&lt;/b&gt; seconds. If you stop your activity during that time, or choose to continue until the timer runs out, it transitions to full-screen. During the break, Sane Break locks into full-screen mode after &lt;b&gt;%2&lt;/b&gt; seconds of inactivity. If you resume activity before the lock, the prompt shrinks to a small window temporarily but re-expands to full-screen when you&apos;re idle again or the timer ends.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;提醒行为&lt;/h3&gt;
&lt;p&gt;Sane Break 会显示一个持续 &lt;b&gt;%1&lt;/b&gt; 秒的小型提示窗。若您在此期间停止活动，或选择继续超过这个时间，界面将转换至全屏模式。休息期间，若持续 &lt;b&gt;%2&lt;/b&gt; 秒无操作，Sane Break 会锁定全屏状态。若在锁定前恢复活动，提示窗会暂时缩成小窗口，但当再次无操作或到时间后，界面将重新转换至全屏。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="815"/>
        <source>&lt;h4&gt;Small break&lt;/h4&gt;</source>
        <translation>&lt;h4&gt;小休息&lt;/h4&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="861"/>
        <source>&lt;h4&gt;Big break&lt;/h4&gt;</source>
        <translation>&lt;h4&gt;大休息&lt;/h4&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1014"/>
        <source>&lt;p&gt;To preview break windows, save all changes first. The preview begins with a 2-second flashing prompt, followed by a 4-second full-screen break. A small break window will appear first, then a big break window.&lt;/p&gt;</source>
        <translation>&lt;p&gt;预览休息窗口前，请先保存所有设置。预览时，会先显示2秒提示小窗，随后进入4秒全屏窗口。先显示小休息窗口，再显示大休息窗口。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1024"/>
        <source>Preview Break Windows</source>
        <translation>预览休息窗口</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1109"/>
        <source>Pause on battery</source>
        <translation>电池供电时暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1116"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>当以下程序运行时暂停休息提示：</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1123"/>
        <source>(Write one program name per line)</source>
        <translation>（每一行写一个程序名）</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1130"/>
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
        <location filename="../../src/app/pref-window.ui" line="1408"/>
        <source>Configuration file</source>
        <translation>配置文件</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1477"/>
        <source>Launch Sane Break at start up</source>
        <translation>开机自动启动 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1603"/>
        <source>Get the Source Code</source>
        <translation>获取源代码</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1658"/>
        <source>Revert to Last Saved</source>
        <translation>恢复上次设置</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1401"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1427"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>改变语言将在重启应用后生效。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1623"/>
        <source>Third-party Libraries Licenses</source>
        <translation>第三方库许可</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1665"/>
        <source>Defaults</source>
        <translation>恢复默认</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1688"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
</context>
<context>
    <name>PreferenceWindow</name>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="399"/>
        <location filename="../../src/app/pref-window.cpp" line="400"/>
        <location filename="../../src/app/pref-window.cpp" line="401"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="165"/>
        <location filename="../../src/app/pref-window.cpp" line="176"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>每 %n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="398"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n 秒</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="397"/>
        <source>Disabled</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="408"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>在中键托盘图标后，立刻开始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="413"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>在双击托盘图标后，立刻开始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="481"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="482"/>
        <source>Discard</source>
        <translation>放弃</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="483"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="373"/>
        <source>Setting autostart failed</source>
        <translation>设置开机自启失败</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="478"/>
        <source>The preferences have been modified.</source>
        <translation>偏好设置已被修改。</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="479"/>
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
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <location filename="../../src/app/app.cpp" line="106"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation>确定退出 Sane Break？</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="107"/>
        <source>You can postpone the breaks instead.</source>
        <translation>你可以推迟休息而非退出。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="85"/>
        <location filename="../../src/app/app.cpp" line="109"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="81"/>
        <source>Are you sure to postpone for %n minute?</source>
        <translation>
            <numerusform>确定推迟 %n 分钟吗？</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="82"/>
        <source>You haven&apos;t taken breaks for %1 minutes.</source>
        <translation>您已经 %1 分钟没有休息了。</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="110"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>推迟 %n 分钟</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="86"/>
        <location filename="../../src/app/app.cpp" line="111"/>
        <source>Yes</source>
        <translation>是的</translation>
    </message>
</context>
<context>
    <name>SanePreferences</name>
    <message>
        <location filename="../../src/core/preferences.cpp" line="49"/>
        <source>Time for a small break</source>
        <translation>小休息时间</translation>
    </message>
    <message>
        <location filename="../../src/core/preferences.cpp" line="52"/>
        <source>Time for a big break</source>
        <translation>大休息时间</translation>
    </message>
</context>
<context>
    <name>StatusTrayWindow</name>
    <message>
        <location filename="../../src/app/tray.cpp" line="52"/>
        <source>Postpone</source>
        <translation>推迟</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/tray.cpp" line="55"/>
        <location filename="../../src/app/tray.cpp" line="141"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分钟</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="58"/>
        <source>Enable Break</source>
        <translation>启用休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="65"/>
        <source>Preferences</source>
        <translation>偏好设置</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="68"/>
        <source>Quit</source>
        <translation>退出</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="94"/>
        <source>big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="94"/>
        <source>small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="74"/>
        <source>Next break after %1</source>
        <translation>%1 后休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="45"/>
        <source>Take a small break instead</source>
        <translation>改为进行小休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="76"/>
        <source>Next big break after %1</source>
        <translation>%1 后大休息</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="86"/>
        <source>Paused on battery</source>
        <translation>因电池供电而暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="88"/>
        <source>Paused on app running</source>
        <translation>因特定应用运行而暂停</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="90"/>
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
