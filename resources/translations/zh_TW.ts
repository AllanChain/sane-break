<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_TW" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="54"/>
        <source>Failed to connect to Flatpak response</source>
        <extracomment>Error message when requesting autostart in Flatpak got no response</extracomment>
        <translation>無法獲取 Flatpak 回覆</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="74"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="84"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="90"/>
        <source>Autostart desktop entry not writable</source>
        <extracomment>Error message when failed to write Linux desktop entry for autostart</extracomment>
        <translation>自動啟動文件無法被寫入</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="108"/>
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
        <location filename="../../src/lib/linux/auto-start.cpp" line="44"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="33"/>
        <source>Unknown error</source>
        <extracomment>Error message when requesting autostart got unknown error</extracomment>
        <translation>未知錯誤</translation>
    </message>
</context>
<context>
    <name>BreakReminder</name>
    <message>
        <location filename="../../src/gui/break-window.ui" line="167"/>
        <source>Lock Screen</source>
        <translation>鎖屏</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.ui" line="221"/>
        <source>Exit Force Break</source>
        <translation>退出強制休息</translation>
    </message>
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="67"/>
        <location filename="../../src/gui/widgets/language-select.cpp" line="94"/>
        <source>Default</source>
        <translation>默認語言</translation>
    </message>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="79"/>
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
        <location filename="../../src/gui/pref-window.ui" line="1152"/>
        <location filename="../../src/gui/pref-window.ui" line="1190"/>
        <source>Bell before breaks</source>
        <translation>休息前提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1121"/>
        <location filename="../../src/gui/pref-window.ui" line="1218"/>
        <source>Bell after breaks</source>
        <translation>休息後提示音</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="576"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <extracomment>Tooltip for &quot;Flash break prompt for&quot; config</extracomment>
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
        <location filename="../../src/gui/pref-window.ui" line="356"/>
        <source>Length of work time between breaks</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (small break)</extracomment>
        <translation>兩次休息之間的工作時間</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="359"/>
        <location filename="../../src/gui/pref-window.ui" line="396"/>
        <source>Break every</source>
        <translation>休息提示間隔</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="393"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (big break)</extracomment>
        <translation>每這麼多次休息開始大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="478"/>
        <source>&lt;h3&gt;Postpone&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;推遲&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="579"/>
        <source>Flash break prompt for</source>
        <translation>窗口最長閃爍時間</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="586"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <extracomment>Tooltip for &quot;Confirm break after&quot; config</extracomment>
        <translation>休息提示開始後，在給定時間內持續處於空閒狀態，則將自動進入強制休息狀態，不會因為用戶輸入而重新計時。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="589"/>
        <source>Confirm break after</source>
        <translation>確認休息時間</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="695"/>
        <source>Flash speed</source>
        <translation>閃爍速度</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="737"/>
        <location filename="../../src/gui/pref-window.ui" line="770"/>
        <source>(One line per message)</source>
        <translation>（每行一條提示文本）</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="744"/>
        <source>Big break prompt messages:</source>
        <translation>大休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="754"/>
        <source>Small break prompt messages:</source>
        <translation>小休息提示文本：</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="779"/>
        <source>&lt;h3&gt;Color Theme&lt;/h3&gt;
All color are in &lt;code&gt;#AARRGGBB&lt;/code&gt; format.</source>
        <translation>&lt;h3&gt;顏色主題&lt;/h3&gt;
所有顏色的格式均為 &lt;code&gt;#AARRGGBB&lt;/code&gt;。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="792"/>
        <source>Big break</source>
        <translation>大休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="799"/>
        <source>Message</source>
        <translation>提示文本</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="816"/>
        <source>Small break</source>
        <translation>小休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="823"/>
        <source>Background</source>
        <translation>背景</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="860"/>
        <source>Count down</source>
        <translation>倒計時</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="886"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;暫停條件&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1039"/>
        <source>Long pause threshold</source>
        <translation>長時間暫停閾值</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1093"/>
        <source>Short pause threshold</source>
        <translation>短時間暫停閾值</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1310"/>
        <source>Let&apos;s say you&apos;ve configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <extracomment>Tooltip for &quot;Auto screen lock&quot; config</extracomment>
        <translation>假設設置為 2 分鐘，則如果休息持續時間短於 2 分鐘，只要休息
