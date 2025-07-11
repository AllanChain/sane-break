<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="nl_NL" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="54"/>
        <source>Failed to connect to Flatpak response</source>
        <extracomment>Error message when requesting autostart in Flatpak got no response</extracomment>
        <translation>Er kan geen verbinding met Flatpak worden gemaakt</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="74"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="84"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="90"/>
        <source>Autostart desktop entry not writable</source>
        <extracomment>Error message when failed to write Linux desktop entry for autostart</extracomment>
        <translation>Automatisch opstarten is niet beschrijfbaar</translation>
    </message>
    <message>
        <source>File not writable</source>
        <translation type="vanished">Het bestand is niet beschrijfbaar</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="108"/>
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
        <location filename="../../src/lib/linux/auto-start.cpp" line="44"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="33"/>
        <source>Unknown error</source>
        <extracomment>Error message when requesting autostart got unknown error</extracomment>
        <translation>Onbekende foutmelding</translation>
    </message>
</context>
<context>
    <name>BreakReminder</name>
    <message>
        <location filename="../../src/gui/break-window.ui" line="167"/>
        <source>Lock Screen</source>
        <translation>Scherm vergrendelen</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.ui" line="221"/>
        <source>Exit Force Break</source>
        <translation>Gedwongen pauze beëindigen</translation>
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
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="67"/>
        <location filename="../../src/gui/widgets/language-select.cpp" line="94"/>
        <source>Default</source>
        <translation>Standaard</translation>
    </message>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="79"/>
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
        <location filename="../../src/gui/pref-window.ui" line="576"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <extracomment>Tooltip for &quot;Flash break prompt for&quot; config</extracomment>
        <translation>De duur - in seconden - alvorens een pauzeherinnering wordt getoond.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="356"/>
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
        <location filename="../../src/gui/pref-window.ui" line="359"/>
        <location filename="../../src/gui/pref-window.ui" line="396"/>
        <source>Break every</source>
        <translation>Pauzeren, iedere</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="393"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (big break)</extracomment>
        <translation>Ieder x-aantal pauzes zal een lange zijn</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="478"/>
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
        <location filename="../../src/gui/pref-window.ui" line="579"/>
        <source>Flash break prompt for</source>
        <translation>Pauzeherinnering tonen, gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="586"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <extracomment>Tooltip for &quot;Confirm break after&quot; config</extracomment>
        <translation>Na een bepaalde mate van inactiviteit tijdens een pauze, zal Sane Break een schermvullende melding tonen.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="589"/>
        <source>Confirm break after</source>
        <translation>Pauze automatisch bevestigen na</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Reminder Appearance&lt;/h3&gt;</source>
        <translation type="vanished">&lt;h3&gt;Vormgeving van herinnering&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="695"/>
        <source>Flash speed</source>
        <translation>Flitssnelheid</translation>
    </message>
    <message>
        <source>Text Transparency</source>
        <translation type="vanished">Tekstdoorzichtigheid</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="886"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Pauzevoorwaarden&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="978"/>
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
        <location filename="../../src/gui/pref-window.ui" line="1039"/>
        <source>Long pause threshold</source>
        <translation>Drempelwaarde van lange pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1093"/>
        <source>Short pause threshold</source>
        <translation>Drempelwaarde van korte pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1310"/>
        <source>Let&apos;s say you&apos;ve configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <extracomment>Tooltip for &quot;Auto screen lock&quot; config</extracomment>
        <translation>Voorbeeld: de ingestelde tijd is 2 minuten. Een pauze van minder dan 2 minuten zal leiden
tot vergrendeling van het scherm, 2 minuten nadat de pauze is begonnen. Als de pauze meer
dan 2 minuten duurt, zal het scherm sowieso worden vergrendeld.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1315"/>
        <source>Automatic screen lock after break start</source>
        <translation>Scherm automatisch vergrendelen tijdens pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1338"/>
        <source>Please grant accessibility permission and restart app to use auto lock screen</source>
        <translation>Verleen toegang tot de toegankelijkheidsinstellingen en herstart het programma</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1288"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Helpen met vertalen!&lt;/a&gt;</translation>
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
&lt;p&gt;Voor pauzes die niet kunnen worden overgeslagen. &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break-ontwikkelaars&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="302"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (small break)</extracomment>
        <translation>De duur van korte pauzes (in seconden)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="466"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (big break)</extracomment>
        <translation>De duur van lange pauzes (in seconden)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="503"/>
        <source>Postpone minutes</source>
        <translation>Uitstellen, gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="519"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;%1&lt;/b&gt; seconds. If you stop your activity during that time, or choose to continue until the timer runs out, it transitions to full-screen. During the break, Sane Break locks into full-screen mode after &lt;b&gt;%2&lt;/b&gt; seconds of inactivity. If you resume activity before the lock, the prompt shrinks to a small window temporarily but re-expands to full-screen when you&apos;re idle again or the timer ends.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Gedrag van herinnering&lt;/h3&gt;
