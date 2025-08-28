<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="54"/>
        <source>Failed to connect to Flatpak response</source>
        <extracomment>Error message when requesting autostart in Flatpak got no response</extracomment>
        <translation>Échec lors de la connexion à la réponse Flatpak</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="74"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="84"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="90"/>
        <source>Autostart desktop entry not writable</source>
        <extracomment>Error message when failed to write Linux desktop entry for autostart</extracomment>
        <translation>Entrée de démarrage automatique non modifiable</translation>
    </message>
    <message>
        <source>File not writable</source>
        <translation type="vanished">Fichier inaccessible à l&apos;écriture</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="108"/>
        <source>The request to autostart was cancelled</source>
        <extracomment>Error message when requesting autostart in Flatpak got cancelled by user</extracomment>
        <translation>La requête de démarrage automatique a été annulée.</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="30"/>
        <source>Permission denied</source>
        <extracomment>Error message when requesting autostart got permission denied</extracomment>
        <translation>Permission refusée</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="44"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="33"/>
        <source>Unknown error</source>
        <extracomment>Error message when requesting autostart got unknown error</extracomment>
        <translation>Erreur inconnue</translation>
    </message>
</context>
<context>
    <name>BreakReminder</name>
    <message>
        <location filename="../../src/app/break-window.ui" line="246"/>
        <source>Lock Screen</source>
        <translation>Verrouiller l&apos;écran</translation>
    </message>
    <message>
        <location filename="../../src/app/break-window.ui" line="300"/>
        <source>Exit Force Break</source>
        <translation>Quitter la pause forcée</translation>
    </message>
</context>
<context>
    <name>BreakWindow</name>
    <message>
        <source>Time for a big break</source>
        <translation type="vanished">Il est temps de faire une grosse pause</translation>
    </message>
    <message>
        <source>Time for a small break</source>
        <translation type="vanished">Il est temps de faire une petite pause</translation>
    </message>
    <message>
        <location filename="../../src/app/break-window.cpp" line="122"/>
        <source>Break will end at: %1</source>
        <translation>La pause prendra fin à&#xa0;: %1</translation>
    </message>
    <message>
        <location filename="../../src/app/break-window.cpp" line="124"/>
        <source>Break has ended</source>
        <translation>La pause est terminée</translation>
    </message>
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/app/widgets/language-select.cpp" line="67"/>
        <location filename="../../src/app/widgets/language-select.cpp" line="94"/>
        <source>Default</source>
        <translation>Défaut</translation>
    </message>
    <message>
        <location filename="../../src/app/widgets/language-select.cpp" line="79"/>
        <source>English</source>
        <comment>current language</comment>
        <extracomment>This is the name of the current language in the current language</extracomment>
        <translation>Français</translation>
    </message>
</context>
<context>
    <name>PrefWindow</name>
    <message>
        <source>Break</source>
        <translation type="vanished">Grande pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="126"/>
        <source>Pause</source>
        <translation>Pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="142"/>
        <source>Sound</source>
        <translation>Son</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="161"/>
        <source>General</source>
        <translation>Général</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="183"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="372"/>
        <source>Length of work time between breaks</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (small break)</extracomment>
        <translation>Durée du temps de travail entre chaque pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="375"/>
        <location filename="../../src/app/pref-window.ui" line="412"/>
        <source>Break every</source>
        <translation>Faire une pause toutes les</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="409"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (big break)</extracomment>
        <translation>La dernière de chaque série sera une grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="494"/>
        <source>&lt;h3&gt;Postpone&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Retarder une pause&lt;/h3&gt;</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;Flash break prompt for&lt;/b&gt; seconds. If you stop working or ignore it past this time, it transitions to full-screen. After &lt;b&gt;Confirm break after&lt;/b&gt; seconds of inactivity during the break, it locks until the timer ends.&lt;/p&gt;</source>
        <translation type="vanished">&lt;h3&gt;Comportement des Rappels&lt;/h3&gt;