結束後用戶沒有活動，屏幕會在休息提示開始後的 2 分鐘自動鎖
定。如果休息持續超過 2 分鐘，則屏幕必然會自動鎖定。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1315"/>
        <source>Automatic screen lock after break start</source>
        <translation>休息開始一段時間後自動鎖屏</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1338"/>
        <source>Please grant accessibility permission and restart app to use auto lock screen</source>
        <translation>請授予輔助功能權限並重啟應用以使用自動鎖屏功能</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1288"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;幫我們翻譯！&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1455"/>
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
&lt;p&gt;拒絕無意識跳過的休息提示器 &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break developers&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="302"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (small break)</extracomment>
        <translation>小休息持續多少秒</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="466"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (big break)</extracomment>
        <translation>大休息持續多少秒</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="503"/>
        <source>Postpone minutes</source>
        <translation>推遲分鐘數</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="642"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;提示小窗外觀&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="659"/>
        <source>Off</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>無</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="666"/>
        <source>Slower</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>更慢</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="673"/>
        <source>Default</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>默認</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="683"/>
        <source>Faster</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>更快</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1118"/>
        <location filename="../../src/gui/pref-window.ui" line="1149"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>如果沒有聲音，請嘗試重啟 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1135"/>
        <location filename="../../src/gui/pref-window.ui" line="1142"/>
        <location filename="../../src/gui/pref-window.ui" line="1197"/>
        <location filename="../../src/gui/pref-window.ui" line="1204"/>
        <source>Play</source>
        <translation>播放</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="895"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for &quot;Pause if idle for&quot; config</extracomment>
        <translation>在這麼多時間內沒有活動，Sane Break 將暫停計時</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="898"/>
        <source>Pause if idle for</source>
        <translation>空閒一段時間後暫停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="252"/>
        <location filename="../../src/gui/pref-window.ui" line="1109"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;小休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="312"/>
        <location filename="../../src/gui/pref-window.ui" line="403"/>
        <location filename="../../src/gui/pref-window.ui" line="939"/>
        <location filename="../../src/gui/pref-window.ui" line="1023"/>
        <location filename="../../src/gui/pref-window.ui" line="1077"/>
        <source> min</source>
        <translation> 分鐘</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="261"/>
        <location filename="../../src/gui/pref-window.ui" line="535"/>
        <location filename="../../src/gui/pref-window.ui" line="624"/>
        <source> sec</source>
        <translation> 秒鐘</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="305"/>
        <location filename="../../src/gui/pref-window.ui" line="469"/>
        <source>Break length</source>
        <translation>休息時長</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1230"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;提示&lt;/strong&gt;：你可以這樣使用本地文件 &lt;code&gt;file:/D:\sound.mp3&lt;/code&gt;  (Windows) 或 &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux)。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="368"/>
        <location filename="../../src/gui/pref-window.ui" line="1168"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;大休息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="377"/>
        <source> breaks</source>
        <translation> 次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="519"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;%1&lt;/b&gt; seconds. If you stop your activity during that time, or choose to continue until the timer runs out, it transitions to full-screen. During the break, Sane Break locks into full-screen mode after &lt;b&gt;%2&lt;/b&gt; seconds of inactivity. If you resume activity before the lock, the prompt shrinks to a small window temporarily but re-expands to full-screen when you&apos;re idle again or the timer ends.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;提醒行為&lt;/h3&gt;
