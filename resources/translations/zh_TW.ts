<?xml version="1.0" encoding="utf-8"?><!DOCTYPE TS><TS version="2.1" language="zh_TW" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="53"/>
        <source>Failed to connect to Flatpak response</source>
        <extracomment>Error message when requesting autostart in Flatpak got no response</extracomment>
        <translation>無法獲取 Flatpak 回覆</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="76"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="82"/>
        <source>Autostart desktop entry not writable</source>
        <extracomment>Error message when failed to write Linux desktop entry for autostart</extracomment>
        <translation>自動啟動文件無法被寫入</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="100"/>
        <source>The request to autostart was cancelled</source>
        <extracomment>Error message when requesting autostart in Flatpak got cancelled by user</extracomment>
        <translation>自動啟動請求被拒絕</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="30"/>
        <source>Permission denied</source>
        <extracomment>Error message when requesting autostart got permission denied</extracomment>
        <translation>權限被拒絕</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="43"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="33"/>
        <source>Unknown error</source>
        <extracomment>Error message when requesting autostart got unknown error</extracomment>
        <translation>未知錯誤</translation>
    </message>
</context>
<context>
    <name>BreakWindow</name>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="133"/>
        <source>&lt;p&gt;Sane Break is in force break mode.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Sane Break 已進入強制休息模式。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="136"/>
        <source>&lt;p&gt;Quit Sane Break by running &lt;code&gt;killall sane-break&lt;/code&gt; in terminal.&lt;/p&gt;</source>
        <translation>&lt;p&gt;在終端中運行 &lt;code&gt;killall sane-break&lt;/code&gt; 以退出 Sane Break。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="140"/>
        <source>&lt;p&gt;Quit Sane Break by enabling Spotlight with &lt;code&gt;Cmd + Space&lt;/code&gt;, opening terminal, and running &lt;code&gt;killall sane-break&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;使用 &lt;code&gt;Cmd + Space&lt;/code&gt; 喚出聚焦搜索，打開終端，運行 &lt;code&gt;killall sane-break&lt;/code&gt; 以退出 Sane Break。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="144"/>
        <source>&lt;p&gt;Pospone Sane Break by pressing &lt;code&gt;Win&lt;/code&gt; and right click Sane Break icon in the system tray.&lt;/p&gt;</source>
        <translation>&lt;p&gt;按 &lt;code&gt;Win&lt;/code&gt; 鍵後右鍵系統托盤中的 Sane Break 圖標以推遲休息&lt;/p&gt;</translation>
    </message>
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="34"/>
        <location filename="../../src/gui/widgets/language-select.cpp" line="61"/>
        <source>Default</source>
        <translation>默認語言</translation>
    </message>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="46"/>
        <source>English</source>
        <comment>current language</comment>
        <extracomment>This is the name of the current language in the current language</extracomment>
        <translation>繁體中文</translation>
    </message>
</context>
<context>
    <name>PrefWindow</name>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="110"/>
        <source>Pause</source>
        <translation>暫停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="145"/>
        <source>General</source>
        <translation>通用</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="167"/>
        <source>About</source>
        <translation>關於</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1129"/>
        <location filename="../../src/gui/pref-window.ui" line="1167"/>
        <source>Bell before breaks</source>
        <translation>休息前提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1098"/>
        <location filename="../../src/gui/pref-window.ui" line="1195"/>
        <source>Bell after breaks</source>
        <translation>休息後提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="564"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <extracomment>Tooltip for "Flash break prompt for" config</extracomment>
        <translation>休息提醒彈出後允許繼續工作的時長（秒），超時則強制休息。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="75"/>
        <source>Schedule</source>
        <translation>休息計劃</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="94"/>
        <source>Reminder</source>
        <translation>提示窗口</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="126"/>
        <source>Sound</source>
        <translation>聲音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="350"/>
        <source>Length of work time between breaks</source>
        <extracomment>Tooltip for "Break every" config (small break)</extracomment>
        <translation>兩次休息之間的工作時間</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="353"/>
        <location filename="../../src/gui/pref-window.ui" line="387"/>
        <source>Break every</source>
        <translation>休息提示間隔</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="384"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for "Break every" config (big break)</extracomment>
        <translation>每這麼多次休息開始大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="469"/>
        <source>&lt;h3&gt;Postpone&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;推遲&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="567"/>
        <source>Flash break prompt for</source>
        <translation>窗口最長閃爍時間</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="574"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there's no way back.</source>
        <extracomment>Tooltip for "Confirm break after" config</extracomment>
        <translation>休息提示開始後，在給定時間內持續處於空閒狀態，則將自動進入強制休息狀態，不會因為用戶輸入而重新計時。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="577"/>
        <source>Confirm break after</source>
        <translation>確認休息時間</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="680"/>
        <source>Flash speed</source>
        <translation>閃爍速度</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="711"/>
        <source>Show tips to kill Sane Break when locked in full-screen mode</source>
        <translation>在全屏模式顯示強殺 Sane Break 的提示</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="729"/>
        <location filename="../../src/gui/pref-window.ui" line="756"/>
        <source>(One line per message)</source>
        <translation>（每行一條提示文本）</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="736"/>
        <source>Big break prompt messages:</source>
        <translation>大休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="743"/>
        <source>Small break prompt messages:</source>
        <translation>小休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="765"/>
        <source>&lt;h3&gt;Color Theme&lt;/h3&gt;