&lt;p&gt;Sane Break affiche un petit panneau pendant&lt;b&gt;X&lt;/b&gt; secondes. Si vous arrêtez de travailler ou continuez d&apos;ignorer ce panneau tout ce temps, le panneau passera en plein écran. Après &lt;b&gt;X&lt;/b&gt; secondes d&apos;inactivité pendant la pause, les contrôles sont verrouillés jusqu&apos;à la fin du minuteur.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="595"/>
        <source>Flash break prompt for</source>
        <translation>Faire clignoter le message de pause pendant</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="602"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <extracomment>Tooltip for &quot;Confirm break after&quot; config</extracomment>
        <translation>Si inactif pendant ce temps lors d&apos;une pause, Sane Break restera en plein écran sans retour arrière possible.</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="605"/>
        <source>Confirm break after</source>
        <translation>Confirmer le début de la pause après</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="709"/>
        <source>Flash speed</source>
        <translation>Vitesse de clignotement</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="764"/>
        <location filename="../../src/app/pref-window.ui" line="797"/>
        <source>(One line per message)</source>
        <translation>(Une ligne par message)</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="771"/>
        <source>Big break prompt messages:</source>
        <translation>Message du panneau de rappel pour les grosses pauses&#xa0;:</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="781"/>
        <source>Small break prompt messages:</source>
        <translation>Message du panneau de rappel pour les petites pauses&#xa0;:</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="660"/>
        <source>Max number of force break exits</source>
        <translation>Nombre maximal de Sorties de Pause Forcées</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="110"/>
        <source>Interface</source>
        <translation>Interface</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="667"/>
        <source> times</source>
        <translation> fois</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="806"/>
        <source>&lt;h3&gt;Components to Show&lt;/h3&gt;

&lt;p&gt;User interface components to show in the prompt and full-screen window.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Composants à montrer&lt;/h3&gt;

&lt;p&gt;Ce qui apparaîtra dans le panneau et sur le plein-écran de pause.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="831"/>
        <location filename="../../src/app/pref-window.ui" line="877"/>
        <source>Time when break ends</source>
        <translation>Heure à laquelle la pause s&apos;arrête</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="852"/>
        <location filename="../../src/app/pref-window.ui" line="898"/>
        <source>Countdown timer</source>
        <translation>Compte à rebours</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="824"/>
        <location filename="../../src/app/pref-window.ui" line="870"/>
        <source>Progress bar</source>
        <translation>Barre de chargement</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="838"/>
        <location filename="../../src/app/pref-window.ui" line="884"/>
        <source>Clock</source>
        <translation>Horloge</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="845"/>
        <location filename="../../src/app/pref-window.ui" line="891"/>
        <source>Buttons</source>
        <translation>Boutons</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="907"/>
        <source>&lt;h3&gt;Color Theme&lt;/h3&gt;
All color are in &lt;code&gt;#AARRGGBB&lt;/code&gt; format.</source>
        <translation>&lt;h3&gt;Thème de couleur&lt;/h3&gt;
Toutes les couleurs sont au format &lt;code&gt;#AARRGGBB&lt;/code&gt;.</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="991"/>
        <source>Big break</source>
        <translation>Grande pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="960"/>
        <source>Message</source>
        <translation>Message</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="998"/>
        <source>Small break</source>
        <translation>Petite pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="967"/>
        <source>Background</source>
        <translation>Arrière-plan</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="974"/>
        <source>Count down</source>
        <translation>Compte à rebours</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1038"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Conditions de pause&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1130"/>
        <source>&lt;h3&gt;Reset Break Schedule After Pause&lt;/h3&gt;
&lt;p&gt;Sane Break will adjust the break schedule after pausing:&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;If the pause is short, Sane Break will reset the time to the next break.&lt;/li&gt;
  &lt;li&gt;If it is longer, Sane Break will reset the entire break cycle, and the next break will be the first small break in the cycle.&lt;/li&gt;
&lt;/ul&gt;</source>
        <translation>&lt;h3&gt;Réinitialiser le programme de pause après chaque pause&lt;/h3&gt;
&lt;p&gt;Sane Break réajustera le programme des pauses après chaque pause&#xa0;:&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;Si la pause est courte, Sane Break réinitialisera le temps restant avant la pause d&apos;après.&lt;/li&gt;
  &lt;li&gt;Si la pause est plus longue, Sane Break réinitialisera le cycle de pause en entier, et la prochaine pause sera la première petite pause du cycle.&lt;/li&gt;
&lt;/ul&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1191"/>
        <source>Long pause threshold</source>
        <translation>Seuil d&apos;une longue pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1245"/>
        <source>Short pause threshold</source>
        <translation>Seuil d&apos;une petite pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1457"/>
        <source>Let&apos;s say you&apos;ve configured it to 2 minutes. If the break is shorter than 2 minutes, the
screen will lock 2 minutes after the break starts, provided there is no activity after the
break. If the break is longer than 2 minutes, the screen will be locked regardless.</source>
        <extracomment>Tooltip for &quot;Auto screen lock&quot; config</extracomment>
        <translation>Imaginons que vous mettiez 2 minutes. Si la pause est plus courte que 2 minutes,
