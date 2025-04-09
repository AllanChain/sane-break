<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="nl_NL" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="53"/>
        <source>Failed to connect to Flatpak response</source>
        <extracomment>Error message when requesting autostart in Flatpak got no response</extracomment>
        <translation>Er kan geen verbinding met Flatpak worden gemaakt</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="76"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="82"/>
        <source>Autostart desktop entry not writable</source>
        <extracomment>Error message when failed to write Linux desktop entry for autostart</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File not writable</source>
        <translation type="vanished">Het bestand is niet beschrijfbaar</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="100"/>
        <source>The request to autostart was cancelled</source>
        <extracomment>Error message when requesting autostart in Flatpak got cancelled by user</extracomment>
        <translation>Het verzoek tot automatisch opstarten is afgebroken.</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="30"/>
        <source>Permission denied</source>
        <extracomment>Error message when requesting autostart got permission denied</extracomment>
        <translation>Toegang geweigerd</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="43"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="33"/>
        <source>Unknown error</source>
        <extracomment>Error message when requesting autostart got unknown error</extracomment>
        <translation>Onbekende foutmelding</translation>
    </message>
</context>
<context>
    <name>BreakWindow</name>
    <message>
        <source>Time for a big break</source>
        <translation type="vanished">Tijd voor een lange pauze</translation>
    </message>
    <message>
        <source>Time for a small break</source>
        <translation type="vanished">Tijd voor een korte pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="133"/>
        <source>&lt;p&gt;Sane Break is in force break mode.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="136"/>
        <source>&lt;p&gt;Quit Sane Break by running &lt;code&gt;killall sane-break&lt;/code&gt; in terminal.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="140"/>
        <source>&lt;p&gt;Quit Sane Break by enabling Spotlight with &lt;code&gt;Cmd + Space&lt;/code&gt;, opening terminal, and running &lt;code&gt;killall sane-break&lt;/code&gt;.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="144"/>
        <source>&lt;p&gt;Quit Sane Break by pressing &lt;code&gt;Win + R&lt;/code&gt; and run &lt;code&gt;taskkill /IM sane-break.exe /F&lt;/code&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="34"/>
        <location filename="../../src/gui/widgets/language-select.cpp" line="61"/>
        <source>Default</source>
        <translation>Standaard</translation>
    </message>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="46"/>
        <source>English</source>
        <comment>current language</comment>
        <extracomment>This is the name of the current language in the current language</extracomment>
        <translation>Nederlands</translation>
    </message>
</context>
<context>
    <name>PrefWindow</name>
    <message>
        <source>Break</source>
        <translation type="vanished">Pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="110"/>
        <source>Pause</source>
        <translation>Opschorting</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="145"/>
        <source>General</source>
        <translation>Algemeen</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="167"/>
        <source>About</source>
        <translation>Over</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Flashing Reminder&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small reminder for &lt;b&gt;Flash break prompt for&lt;/b&gt; seconds. If you stop working or ignore it past this time, it transitions to full-screen. After &lt;b&gt;Confirm break after&lt;/b&gt; seconds of inactivity during the break, it locks until the timer ends.&lt;/p&gt;</source>
        <translation type="vanished">&lt;h3&gt;Flitsherinnering&lt;/h3&gt;
&lt;p&gt;Sane Break toont een kleine &lt;b&gt;flitsherinnering gedurende&lt;/b&gt; seconden. Als u stopt met werken of de herinnering bewust negeert, dan wordt de herinnering schermvullend getoond. Na het &lt;b&gt;bevestigen van de pauze na&lt;/b&gt; seconden of inactiviteit, wordt de boel vergrendeld totdat de tijdklok afloopt.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="564"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <extracomment>Tooltip for &quot;Flash break prompt for&quot; config</extracomment>
        <translation>De duur - in seconden - alvorens een pauzeherinnering wordt getoond.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="350"/>
        <source>Length of work time between breaks</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (small break)</extracomment>
        <translation>De werktijd tussen pauzes</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="75"/>
        <source>Schedule</source>
        <translation>Schema</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="94"/>
        <source>Reminder</source>
        <translation>Herinnering</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="126"/>
        <source>Sound</source>
        <translation>Geluid</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="353"/>
        <location filename="../../src/gui/pref-window.ui" line="387"/>
        <source>Break every</source>
        <translation>Pauzeren, iedere</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="384"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (big break)</extracomment>
        <translation>Ieder x-aantal pauzes zal een lange zijn</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="469"/>
        <source>&lt;h3&gt;Postpone&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Uitstellen&lt;/h3&gt;</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;Flash break prompt for&lt;/b&gt; seconds. If you stop working or ignore it past this time, it transitions to full-screen. After &lt;b&gt;Confirm break after&lt;/b&gt; seconds of inactivity during the break, it locks until the timer ends.&lt;/p&gt;</source>
        <translation type="vanished">&lt;h3&gt;Flitsherinnering&lt;/h3&gt;