All color are in &lt;code&gt;#AARRGGBB&lt;/code&gt; format.</source>
        <translation>&lt;h3&gt;顏色主題&lt;/h3&gt;
所有顏色的格式均為 &lt;code&gt;#AARRGGBB&lt;/code&gt;。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="778"/>
        <source>Big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="785"/>
        <source>Message</source>
        <translation>提示文本</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="802"/>
        <source>Small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="809"/>
        <source>Background</source>
        <translation>背景</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="846"/>
        <source>Count down</source>
        <translation>倒計時</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="872"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;暫停條件&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1019"/>
        <source>Long pause threshold</source>
        <translation>長時間暫停閾值</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1070"/>
        <source>Short pause threshold</source>
        <translation>短時間暫停閾值</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1287"/>
        <source>Let's say you've configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <extracomment>Tooltip for "Auto screen lock" config</extracomment>
        <translation>假設設置為 2 分鐘，則如果休息持續時間短於 2 分鐘，只要休息
結束後用戶沒有活動，屏幕會在休息提示開始後的 2 分鐘自動鎖
定。如果休息持續超過 2 分鐘，則屏幕必然會自動鎖定。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1292"/>
        <source>Automatic screen lock after break start</source>
        <translation>休息開始一段時間後自動鎖屏</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1312"/>
        <source>Please grant accessibility permission and restart app to use auto lock screen</source>
        <translation>請授予輔助功能權限並重啟應用以使用自動鎖屏功能</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1265"/>
        <source>&lt;a href="https://hosted.weblate.org/engage/sane-break/"&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href="https://hosted.weblate.org/engage/sane-break/"&gt;幫我們翻譯！&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1445"/>
        <source>&lt;div&gt;