l&apos;écran se bloquera 2 minutes à partir du moment où la pause commence, si tant
est que l&apos;utilisateur est bien inactif. Si la pause dure plus longue que 2 minutes,
l&apos;écran sera quand même bloqué.</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1462"/>
        <source>Automatic screen lock after break start</source>
        <translation>Verrouillage automatique de l&apos;écran après le début de la pause</translation>
    </message>
    <message>
        <source>Please grant accessibility permission and restart app to use auto lock screen</source>
        <translation type="vanished">Merci d&apos;octroyer les droits d&apos;accès et de redémarrer l&apos;appli pour utiliser le verrouillage d&apos;écran</translation>
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
&lt;p&gt;Vous rappelle de faire des pause en évitant les automatismes. &lt;/p&gt;
&lt;p&gt;2024-2025, &lt;a href=&quot;https://github.com/AllanChain/sane-break/&quot;&gt;Sane Break developers&lt;/a&gt; (&lt;a href=&quot;https://www.gnu.org/licenses/gpl-3.0.en.html&quot;&gt;GPL 3.0&lt;/a&gt;)
&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="318"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (small break)</extracomment>
        <translation>Combien de secondes pour chaque petite pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="592"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <extracomment>Tooltip for &quot;Flash break prompt for&quot; config</extracomment>
        <translation>Combien de secondes pour continuer le travail avant que la pause ne soit forcée.</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="482"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (big break)</extracomment>
        <translation>Combien de secondes pour chaque grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="519"/>
        <source>Postpone minutes</source>
        <translation>Minutes de retard</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="694"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Apparence du panneau&lt;/h3&gt;</translation>
    </message>
    <message>
        <source>Off</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="vanished">Désactivé</translation>
    </message>
    <message>
        <source>Slower</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="vanished">Plus lent</translation>
    </message>
    <message>
        <source>Default</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="vanished">Défaut</translation>
    </message>
    <message>
        <source>Faster</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="vanished">Plus rapide</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Break Window Appearance&lt;/h3&gt;</source>
        <translation type="vanished">&lt;h3&gt;Apparence de la fenêtre de pause&lt;/h3&gt;</translation>
    </message>
    <message>
        <source>Countdown Transparency</source>
        <translation type="vanished">Transparence du compte à rebours</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1270"/>
        <location filename="../../src/app/pref-window.ui" line="1301"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>Si le son ne fonctionne pas, essayez de redémarrer Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1287"/>
        <location filename="../../src/app/pref-window.ui" line="1294"/>
        <location filename="../../src/app/pref-window.ui" line="1349"/>
        <location filename="../../src/app/pref-window.ui" line="1356"/>
        <source>Play</source>
        <translation>Écouter</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="75"/>
        <source>Schedule</source>
        <translation>Programme</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="94"/>
        <source>Reminder</source>
        <translation>Rappel</translation>
    </message>
    <message>
        <source>Reset break after paused for</source>
        <translation type="vanished">Réinitialiser ce cycle s&apos;il est en pause depuis</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1047"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for &quot;Pause if idle for&quot; config</extracomment>
        <translation>Sane Break mettra son cycle en pause s&apos;il ne détecte aucune activité sur cette période</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1050"/>
        <source>Pause if idle for</source>
        <translation>Mettre le cycle en pause si inactif depuis</translation>
    </message>
    <message>
        <source>Reset cycle after paused for</source>
        <translation type="vanished">Réinitialiser tout le cycle s&apos;il est en pause depuis</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="268"/>
        <location filename="../../src/app/pref-window.ui" line="1261"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Petites pauses&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="328"/>
        <location filename="../../src/app/pref-window.ui" line="419"/>
        <location filename="../../src/app/pref-window.ui" line="1091"/>
        <location filename="../../src/app/pref-window.ui" line="1175"/>
        <location filename="../../src/app/pref-window.ui" line="1229"/>
        <source> min</source>
        <translation> min</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1304"/>
        <location filename="../../src/app/pref-window.ui" line="1342"/>
        <source>Bell before breaks</source>
        <translation>Son de cloche avant les pauses</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="277"/>
        <location filename="../../src/app/pref-window.ui" line="551"/>
        <location filename="../../src/app/pref-window.ui" line="640"/>
        <source> sec</source>
        <translation> sec</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="321"/>
        <location filename="../../src/app/pref-window.ui" line="485"/>
        <source>Break length</source>
        <translation>Durée d&apos;une pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1273"/>
        <location filename="../../src/app/pref-window.ui" line="1370"/>
        <source>Bell after breaks</source>
        <translation>Son de cloche après les pauses</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1382"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;Astuce&lt;/strong&gt; : Vous pouvez utiliser un fichier son local avec un URL du type  &lt;code&gt;file:/D:\fichierson.mp3&lt;/code&gt;  (Windows) ou &lt;code&gt;file:/home/dossier/fichierson.mp3&lt;/code&gt; (Linux).</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="384"/>
        <location filename="../../src/app/pref-window.ui" line="1320"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Grosses pauses&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="393"/>
        <source> breaks</source>
        <translation> pauses</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="535"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;%1&lt;/b&gt; seconds. If you stop your activity during that time, or choose to continue until the timer runs out, it transitions to full-screen. During the break, Sane Break locks into full-screen mode after &lt;b&gt;%2&lt;/b&gt; seconds of inactivity. If you resume activity before the lock, the prompt shrinks to a small window temporarily but re-expands to full-screen when you&apos;re idle again or the timer ends.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Comportement du Rappel&lt;/h3&gt;