&lt;p&gt;Sane Break toont een kleine &lt;b&gt;flitsherinnering gedurende&lt;/b&gt; seconden. Als u stopt met werken of de herinnering bewust negeert, dan wordt de herinnering schermvullend getoond. Na het &lt;b&gt;bevestigen van de pauze na&lt;/b&gt; seconden of inactiviteit, wordt de boel vergrendeld totdat de tijdklok afloopt.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="567"/>
        <source>Flash break prompt for</source>
        <translation>Pauzeherinnering tonen, gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="574"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <extracomment>Tooltip for &quot;Confirm break after&quot; config</extracomment>
        <translation>Na een bepaalde mate van inactiviteit tijdens een pauze, zal Sane Break een schermvullende melding tonen.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="577"/>
        <source>Confirm break after</source>
        <translation>Pauze automatisch bevestigen na</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Reminder Appearance&lt;/h3&gt;</source>
        <translation type="vanished">&lt;h3&gt;Vormgeving van herinnering&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="680"/>
        <source>Flash speed</source>
        <translation>Flitssnelheid</translation>
    </message>
    <message>
        <source>Text Transparency</source>
        <translation type="vanished">Tekstdoorzichtigheid</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="872"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Pauzevoorwaarden&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="961"/>
        <source>&lt;h3&gt;Reset Break Schedule After Pause&lt;/h3&gt;
&lt;p&gt;Sane Break will adjust the break schedule after pausing:&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;If the pause is short, Sane Break will reset the time to the next break.&lt;/li&gt;
  &lt;li&gt;If it is longer, Sane Break will reset the entire break cycle, and the next break will be the first small break in the cycle.&lt;/li&gt;
&lt;/ul&gt;</source>
        <translation>&lt;h3&gt;Pauzeschema herstellen na pauze&lt;/h3&gt;
&lt;p&gt;Sane Break zal het pauzeschema aanpassen na een pauze:&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;Als de pauze kort is, zal Sane Break de tijd instellen op de volgende pauze.&lt;/li&gt;
  &lt;li&gt;Als de pauze langer is, zal Sane Break de gehele cyclus herstellen en de volgende lange pauze instellen als korte.&lt;/li&gt;
&lt;/ul&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1019"/>
        <source>Long pause threshold</source>
        <translation>Drempelwaarde van lange pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1070"/>
        <source>Short pause threshold</source>
        <translation>Drempelwaarde van korte pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1285"/>
        <source>Let&apos;s say you&apos;ve configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <extracomment>Tooltip for &quot;Auto screen lock&quot; config</extracomment>
        <translation>Voorbeeld: de ingestelde tijd is 2 minuten. Een pauze van minder dan 2 minuten zal leiden