&lt;p&gt;Sane Break toont een kleine melding gedurende &lt;b&gt;%1&lt;/b&gt; seconden. Als u stopt met werken of ervoor kiest om door te gaan tot de tijd is verlopen, dan wordt het venster schermvullend weergegeven. Tijdens de pauze wordt het scherm vergrendeld na &lt;b&gt;%2&lt;/b&gt; seconden van inactiviteit. Indien u weer actief bent vóór het vergrendelen, dan krimpt de melding tijdelijk naar een klein venster, maar zodra u weer inactief bent of de tijd is verstreken, wordt het opnieuw schermvullend getoond.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="642"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Herinneringssnelheid&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="659"/>
        <source>Off</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>Uit</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="666"/>
        <source>Slower</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>Langzamer</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="673"/>
        <source>Default</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation>Standaard</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="683"/>
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
        <location filename="../../src/gui/pref-window.ui" line="1118"/>
        <location filename="../../src/gui/pref-window.ui" line="1149"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>Herstart Sane Break als er geen geluid wordt afgespeeld</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1135"/>
        <location filename="../../src/gui/pref-window.ui" line="1142"/>
        <location filename="../../src/gui/pref-window.ui" line="1197"/>
        <location filename="../../src/gui/pref-window.ui" line="1204"/>
        <source>Play</source>
        <translation>Afspelen</translation>
    </message>
    <message>
        <source>Reset break after paused for</source>
        <translation type="vanished">Opschorten indien gepauzeerd gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="895"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for &quot;Pause if idle for&quot; config</extracomment>
        <translation>Sane Break zal de pauze opschorten als er geen activiteit wordt waargenomen gedurende het opgegeven aantal minuten</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="898"/>
        <source>Pause if idle for</source>
        <translation>Opschorten indien inactief gedurende</translation>
    </message>
    <message>
        <source>Reset cycle after paused for</source>
        <translation type="vanished">Nieuwe cyclus starten indien gepauzeerd gedurende</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="252"/>
        <location filename="../../src/gui/pref-window.ui" line="1109"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Korte pauzes&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="312"/>
        <location filename="../../src/gui/pref-window.ui" line="403"/>
        <location filename="../../src/gui/pref-window.ui" line="939"/>
        <location filename="../../src/gui/pref-window.ui" line="1023"/>
        <location filename="../../src/gui/pref-window.ui" line="1077"/>
        <source> min</source>
        <translation> min.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1152"/>
        <location filename="../../src/gui/pref-window.ui" line="1190"/>
        <source>Bell before breaks</source>
        <translation>Bel vóór pauzes</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="261"/>
        <location filename="../../src/gui/pref-window.ui" line="535"/>
        <location filename="../../src/gui/pref-window.ui" line="624"/>
        <source> sec</source>
        <translation> sec.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="305"/>
        <location filename="../../src/gui/pref-window.ui" line="469"/>
        <source>Break length</source>
        <translation>Pauzeduur</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="737"/>
        <location filename="../../src/gui/pref-window.ui" line="770"/>
        <source>(One line per message)</source>
        <translation>(Eén regel per bericht)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="744"/>
        <source>Big break prompt messages:</source>
        <translation>Berichten voor lange pauze:</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="754"/>
        <source>Small break prompt messages:</source>
        <translation>Berichten voor korte pauze:</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="779"/>
        <source>&lt;h3&gt;Color Theme&lt;/h3&gt;
All color are in &lt;code&gt;#AARRGGBB&lt;/code&gt; format.</source>
        <translation>&lt;h3&gt;Kleurenschema&lt;/h3&gt;