&lt;p&gt;Sane Break affiche un petit panneau pendant &lt;b&gt;%1&lt;/b&gt; secondes. Si vous arrêtez votre activité à ce moment, ou si vous la continuez jusqu&apos;à ce que le compte à rebours s&apos;écoule, le panneau passera en plein écran. Pendant la pause, Sane Break se bloque en mode plein-écran après &lt;b&gt;%2&lt;/b&gt; secondes d&apos;inactivité. Si vous reprenez votre activité avant que le blocage, le panneau rétrécira à nouveau en une petite fenêtre mais se ré-agrandira en plein-écran à la fin du compte à rebours ou si vous ne arrêtez toute activité.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="676"/>
        <source>Close windows right after small break ends</source>
        <translation>Fermer le plein-écran juste après la fin de la petite pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="683"/>
        <source>Close windows right after big break ends</source>
        <translation>Fermer le plein-écran juste après la fin de la grande pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="815"/>
        <source>&lt;h4&gt;Small break&lt;/h4&gt;</source>
        <translation>&lt;h4&gt;Petite pause&lt;/h4&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="861"/>
        <source>&lt;h4&gt;Big break&lt;/h4&gt;</source>
        <translation>&lt;h4&gt;Grande pause&lt;/h4&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1014"/>
        <source>&lt;p&gt;To preview break windows, save all changes first. The preview begins with a 2-second flashing prompt, followed by a 4-second full-screen break. A small break window will appear first, then a big break window.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Pour avoir un aperçu de la fenêtre de pause, sauvegardez d&apos;abord tous les changements. L&apos;aperçu commence par un panneau clignotant de 2 secondes, suivi par un plein-écran de pause de 4 secondes. Vous verrez d&apos;abord une fenêtre de petite pause, puis une fenêtre de grande pause.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1024"/>
        <source>Preview Break Windows</source>
        <translation>Voir un aperçu de la fenêtre de pause</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1109"/>
        <source>Pause on battery</source>
        <translation>Mettre en pause sur batterie</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1116"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>La pause s&apos;arrête dès que l&apos;un de ces programmes se met en route&#xa0;:</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1123"/>
        <source>(Write one program name per line)</source>
        <translation>(écrire un seul nom de programme par ligne)</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1408"/>
        <source>Configuration file</source>
        <translation>Fichier de configuration</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1440"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Aidez-nous à faire la traduction&#x202f;!&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1477"/>
        <source>Launch Sane Break at start up</source>
        <translation>Lancer Sane Break au démarrage de l&apos;ordinateur</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1603"/>
        <source>Get the Source Code</source>
        <translation>Obtenir le code source</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1658"/>
        <source>Revert to Last Saved</source>
        <translation>Revenir aux anciens paramètres sauvegardés</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1401"/>
        <source>Language</source>
        <translation>Langue</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1427"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>Le changement de langue prendra effet après redémarrage de l&apos;appli.</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1623"/>
        <source>Third-party Libraries Licenses</source>
        <translation>Licences des bibliothèques tierces</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1665"/>
        <source>Defaults</source>
        <translation>Paramètres par défaut</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.ui" line="1688"/>
        <source>Save</source>
        <translation>Sauvegarder</translation>
    </message>