tot vergrendeling van het scherm, 2 minuten nadat de pauze is begonnen. Als de pauze meer
dan 2 minuten duurt, zal het scherm sowieso worden vergrendeld.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1290"/>
        <source>Automatic screen lock after break start</source>
        <translation>Scherm automatisch vergrendelen tijdens pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1310"/>
        <source>Please grant accessibility permission and restart app to use auto lock screen</source>
        <translation>Verleen toegang tot de toegankelijkheidsinstellingen en herstart het programma</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1263"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Helpen met vertalen!&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1443"/>
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
&lt;p&gt;Voor pauzes die niet kunnen worden overgeslagen. &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break-ontwikkelaars&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="299"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (small break)</extracomment>
        <translation>De duur van korte pauzes (in seconden)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="457"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (big break)</extracomment>
        <translation>De duur van lange pauzes (in seconden)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="494"/>
        <source>Postpone minutes</source>
        <translation>Uitstellen, gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="510"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;%1&lt;/b&gt; seconds. If you stop your activity during that time, or choose to continue until the timer runs out, it transitions to full-screen. During the break, Sane Break locks into full-screen mode after &lt;b&gt;%2&lt;/b&gt; seconds of inactivity. If you resume activity before the lock, the prompt shrinks to a small window temporarily but re-expands to full-screen when you&apos;re idle again or the timer ends.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="627"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Herinneringssnelheid&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="644"/>
        <source>Off</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>Uit</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="651"/>
        <source>Slower</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>Langzamer</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="658"/>
        <source>Default</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>Standaard</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="668"/>
        <source>Faster</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>Sneller</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Break Window Appearance&lt;/h3&gt;</source>
        <translation type="vanished">&lt;h3&gt;Pauzevensteruiterlijk&lt;/h3&gt;</translation>
    </message>
    <message>
        <source>Countdown Transparency</source>
        <translation type="vanished">Doorzichtigheid van aftelklok</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1095"/>
        <location filename="../../src/gui/pref-window.ui" line="1126"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>Herstart Sane Break als er geen geluid wordt afgespeeld</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1112"/>
        <location filename="../../src/gui/pref-window.ui" line="1119"/>
        <location filename="../../src/gui/pref-window.ui" line="1174"/>
        <location filename="../../src/gui/pref-window.ui" line="1181"/>
        <source>Play</source>
        <translation>Afspelen</translation>
    </message>
    <message>
        <source>Reset break after paused for</source>
        <translation type="vanished">Opschorten indien gepauzeerd gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="881"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for &quot;Pause if idle for&quot; config</extracomment>
        <translation>Sane Break zal de pauze opschorten als er geen activiteit wordt waargenomen gedurende het opgegeven aantal minuten</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="884"/>
        <source>Pause if idle for</source>
        <translation>Opschorten indien inactief gedurende</translation>
    </message>
    <message>
        <source>Reset cycle after paused for</source>
        <translation type="vanished">Nieuwe cyclus starten indien gepauzeerd gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="252"/>
        <location filename="../../src/gui/pref-window.ui" line="1086"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Korte pauzes&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="309"/>
        <location filename="../../src/gui/pref-window.ui" line="394"/>
        <location filename="../../src/gui/pref-window.ui" line="925"/>
        <location filename="../../src/gui/pref-window.ui" line="1006"/>
        <location filename="../../src/gui/pref-window.ui" line="1057"/>
        <source> min</source>
        <translation> min.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="711"/>
        <source>Show tips to kill Sane Break when locked in full-screen mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1129"/>
        <location filename="../../src/gui/pref-window.ui" line="1167"/>
        <source>Bell before breaks</source>
        <translation>Bel vóór pauzes</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="261"/>
        <location filename="../../src/gui/pref-window.ui" line="526"/>
        <location filename="../../src/gui/pref-window.ui" line="612"/>
        <source> sec</source>
        <translation> sec.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="302"/>
        <location filename="../../src/gui/pref-window.ui" line="460"/>
        <source>Break length</source>
        <translation>Pauzeduur</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="729"/>
        <location filename="../../src/gui/pref-window.ui" line="756"/>
        <source>(One line per message)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="736"/>
        <source>Big break prompt messages:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="743"/>
        <source>Small break prompt messages:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="765"/>
        <source>&lt;h3&gt;Color Theme&lt;/h3&gt;