&lt;p&gt;Sane Break 會顯示一個持續 &lt;b&gt;%1&lt;/b&gt; 秒的小型提示窗。若您在此期間停止活動，或選擇繼續超過這個時間，界面將轉換至全屏模式。休息期間，若持續 &lt;b&gt;%2&lt;/b&gt; 秒無操作，Sane Break 會鎖定全屏狀態。若在鎖定前恢復活動，提示窗會暫時縮成小窗口，但當再次無操作或到時間後，界面將重新轉換至全屏。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="957"/>
        <source>Pause on battery</source>
        <translation>電池供電時暫停</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="964"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>當以下程序運行時暫停休息提示：</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="971"/>
        <source>(Write one program name per line)</source>
        <translation>（每一行寫一個程序名）</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="978"/>
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
        <location filename="../../src/gui/pref-window.ui" line="1256"/>
        <source>Configuration file</source>
        <translation>配置文件</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1353"/>
        <source>Launch Sane Break at start up</source>
        <translation>開機自動啟動 Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1479"/>
        <source>Get the Source Code</source>
        <translation>獲取源代碼</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1534"/>
        <source>Revert to Last Saved</source>
        <translation>恢復上次設置</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1249"/>
        <source>Language</source>
        <translation>語言</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1275"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>改變語言將在重啟應用後生效。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1499"/>
        <source>Third-party Libraries Licenses</source>
        <translation>第三方庫許可</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1557"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
</context>
<context>
    <name>PreferenceWindow</name>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="310"/>
        <location filename="../../src/gui/pref-window.cpp" line="311"/>
        <location filename="../../src/gui/pref-window.cpp" line="312"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n 分鐘</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="143"/>
        <location filename="../../src/gui/pref-window.cpp" line="152"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>每 %n 分鐘</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="309"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n 秒</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="308"/>
        <source>Disabled</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="319"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>在中鍵托盤圖標後，立刻開始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="324"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>在雙擊托盤圖標後，立刻開始下一次休息</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="384"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="385"/>
        <source>Discard</source>
        <translation>放棄</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="386"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="283"/>
        <source>Setting autostart failed</source>
        <translation>設置開機自啟失敗</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="381"/>
        <source>The preferences have been modified.</source>
        <translation>偏好設置已被修改。</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="382"/>
        <source>Do you want to save your changes?</source>
        <translation>是否保存修改？</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="95"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>托盤不可用，將使用普通窗口</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="101"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>窗口合成器不支持 layer shell 協議，窗口位置會有問題</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="105"/>
        <source>Sane Break is not compiled with layer shell support, and window positioning will be bugged</source>
        <translation>Sane Break 在編譯時為加入 layer shell 支持，窗口位置會有問題</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="109"/>
        <source>You need to install the shell extension if you are using GNOME. Otherwise, Sane break is almost unusable</source>
        <translation>如果你在使用 GNOME，那麼需要安裝對應的 shell 擴展，否則 Sane Break 不能正常使用</translation>
    </message>
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <location filename="../../src/app/app.cpp" line="112"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation>確定退出 Sane Break？</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="113"/>
        <source>You can postpone the breaks instead.</source>
        <translation>你可以推遲休息而非退出。</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="91"/>
        <location filename="../../src/app/app.cpp" line="115"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="87"/>
        <source>Are you sure to postpone for %n minute?</source>
        <translation>
            <numerusform>確定推遲 %n 分鐘嗎？</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="89"/>
        <source>You haven&apos;t taken breaks for %1 minutes.</source>
        <translation>您已經 %1 分鐘沒有休息了。</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="116"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>推遲 %n 分鐘</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="92"/>
        <location filename="../../src/app/app.cpp" line="117"/>
        <source>Yes</source>
        <translation>是的</translation>
    </message>
</context>
<context>
    <name>SanePreferences</name>
    <message>
        <location filename="../../src/core/preferences.cpp" line="49"/>
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
            <numerusform>%n 分鐘</numerusform>
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
        <source>&lt;h3 align=center&gt;Welcome to Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow. Sane Break will stay in the system tray and remind you to take breaks at regular intervals. To quit, go to &quot;Postpone&quot; in the tray menu.&lt;/p&gt;</source>
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