</context>
<context>
    <name>PreferenceWindow</name>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="400"/>
        <location filename="../../src/app/pref-window.cpp" line="401"/>
        <location filename="../../src/app/pref-window.cpp" line="402"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n min</numerusform>
            <numerusform>%n min</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="166"/>
        <location filename="../../src/app/pref-window.cpp" line="177"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>Chaque minute</numerusform>
            <numerusform>Toutes les %n minutes</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/pref-window.cpp" line="399"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n sec</numerusform>
            <numerusform>%n sec</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="398"/>
        <source>Disabled</source>
        <translation>Désactivé</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="409"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>Démarrer la pause suivante en faisant clic du milieu sur l&apos;icône dans la barre des tâches</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="414"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>Démarrer la pause suivante en faisant double-clic sur l&apos;icône dans la barre des tâches</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="482"/>
        <source>Save</source>
        <translation>Sauvegarder</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="483"/>
        <source>Discard</source>
        <translation>Rejeter</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="484"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="374"/>
        <source>Setting autostart failed</source>
        <translation>La mise en place du démarrage automatique a échoué</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="479"/>
        <source>The preferences have been modified.</source>
        <translation>Les préférences ont été modifiées.</translation>
    </message>
    <message>
        <location filename="../../src/app/pref-window.cpp" line="480"/>
        <source>Do you want to save your changes?</source>
        <translation>Voulez-vous sauvegarder les changements&#x202f;?</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="59"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>Icône de la barre des tâches indisponible, retour à la fenêtre normale</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="63"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>Le compositeur ne supporte pas le &quot;layer shell&quot;, et la position de la fenêtre sera incorrecte</translation>
    </message>
    <message>
        <source>Sane Break is not compiled with layer shell support, and window positioning will be bugged</source>
        <translation type="vanished">Sane Break n&apos;est pas compilé avec support du &quot;layer shell&quot;, et la position de la fenêtre sera incorrecte</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="66"/>
        <source>You need to install the shell extension if you are using GNOME. Otherwise, Sane break is almost unusable</source>
        <translation>Si vous utilisez GNOME, vous devrez installer l&apos;extension &quot;shell&quot;, ou Sane Break sera presque inutilisable</translation>
    </message>
    <message>
        <location filename="../../src/app/main.cpp" line="55"/>
        <source>Another instance of Sane Break is running.</source>
        <translation>Une autre instance de Sane Break est en cours d’exécution.</translation>
    </message>
    <message>
        <location filename="../../src/app/main.cpp" line="56"/>
        <source>Please quit the old instance before starting a new one.</source>
        <translation>Merci de quitter l&apos;ancienne instance avant d&apos;en démarrer une nouvelle.</translation>
    </message>
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <source>big break</source>
        <translation type="vanished">grosse pause</translation>
    </message>
    <message>
        <source>small break</source>
        <translation type="vanished">petite pause</translation>
    </message>
    <message>
        <source>Next Break</source>
        <translation type="vanished">Prochaine pause</translation>
    </message>
    <message>
        <source>Big Break</source>
        <translation type="vanished">Grosse pause</translation>
    </message>
    <message>
        <source>Postpone</source>
        <translation type="vanished">Retarder</translation>
    </message>
    <message numerus="yes">
        <source>%n min</source>
        <translation type="vanished">
            <numerusform>%n min</numerusform>
            <numerusform>%n min</numerusform>
        </translation>
    </message>
    <message>
        <source>Quit</source>
        <translation type="vanished">Quitter</translation>
    </message>
    <message>
        <source>Enable Break</source>
        <translation type="vanished">Activer pause</translation>
    </message>
    <message>
        <source>Next break after %1</source>
        <translation type="vanished">Prochaine pause dans %1</translation>
    </message>
    <message>
        <source>Next big break after %1</source>
        <translation type="vanished">Grosse pause dans %1</translation>
    </message>
    <message>
        <source>Enter Reading Mode</source>
        <translation type="vanished">Activer le mode Lecture</translation>
    </message>
    <message>
        <source>Preferences</source>
        <translation type="vanished">Préférences</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="106"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation>Voulez-vous vraiment quitter Sane Break&#x202f;?</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="107"/>
        <source>You can postpone the breaks instead.</source>
        <translation>Alternativement, vous pouvez retarder les pauses.</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="85"/>
        <location filename="../../src/app/app.cpp" line="109"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="81"/>
        <source>Are you sure to postpone for %n minute?</source>
        <translation>
            <numerusform>Voulez-vous vraiment retarder cette de pour %n minute&#x202f;?</numerusform>
            <numerusform>Voulez-vous vraiment retarder cette pause de %n minutes&#x202f;?</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="82"/>
        <source>You haven&apos;t taken breaks for %1 minutes.</source>
        <translation>Vous n&apos;avez pas pris de pause depuis %1 minutes.</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="110"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>Retarder de %n min</numerusform>
            <numerusform>Retarder de %n min</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="86"/>
        <location filename="../../src/app/app.cpp" line="111"/>
        <source>Yes</source>
        <translation>Oui</translation>
    </message>
    <message>
        <source>Paused on battery</source>
        <translation type="vanished">En pause sur batterie</translation>
    </message>
    <message>
        <source>Paused on app running</source>
        <translation type="vanished">En pause lorsqu&apos;une application est lancée</translation>
    </message>
    <message>
        <source>Paused on idle</source>
        <translation type="vanished">En pause en cas d&apos;inactivité</translation>
    </message>
    <message>
        <source>Exit Reading Mode</source>
        <translation type="vanished">Quitter le mode Lecture</translation>
    </message>