Alle kleuren zijn in &lt;code&gt;#AARRGGBB&lt;/code&gt;-formaat.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="792"/>
        <source>Big break</source>
        <translation>Lange pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="799"/>
        <source>Message</source>
        <translation>Bericht</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="816"/>
        <source>Small break</source>
        <translation>Korte pauze</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="823"/>
        <source>Background</source>
        <translation>Achtergrond</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="860"/>
        <source>Count down</source>
        <translation>Aftellen</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1121"/>
        <location filename="../../src/gui/pref-window.ui" line="1218"/>
        <source>Bell after breaks</source>
        <translation>Bel ná pauzes</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1230"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;Tip&lt;/strong&gt;: het gebruik van lokale geluidsbestanden is mogelijk door middel van &lt;code&gt;file:/D:\sound.mp3&lt;/code&gt;  (Windows) of &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="368"/>
        <location filename="../../src/gui/pref-window.ui" line="1168"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Lange pauzes&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="377"/>
        <source> breaks</source>
        <translation> pauzes</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Shared Options&lt;/h3&gt;</source>
        <translation type="vanished">&lt;h3&gt;Gedeelde opties&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="957"/>
        <source>Pause on battery</source>
        <translation>Opschorten tijdens werken op accu</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="964"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>Pauzes opschorten indien een van deze programma&apos;s actief is:</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="971"/>
        <source>(Write one program name per line)</source>
        <translation>(Eén programma per regel)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1256"/>
        <source>Configuration file</source>
        <translation>Configuratiebestand</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1353"/>
        <source>Launch Sane Break at start up</source>
        <translation>Sane Break automatisch opstarten</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1479"/>
        <source>Get the Source Code</source>
        <translation>Broncode bekijken</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1534"/>
        <source>Revert to Last Saved</source>
        <translation>Vorig bewaard bestand terugzetten</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1249"/>
        <source>Language</source>
        <translation>Taal</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1275"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>Herstart het programma om de wijziging toe te passen.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1499"/>
        <source>Third-party Libraries Licenses</source>
        <translation>Licenties van externe bibliotheken</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1557"/>
        <source>Save</source>
        <translation>Opslaan</translation>
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
            <numerusform>%n min.</numerusform>
            <numerusform>%n min.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="143"/>
        <location filename="../../src/gui/pref-window.cpp" line="152"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>Iedere %n min.</numerusform>
            <numerusform>Iedere %n min.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="309"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n sec.</numerusform>
            <numerusform>%n sec.</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="308"/>
        <source>Disabled</source>
        <translation>Uitgeschakeld</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="319"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>Volgende pauze starten na middelklikken op systeemvakpictogram</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="324"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>Volgende pauze starten na dubbelklikken op systeemvakpictogram</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="384"/>
        <source>Save</source>
        <translation>Opslaan</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="385"/>
        <source>Discard</source>
        <translation>Verwerpen</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="386"/>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="283"/>
        <source>Setting autostart failed</source>
        <translation>Automatisch opstarten is niet mogelijk</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="381"/>
        <source>The preferences have been modified.</source>
        <translation>De instellingen zijn aangepast.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="382"/>
        <source>Do you want to save your changes?</source>
        <translation>Wilt u de instellingen opslaan?</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="95"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>Er is geen systeemvakpictogram beschikbaar - het normale venster wordt getoond</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="101"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>De vensterbeheerder heeft geen ondersteuning voor lagen, dus de positionering zal niet goed werken</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="105"/>
        <source>Sane Break is not compiled with layer shell support, and window positioning will be bugged</source>
        <translation>Sane Break is niet gebouwd met ondersteuning voor lagen, dus de positionering zal niet goed werken</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="109"/>
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
        <translation type="obsolete">
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
        <location filename="../../src/app/app.cpp" line="112"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation>Weet u zeker dat u Sane Break wilt afsluiten?</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="113"/>
        <source>You can postpone the breaks instead.</source>
        <translation>U kunt ook de pauzes uitstellen.</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="91"/>
        <location filename="../../src/app/app.cpp" line="115"/>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="87"/>
        <source>Are you sure to postpone for %n minute?</source>
        <translation>
            <numerusform>Weet u zeker dat u de pauze gedurende %n minuut wilt uitstellen?</numerusform>
            <numerusform>Weet u zeker dat u de pauze gedurende %n minuten wilt uitstellen?</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="89"/>
        <source>You haven&apos;t taken breaks for %1 minutes.</source>
        <translation>U heeft al %1 minuten lang geen pauze genomen.</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="116"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>%n minuut uitstellen</numerusform>
            <numerusform>%n minuten uitstellen</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="92"/>
        <location filename="../../src/app/app.cpp" line="117"/>
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
        <location filename="../../src/core/preferences.cpp" line="49"/>
        <source>Time for a small break</source>
        <translation>Tijd voor een korte pauze</translation>
    </message>
    <message>
        <location filename="../../src/core/preferences.cpp" line="52"/>
        <source>Time for a big break</source>
        <translation>Tijd voor een lange pauze</translation>
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
        <translation>Neem liever een korte pauze</translation>
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