All color are in &lt;code&gt;#AARRGGBB&lt;/code&gt; format.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="778"/>
        <source>Big break</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="785"/>
        <source>Message</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="802"/>
        <source>Small break</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="809"/>
        <source>Background</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="846"/>
        <source>Count down</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1098"/>
        <location filename="../../src/gui/pref-window.ui" line="1195"/>
        <source>Bell after breaks</source>
        <translation>Bel ná pauzes</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1207"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;Tip&lt;/strong&gt;: het gebruik van lokale geluidsbestanden is mogelijk door middel van &lt;code&gt;file:/D:\sound.mp3&lt;/code&gt;  (Windows) of &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="362"/>
        <location filename="../../src/gui/pref-window.ui" line="1145"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Lange pauzes&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="371"/>
        <source> breaks</source>
        <translation> pauzes</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Shared Options&lt;/h3&gt;</source>
        <translation type="vanished">&lt;h3&gt;Gedeelde opties&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="940"/>
        <source>Pause on battery</source>
        <translation>Opschorten tijdens werken op accu</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="947"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>Pauzes opschorten indien een van deze programma&apos;s actief is:</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="954"/>
        <source>(Write one program name per line)</source>
        <translation>(Eén programma per regel)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1233"/>
        <source>Configuration file</source>
        <translation>Configuratiebestand</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1325"/>
        <source>Launch Sane Break at start up</source>
        <translation>Sane Break automatisch opstarten</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1524"/>
        <source>Revert to Last Saved</source>
        <translation>Vorig bewaard bestand terugzetten</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1226"/>
        <source>Language</source>
        <translation>Taal</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1250"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>Herstart het programma om de wijziging toe te passen.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1547"/>
        <source>Save</source>
        <translation>Opslaan</translation>
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
            <numerusform>%n min.</numerusform>
            <numerusform>%n min.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="142"/>
        <location filename="../../src/gui/pref-window.cpp" line="151"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>Iedere %n min.</numerusform>
            <numerusform>Iedere %n min.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="298"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n sec.</numerusform>
            <numerusform>%n sec.</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="297"/>
        <source>Disabled</source>
        <translation>Uitgeschakeld</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="308"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>Volgende pauze starten na middelklikken op systeemvakpictogram</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="314"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>Volgende pauze starten na dubbelklikken op systeemvakpictogram</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="382"/>
        <source>Save</source>
        <translation>Opslaan</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="383"/>
        <source>Discard</source>
        <translation>Verwerpen</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="384"/>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="279"/>
        <source>Setting autostart failed</source>
        <translation>Automatisch opstarten is niet mogelijk</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="379"/>
        <source>The preferences have been modified.</source>
        <translation>De instellingen zijn aangepast.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="380"/>
        <source>Do you want to save your changes?</source>
        <translation>Wilt u de instellingen opslaan?</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="86"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>Er is geen systeemvakpictogram beschikbaar - het normale venster wordt getoond</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="92"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>De vensterbeheerder heeft geen ondersteuning voor lagen, dus de positionering zal niet goed werken</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="96"/>
        <source>Sane Break is not compiled with layer shell support, and window positioning will be bugged</source>
        <translation>Sane Break is niet gebouwd met ondersteuning voor lagen, dus de positionering zal niet goed werken</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="100"/>
        <source>You need to install the shell extension if you are using GNOME. Otherwise, Sane break is almost unusable</source>
        <translation>Installeer de uitbreiding indien u gebruikmaakt van GNOME, anders zal Sane Break niet naar behoren werken.</translation>
    </message>
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <source>big break</source>
        <translation type="vanished">lange pauze</translation>
    </message>
    <message>
        <source>small break</source>
        <translation type="vanished">korte pauze</translation>
    </message>
    <message>
        <source>Next Break</source>
        <translation type="vanished">Volgende pauze</translation>
    </message>
    <message>
        <source>Big Break</source>
        <translation type="vanished">Lange pauze</translation>
    </message>
    <message>
        <source>Postpone</source>
        <translation type="vanished">Uitstellen</translation>
    </message>
    <message numerus="yes">
        <source>%n min</source>
        <translation type="vanished">
            <numerusform>%n min.</numerusform>
            <numerusform>%n min.</numerusform>
        </translation>
    </message>
    <message>
        <source>Quit</source>
        <translation type="vanished">Afsluiten</translation>
    </message>
    <message>
        <source>Enable Break</source>
        <translation type="vanished">Pauze inschakelen</translation>
    </message>
    <message>
        <source>Next break after %1</source>
        <translation type="vanished">Volgende pauze over %1</translation>
    </message>
    <message>
        <source>Next big break after %1</source>
        <translation type="vanished">Volgende lange pauze over %1</translation>
    </message>
    <message>
        <source>Enter Reading Mode</source>
        <translation type="vanished">Leesmodus starten</translation>
    </message>
    <message>
        <source>Preferences</source>
        <translation type="vanished">Instellingen</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="91"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation>Weet u zeker dat u Sane Break wilt afsluiten?</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="92"/>
        <source>You can postpone the breaks instead.</source>
        <translation>U kunt ook de pauzes uitstellen.</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="94"/>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="95"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>%n minuut uitstellen</numerusform>
            <numerusform>%n minuten uitstellen</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="96"/>
        <source>Yes</source>
        <translation>Ja</translation>
    </message>
    <message>
        <source>Paused on battery</source>
        <translation type="vanished">Opgeschort tijdens werken op accu</translation>
    </message>
    <message>
        <source>Paused on app running</source>
        <translation type="vanished">Opgeschort tijdens actief programma</translation>
    </message>
    <message>
        <source>Paused on idle</source>
        <translation type="vanished">Opgeschort tijdens inactiviteit</translation>
    </message>
    <message>
        <source>Exit Reading Mode</source>
        <translation type="vanished">Leesmodus verlaten</translation>
    </message>