&lt;span style="font-size: 26px; font-weight: bold"&gt;Sane Break&lt;/span&gt;
v%1
&lt;/div&gt;
&lt;p&gt;Remind breaks without mindless skips. &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href="https://github.com/AllanChain/sane-break/"&gt;Sane Break developers&lt;/a&gt; (&lt;a href="https://www.gnu.org/licenses/gpl-3.0.en.html"&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</source>
        <translation>&lt;div&gt;
&lt;span style="font-size: 26px; font-weight: bold"&gt;Sane Break&lt;/span&gt;
v%1
&lt;/div&gt;
&lt;p&gt;拒絕無意識跳過的休息提示器 &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href="https://github.com/AllanChain/sane-break/"&gt;Sane Break developers&lt;/a&gt; (&lt;a href="https://www.gnu.org/licenses/gpl-3.0.en.html"&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="299"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for "Break length" config (small break)</extracomment>
        <translation>小休息持續多少秒</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="457"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for "Break length" config (big break)</extracomment>
        <translation>大休息持續多少秒</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="494"/>
        <source>Postpone minutes</source>
        <translation>推遲分鐘數</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="627"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;提示小窗外觀&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="644"/>
        <source>Off</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>無</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="651"/>
        <source>Slower</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>更慢</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="658"/>
        <source>Default</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>默認</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="668"/>
        <source>Faster</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>更快</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1095"/>
        <location filename="../../src/gui/pref-window.ui" line="1126"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>如果沒有聲音，請嘗試重啟 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1112"/>
        <location filename="../../src/gui/pref-window.ui" line="1119"/>
        <location filename="../../src/gui/pref-window.ui" line="1174"/>
        <location filename="../../src/gui/pref-window.ui" line="1181"/>
        <source>Play</source>
        <translation>播放</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="881"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for "Pause if idle for" config</extracomment>
        <translation>在這麼多時間內沒有活動，Sane Break 將暫停計時</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="884"/>
        <source>Pause if idle for</source>
        <translation>空閒一段時間後暫停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="252"/>
        <location filename="../../src/gui/pref-window.ui" line="1086"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;小休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="309"/>
        <location filename="../../src/gui/pref-window.ui" line="394"/>
        <location filename="../../src/gui/pref-window.ui" line="925"/>
        <location filename="../../src/gui/pref-window.ui" line="1006"/>
        <location filename="../../src/gui/pref-window.ui" line="1057"/>
        <source> min</source>
        <translation> 分鐘</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="261"/>
        <location filename="../../src/gui/pref-window.ui" line="526"/>
        <location filename="../../src/gui/pref-window.ui" line="612"/>
        <source> sec</source>
        <translation> 秒鐘</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="302"/>
        <location filename="../../src/gui/pref-window.ui" line="460"/>
        <source>Break length</source>
        <translation>休息時長</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1207"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;提示&lt;/strong&gt;：你可以這樣使用本地文件 &lt;code&gt;file:/D:\sound.mp3&lt;/code&gt;  (Windows) 或 &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux)。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="362"/>
        <location filename="../../src/gui/pref-window.ui" line="1145"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;大休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="371"/>
        <source> breaks</source>
        <translation> 次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="510"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;%1&lt;/b&gt; seconds. If you stop your activity during that time, or choose to continue until the timer runs out, it transitions to full-screen. During the break, Sane Break locks into full-screen mode after &lt;b&gt;%2&lt;/b&gt; seconds of inactivity. If you resume activity before the lock, the prompt shrinks to a small window temporarily but re-expands to full-screen when you're idle again or the timer ends.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;提醒行為&lt;/h3&gt;
&lt;p&gt;Sane Break 會顯示一個持續 &lt;b&gt;%1&lt;/b&gt; 秒的小型提示窗。若您在此期間停止活動，或選擇繼續超過這個時間，界面將轉換至全屏模式。休息期間，若持續 &lt;b&gt;%2&lt;/b&gt; 秒無操作，Sane Break 會鎖定全屏狀態。若在鎖定前恢復活動，提示窗會暫時縮成小窗口，但當再次無操作或到時間後，界面將重新轉換至全屏。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="940"/>
        <source>Pause on battery</source>
        <translation>電池供電時暫停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="947"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>當以下程序運行時暫停休息提示：</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="954"/>
        <source>(Write one program name per line)</source>
        <translation>（每一行寫一個程序名）</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="961"/>
        <source>&lt;h3&gt;Reset Break Schedule After Pause&lt;/h3&gt;
&lt;p&gt;Sane Break will adjust the break schedule after pausing:&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;If the pause is short, Sane Break will reset the time to the next break.&lt;/li&gt;
  &lt;li&gt;If it is longer, Sane Break will reset the entire break cycle, and the next break will be the first small break in the cycle.&lt;/li&gt;
&lt;/ul&gt;</source>
        <translation>&lt;h3&gt;暫停後重置計時&lt;/h3&gt;
&lt;p&gt;Sane Break 會在暫停後調整計時：&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;短時間暫停後，Sane Break 會重置到下一次休息的時間&lt;/li&gt;
  &lt;li&gt;長時間暫停後，Sane Break 會重置整個休息循環，下一次休息是循環中的第一個小休息&lt;/li&gt;
&lt;/ul&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1233"/>
        <source>Configuration file</source>
        <translation>配置文件</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1327"/>
        <source>Launch Sane Break at start up</source>
        <translation>開機自動啟動 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1526"/>
        <source>Revert to Last Saved</source>
        <translation>恢復上次設置</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1226"/>
        <source>Language</source>
        <translation>語言</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1252"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>改變語言將在重啟應用後生效。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1549"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
</context>
<context>
    <name>PreferenceWindow</name>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="299"/>
        <location filename="../../src/gui/pref-window.cpp" line="300"/>
        <location filename="../../src/gui/pref-window.cpp" line="301"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="142"/>
        <location filename="../../src/gui/pref-window.cpp" line="151"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>每 %n 分钟</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="298"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n 秒</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="297"/>
        <source>Disabled</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="308"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>在中鍵托盤圖標後，立刻開始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="314"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>在雙擊托盤圖標後，立刻開始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="382"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="383"/>
        <source>Discard</source>
        <translation>放棄</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="384"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="279"/>
        <source>Setting autostart failed</source>
        <translation>設置開機自啟失敗</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="379"/>
        <source>The preferences have been modified.</source>
        <translation>偏好設置已被修改。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="380"/>
        <source>Do you want to save your changes?</source>
        <translation>是否保存修改？</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="86"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>托盤不可用，將使用普通窗口</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="92"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>窗口合成器不支持 layer shell 協議，窗口位置會有問題</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="96"/>
        <source>Sane Break is not compiled with layer shell support, and window positioning will be bugged</source>
        <translation>Sane Break 在編譯時為加入 layer shell 支持，窗口位置會有問題</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="100"/>
        <source>You need to install the shell extension if you are using GNOME. Otherwise, Sane break is almost unusable</source>
        <translation>如果你在使用 GNOME，那麼需要安裝對應的 shell 擴展，否則 Sane Break 不能正常使用</translation>
    </message>
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <location filename="../../src/app/app.cpp" line="91"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation>確定退出 Sane Break？</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="92"/>
        <source>You can postpone the breaks instead.</source>
        <translation>你可以推遲休息而非退出。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="94"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="95"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>推迟 %n 分钟</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="96"/>
        <source>Yes</source>
        <translation>是的</translation>
    </message>
</context>
<context>
    <name>SanePreferences</name>
    <message>
        <location filename="../../src/core/preferences.cpp" line="50"/>
        <source>Time for a small break</source>
        <translation>小休息時間</translation>
    </message>
    <message>
        <location filename="../../src/core/preferences.cpp" line="52"/>
        <source>Time for a big break</source>
        <translation>大休息時間</translation>
    </message>
</context>
<context>
    <name>StatusTrayWindow</name>
    <message>
        <location filename="../../src/gui/tray.cpp" line="52"/>
        <source>Postpone</source>
        <translation>推遲</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/tray.cpp" line="55"/>
        <location filename="../../src/gui/tray.cpp" line="141"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分钟</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="58"/>
        <source>Enable Break</source>
        <translation>啟用休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="65"/>
        <source>Preferences</source>
        <translation>偏好設置</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="68"/>
        <source>Quit</source>
        <translation>退出</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="94"/>
        <source>big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="94"/>
        <source>small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="74"/>
        <source>Next break after %1</source>
        <translation>%1 後休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="45"/>
        <source>Take a small break instead</source>
        <translation>改為進行小休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="76"/>
        <source>Next big break after %1</source>
        <translation>%1 後大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="86"/>
        <source>Paused on battery</source>
        <translation>因電池供電而暫停</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="88"/>
        <source>Paused on app running</source>
        <translation>因特定應用運行而暫停</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="90"/>
        <source>Paused on idle</source>
        <translation>因用戶空閒而暫停</translation>
    </message>
</context>
<context>
    <name>WelcomeWindow</name>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="116"/>
        <source>Welcome to Sane Break</source>
        <translation>歡迎使用 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="117"/>
        <source>&lt;h3 align=center&gt;Welcome to Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow. Sane Break will stay in the system tray and remind you to take breaks at regular intervals. To quit, go to "Postpone" in the tray menu.&lt;/p&gt;</source>
        <translation>&lt;h3 align=center&gt;歡迎使用 Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break 是一款跨平臺的休息提醒器，旨在幫助你在不影響工作流程的情況下進行有意義的休息。Sane Break 會停留在系統托盤中，提醒你按時休息。若要退出，請進入托盤菜單中的 “推遲”。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="96"/>
        <location filename="../../src/gui/welcome.cpp" line="126"/>
        <source>Read More</source>
        <translation>瞭解更多</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="98"/>
        <location filename="../../src/gui/welcome.cpp" line="127"/>
        <source>Ignore</source>
        <translation>忽略</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="99"/>
        <location filename="../../src/gui/welcome.cpp" line="128"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="102"/>
        <location filename="../../src/gui/welcome.cpp" line="129"/>
        <source>OK</source>
        <translation>好的</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="124"/>
        <source>Language</source>
        <translation>語言</translation>
    </message>
</context>
</TS>