</context>
<context>
    <name>SanePreferences</name>
    <message>
        <location filename="../../src/core/preferences.cpp" line="49"/>
        <source>Time for a small break</source>
        <translation>Il est temps de faire une petite pause</translation>
    </message>
    <message>
        <location filename="../../src/core/preferences.cpp" line="52"/>
        <source>Time for a big break</source>
        <translation>Il est temps de faire une grosse pause</translation>
    </message>
</context>
<context>
    <name>StatusTrayWindow</name>
    <message>
        <location filename="../../src/app/tray.cpp" line="52"/>
        <source>Postpone</source>
        <translation>Retarder</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/tray.cpp" line="55"/>
        <location filename="../../src/app/tray.cpp" line="141"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n min</numerusform>
            <numerusform>%n min</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="58"/>
        <source>Enable Break</source>
        <translation>Activer pause</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="65"/>
        <source>Preferences</source>
        <translation>Préférences</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="68"/>
        <source>Quit</source>
        <translation>Quitter</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="94"/>
        <source>big break</source>
        <translation>grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="94"/>
        <source>small break</source>
        <translation>petite pause</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="74"/>
        <source>Next break after %1</source>
        <translation>Prochaine pause dans %1</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="45"/>
        <source>Take a small break instead</source>
        <translation>Prendre une petite pause à la place</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="76"/>
        <source>Next big break after %1</source>
        <translation>Grosse pause dans %1</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="86"/>
        <source>Paused on battery</source>
        <translation>En pause sur batterie</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="88"/>
        <source>Paused on app running</source>
        <translation>En pause lorsqu&apos;une application est lancée</translation>
    </message>
    <message>
        <location filename="../../src/app/tray.cpp" line="90"/>
        <source>Paused on idle</source>
        <translation>En pause en cas d&apos;inactivité</translation>
    </message>
</context>
<context>
    <name>WelcomeWindow</name>
    <message>
        <location filename="../../src/app/welcome.cpp" line="116"/>
        <source>Welcome to Sane Break</source>
        <translation>Bienvenue dans Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="117"/>
        <source>&lt;h3 align=center&gt;Welcome to Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow. Sane Break will stay in the system tray and remind you to take breaks at regular intervals. To quit, go to &quot;Postpone&quot; in the tray menu.&lt;/p&gt;</source>
        <translation>&lt;h3 align=center&gt;Bienvenue dans Sane Break&#x202f;!&lt;/h3&gt;&lt;p&gt;Sane Break est un logiciel trans-plateformes de rappel de pauses qui vous aide à prendre des pauses utiles sans perturber vos habitudes de travail. Sane Break se logera dans votre barre des tâches et vous rappellera de prendre des pauses à intervalle régulier. Pour quitter l&apos;appli, ouvrez le menu de l&apos;icône dans la barre des tâches et allez dans la section &quot;Retarder&quot;.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="96"/>
        <location filename="../../src/app/welcome.cpp" line="126"/>
        <source>Read More</source>
        <translation>En savoir plus</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="98"/>
        <location filename="../../src/app/welcome.cpp" line="127"/>
        <source>Ignore</source>
        <translation>Ignorer</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="99"/>
        <location filename="../../src/app/welcome.cpp" line="128"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="102"/>
        <location filename="../../src/app/welcome.cpp" line="129"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="../../src/app/welcome.cpp" line="124"/>
        <source>Language</source>
        <translation>Langue</translation>
    </message>
</context>
</TS>