</context>
<context>
    <name>SanePreferences</name>
    <message>
        <location filename="../../src/core/preferences.cpp" line="50"/>
        <source>Time for a small break</source>
        <translation type="unfinished">Tijd voor een korte pauze</translation>
    </message>
    <message>
        <location filename="../../src/core/preferences.cpp" line="52"/>
        <source>Time for a big break</source>
        <translation type="unfinished">Tijd voor een lange pauze</translation>
    </message>
</context>
<context>
    <name>StatusTrayWindow</name>
    <message>
        <location filename="../../src/gui/tray.cpp" line="52"/>
        <source>Postpone</source>
        <translation>Uitstellen</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/tray.cpp" line="55"/>
        <location filename="../../src/gui/tray.cpp" line="141"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n min.</numerusform>
            <numerusform>%n min.</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="58"/>
        <source>Enable Break</source>
        <translation>Pauze inschakelen</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="65"/>
        <source>Preferences</source>
        <translation>Instellingen</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="68"/>
        <source>Quit</source>
        <translation>Afsluiten</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="94"/>
        <source>big break</source>
        <translation>lange pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="94"/>
        <source>small break</source>
        <translation>korte pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="74"/>
        <source>Next break after %1</source>
        <translation>Volgende pauze over %1</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="45"/>
        <source>Take a small break instead</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="76"/>
        <source>Next big break after %1</source>
        <translation>Volgende lange pauze over %1</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="86"/>
        <source>Paused on battery</source>
        <translation>Opgeschort tijdens werken op accu</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="88"/>
        <source>Paused on app running</source>
        <translation>Opgeschort tijdens actief programma</translation>
    </message>
    <message>
        <location filename="../../src/gui/tray.cpp" line="90"/>
        <source>Paused on idle</source>
        <translation>Opgeschort tijdens inactiviteit</translation>
    </message>
</context>
<context>
    <name>WelcomeWindow</name>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="116"/>
        <source>Welcome to Sane Break</source>
        <translation>Welkom in Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="117"/>
        <source>&lt;h3 align=center&gt;Welcome to Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow. Sane Break will stay in the system tray and remind you to take breaks at regular intervals. To quit, go to &quot;Postpone&quot; in the tray menu.&lt;/p&gt;</source>
        <translation>&lt;h3 align=center&gt;Welkom in Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is een platformonafhankelijk programma die u eraan herinnert pauzes te nemen, zonder u uit uw ritme te halen. Sane Break wordt geminimaliseerd opgestart en herinnert u er tijdig aan om pauzes te nemen, die u naar eigen inzicht kunt instellen. Afsluiten kan door middel van de systeemvakmenu-optie ‘Uitstellen’.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="96"/>
        <location filename="../../src/gui/welcome.cpp" line="126"/>
        <source>Read More</source>
        <translation>Meer informatie</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="98"/>
        <location filename="../../src/gui/welcome.cpp" line="127"/>
        <source>Ignore</source>
        <translation>Negeren</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="99"/>
        <location filename="../../src/gui/welcome.cpp" line="128"/>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="102"/>
        <location filename="../../src/gui/welcome.cpp" line="129"/>
        <source>OK</source>
        <translation>Oké</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="124"/>
        <source>Language</source>
        <translation>Taal</translation>
    </message>
</context>
</TS>
