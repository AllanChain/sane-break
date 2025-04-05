<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR" sourcelanguage="en_US">
<context>
    <name>AutoStart</name>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="51"/>
        <source>Failed to connect to Flatpak response</source>
        <translation>Échec lors de la connexion à la réponse Flatpak</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="73"/>
        <location filename="../../src/lib/linux/auto-start.cpp" line="79"/>
        <source>File not writable</source>
        <translation>Fichier inaccessible à l&apos;écriture</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="96"/>
        <source>The request to autostart was cancelled.</source>
        <translation>La requête de démarrage automatique a été annulée.</translation>
    </message>
    <message>
        <location filename="../../src/lib/windows/auto-start.cpp" line="27"/>
        <source>Permission denied</source>
        <translation>Permission refusée</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/auto-start.cpp" line="42"/>
        <location filename="../../src/lib/windows/auto-start.cpp" line="30"/>
        <source>Unknown error</source>
        <translation>Erreur inconnue</translation>
    </message>
</context>
<context>
    <name>BreakWindow</name>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="84"/>
        <source>Time for a big break</source>
        <translation>Il est temps de faire une grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/break-window.cpp" line="85"/>
        <source>Time for a small break</source>
        <translation>Il est temps de faire une petite pause</translation>
    </message>
</context>
<context>
    <name>LanguageSelect</name>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="36"/>
        <location filename="../../src/gui/widgets/language-select.cpp" line="59"/>
        <source>Default</source>
        <translation>Défaut</translation>
    </message>
    <message>
        <location filename="../../src/gui/widgets/language-select.cpp" line="47"/>
        <source>English</source>
        <comment>current language</comment>
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
        <location filename="../../src/gui/pref-window.ui" line="110"/>
        <source>Pause</source>
        <translation>Pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="126"/>
        <source>Sound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="145"/>
        <source>General</source>
        <translation>Général</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="167"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="344"/>
        <source>Length of work time between breaks</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (small break)</extracomment>
        <translation>Durée du temps de travail entre chaque pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="347"/>
        <location filename="../../src/gui/pref-window.ui" line="378"/>
        <source>Break every</source>
        <translation>Faire une pause toutes les</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="375"/>
        <source>Every so many breaks will be a big break</source>
        <extracomment>Tooltip for &quot;Break every&quot; config (big break)</extracomment>
        <translation>La dernière de chaque série sera une grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="457"/>
        <source>&lt;h3&gt;Postpone&lt;/h3&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="498"/>
        <source>&lt;h3&gt;Reminder Behavior&lt;/h3&gt;
&lt;p&gt;Sane Break shows a small prompt for &lt;b&gt;Flash break prompt for&lt;/b&gt; seconds. If you stop working or ignore it past this time, it transitions to full-screen. After &lt;b&gt;Confirm break after&lt;/b&gt; seconds of inactivity during the break, it locks until the timer ends.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="552"/>
        <source>Flash break prompt for</source>
        <translation>Faire clignoter le message de pause pendant</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="559"/>
        <source>After idle for that long during break, Sane Break will keep full screen and there&apos;s no way back.</source>
        <extracomment>Tooltip for &quot;Confirm break after&quot; config</extracomment>
        <translation>Si inactif pendant ce temps lors d&apos;une pause, Sane Break restera en plein écran sans retour arrière possible.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="562"/>
        <source>Confirm break after</source>
        <translation>Confirmer le début de la pause après</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="659"/>
        <source>Flash speed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="750"/>
        <source>&lt;h3&gt;Pause Conditions&lt;/h3&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="836"/>
        <source>&lt;h3&gt;Reset Break Schedule After Pause&lt;/h3&gt;
&lt;p&gt;Sane Break will adjust the break schedule after pausing:&lt;/p&gt;
&lt;ul&gt;
  &lt;li&gt;If the pause is short, Sane Break will reset the time to the next break.&lt;/li&gt;
  &lt;li&gt;If it is longer, Sane Break will reset the entire break cycle, and the next break will be the first small break in the cycle.&lt;/li&gt;
&lt;/ul&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="891"/>
        <source>Long pause threshold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="939"/>
        <source>Short pause threshold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1154"/>
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
        <location filename="../../src/gui/pref-window.ui" line="1159"/>
        <source>Automatic screen lock after break start</source>
        <translation>Verrouillage automatique de l&apos;écran après le début de la pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1179"/>
        <source>Please grant accessibility permission and restart app to use auto lock screen</source>
        <translation>Merci d&apos;octroyer les droits d&apos;accès et de redémarrer l&apos;appli pour utiliser le verrouillage d&apos;écran</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1312"/>
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
        <location filename="../../src/gui/pref-window.ui" line="296"/>
        <source>Duration of small breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (small break)</extracomment>
        <translation>Combien de secondes pour chaque petite pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="549"/>
        <source>Time in seconds before forcing break after work reminder appears.</source>
        <extracomment>Tooltip for &quot;Flash break prompt for&quot; config</extracomment>
        <translation>Combien de secondes pour continuer le travail avant que la pause ne soit forcée.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="445"/>
        <source>Duration of big breaks (in seconds)</source>
        <extracomment>Tooltip for &quot;Break length&quot; config (big break)</extracomment>
        <translation>Combien de secondes pour chaque grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="482"/>
        <source>Postpone minutes</source>
        <translation>Minutes de retard</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="606"/>
        <source>&lt;h3&gt;Prompt Appearance&lt;/h3&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="623"/>
        <source>Off</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="630"/>
        <source>Slower</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="637"/>
        <source>Default</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="unfinished">Défaut</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="647"/>
        <source>Faster</source>
        <extracomment>Tick for flash speed</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="690"/>
        <source>&lt;h3&gt;Break Window Appearance&lt;/h3&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="734"/>
        <source>Countdown Transparency</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="964"/>
        <location filename="../../src/gui/pref-window.ui" line="995"/>
        <source>Try restart Sane Break if no sound</source>
        <translation>Si le son ne fonctionne pas, essayez de redémarrer Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="981"/>
        <location filename="../../src/gui/pref-window.ui" line="988"/>
        <location filename="../../src/gui/pref-window.ui" line="1043"/>
        <location filename="../../src/gui/pref-window.ui" line="1050"/>
        <source>Play</source>
        <translation>Écouter</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="75"/>
        <source>Schedule</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="94"/>
        <source>Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Reset break after paused for</source>
        <translation type="vanished">Réinitialiser ce cycle s&apos;il est en pause depuis</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="759"/>
        <source>Sane Break will pause if it detects no activity for this many minutes</source>
        <extracomment>Tooltip for &quot;Pause if idle for&quot; config</extracomment>
        <translation>Sane Break mettra son cycle en pause s&apos;il ne détecte aucune activité sur cette période</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="762"/>
        <source>Pause if idle for</source>
        <translation>Mettre le cycle en pause si inactif depuis</translation>
    </message>
    <message>
        <source>Reset cycle after paused for</source>
        <translation type="vanished">Réinitialiser tout le cycle s&apos;il est en pause depuis</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="252"/>
        <location filename="../../src/gui/pref-window.ui" line="955"/>
        <source>&lt;h3&gt;Small Breaks&lt;/h3&gt;</source>
        <translation type="unfinished">&lt;h3&gt;Petite pause&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="306"/>
        <location filename="../../src/gui/pref-window.ui" line="385"/>
        <location filename="../../src/gui/pref-window.ui" line="803"/>
        <location filename="../../src/gui/pref-window.ui" line="881"/>
        <location filename="../../src/gui/pref-window.ui" line="929"/>
        <source> min</source>
        <translation> min</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="998"/>
        <location filename="../../src/gui/pref-window.ui" line="1036"/>
        <source>Bell before breaks</source>
        <translation type="unfinished">Son de cloche avant une pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="261"/>
        <location filename="../../src/gui/pref-window.ui" line="514"/>
        <location filename="../../src/gui/pref-window.ui" line="594"/>
        <source> sec</source>
        <translation> sec</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="299"/>
        <location filename="../../src/gui/pref-window.ui" line="448"/>
        <source>Break length</source>
        <translation type="unfinished">Durée d&apos;une pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="967"/>
        <location filename="../../src/gui/pref-window.ui" line="1064"/>
        <source>Bell after breaks</source>
        <translation type="unfinished">Son de cloche après une pause</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1076"/>
        <source>&lt;strong&gt;Tip&lt;/strong&gt;: You can use local sound files with URLs like &lt;code&gt;file:/D:sound.mp3&lt;/code&gt;  (Windows) or &lt;code&gt;file:/home/name/sound.mp3&lt;/code&gt; (Linux).</source>
        <translation>&lt;strong&gt;Astuce&lt;/strong&gt; : Vous pouvez utiliser un fichier son local avec un URL du type  &lt;code&gt;file:/D:\fichierson.mp3&lt;/code&gt;  (Windows) ou &lt;code&gt;file:/home/dossier/fichierson.mp3&lt;/code&gt; (Linux).</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="356"/>
        <location filename="../../src/gui/pref-window.ui" line="1014"/>
        <source>&lt;h3&gt;Big Breaks&lt;/h3&gt;</source>
        <translation type="unfinished">&lt;h3&gt;Grosse pause&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="365"/>
        <source> breaks</source>
        <translation> pauses</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="815"/>
        <source>Pause on battery</source>
        <translation>Mettre en pause sur batterie</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="822"/>
        <source>Pause breaks when any of the following program is running:</source>
        <translation>La pause s&apos;arrête dès que l&apos;un de ces programmes se met en route&#xa0;:</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="829"/>
        <source>(Write one program name per line)</source>
        <translation>(écrire un seul nom de programme par ligne)</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1102"/>
        <source>Configuration file</source>
        <translation>Fichier de configuration</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1132"/>
        <source>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Help translate!&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://hosted.weblate.org/engage/sane-break/&quot;&gt;Aidez-nous à faire la traduction&#x202f;!&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1194"/>
        <source>Launch Sane Break at start up</source>
        <translation type="unfinished">Activer le démarrage automatique</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1393"/>
        <source>Revert to Last Saved</source>
        <translation>Revenir aux anciens paramètres sauvegardés</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1095"/>
        <source>Language</source>
        <translation>Langue</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1119"/>
        <source>Change of language will take effect after app restart.</source>
        <translation>Le changement de langue prendra effet après redémarrage de l&apos;appli.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.ui" line="1416"/>
        <source>Save</source>
        <translation>Sauvegarder</translation>
    </message>
</context>
<context>
    <name>PreferenceWindow</name>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="265"/>
        <location filename="../../src/gui/pref-window.cpp" line="266"/>
        <location filename="../../src/gui/pref-window.cpp" line="267"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n min</numerusform>
            <numerusform>%n min</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="140"/>
        <location filename="../../src/gui/pref-window.cpp" line="149"/>
        <source>Every %n min</source>
        <translation>
            <numerusform>Chaque minute</numerusform>
            <numerusform>Toutes les %n minutes</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/gui/pref-window.cpp" line="264"/>
        <source>%n sec</source>
        <translation>
            <numerusform>%n sec</numerusform>
            <numerusform>%n sec</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="263"/>
        <source>Disabled</source>
        <translation>Désactivé</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="274"/>
        <source>Start next break after middle clicking on tray icon</source>
        <translation>Démarrer la pause suivante en faisant clic du milieu sur l&amp;apos;icône dans la barre des tâches</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="280"/>
        <source>Start next break after double clicking on tray icon</source>
        <translation>Démarrer la pause suivante en faisant double-clic sur l&amp;apos;icône dans la barre des tâches</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="348"/>
        <source>Save</source>
        <translation type="unfinished">Sauvegarder</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="349"/>
        <source>Discard</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="350"/>
        <source>Cancel</source>
        <translation type="unfinished">Annuler</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="249"/>
        <source>Setting autostart failed</source>
        <translation>La mise en place du démarrage automatique a échoué</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="345"/>
        <source>The preferences have been modified.</source>
        <translation>Les préférences ont été modifiées.</translation>
    </message>
    <message>
        <location filename="../../src/gui/pref-window.cpp" line="346"/>
        <source>Do you want to save your changes?</source>
        <translation>Voulez-vous sauvegarder les changements&#x202f;?</translation>
    </message>
</context>
<context>
    <name>QCoreApplication</name>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="86"/>
        <source>Tray icon is not available, falling back to a normal window</source>
        <translation>Icône de la barre des tâches indisponible, retour à la fenêtre normale</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="92"/>
        <source>The compositor does not support layer shell, and window positioning will be bugged</source>
        <translation>Le compositeur ne supporte pas le &quot;layer shell&quot;, et la position de la fenêtre sera incorrecte</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="96"/>
        <source>Sane Break is not compiled with layer shell support, and window positioning will be bugged</source>
        <translation>Sane Break n&apos;est pas compilé avec support du &quot;layer shell&quot;, et la position de la fenêtre sera incorrecte</translation>
    </message>
    <message>
        <location filename="../../src/lib/linux/system-check.cpp" line="100"/>
        <source>You need to install the shell extension if you are using GNOME. Otherwise, Sane break is almost unusable</source>
        <translation>Si vous utilisez GNOME, vous devrez installer l&apos;extension &quot;shell&quot;, ou Sane Break sera presque inutilisable</translation>
    </message>
</context>
<context>
    <name>SaneBreakApp</name>
    <message>
        <location filename="../../src/app/app.cpp" line="141"/>
        <source>big break</source>
        <translation>grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="141"/>
        <source>small break</source>
        <translation>petite pause</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="153"/>
        <source>Next Break</source>
        <translation>Prochaine pause</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="157"/>
        <source>Big Break</source>
        <translation>Grosse pause</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="166"/>
        <source>Postpone</source>
        <translation>Retarder</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="169"/>
        <location filename="../../src/app/app.cpp" line="332"/>
        <source>%n min</source>
        <translation>
            <numerusform>%n min</numerusform>
            <numerusform>%n min</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="186"/>
        <source>Quit</source>
        <translation>Quitter</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="172"/>
        <source>Enable Break</source>
        <translation>Activer pause</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="144"/>
        <source>Next break after %1</source>
        <translation>Prochaine pause dans %1</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="148"/>
        <source>Next big break after %1</source>
        <translation>Grosse pause dans %1</translation>
    </message>
    <message>
        <source>Enter Reading Mode</source>
        <translation type="vanished">Activer le mode Lecture</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="180"/>
        <source>Preferences</source>
        <translation>Préférences</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="344"/>
        <source>Are you sure to quit Sane Break?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="345"/>
        <source>You can postpone the breaks instead.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="347"/>
        <source>Cancel</source>
        <translation type="unfinished">Annuler</translation>
    </message>
    <message numerus="yes">
        <location filename="../../src/app/app.cpp" line="348"/>
        <source>Postpone %n min</source>
        <translation>
            <numerusform>Retarder de %n min</numerusform>
            <numerusform>Retarder de %n min</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="349"/>
        <source>Yes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="218"/>
        <source>Paused on battery</source>
        <translation>En pause sur batterie</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="220"/>
        <source>Paused on app running</source>
        <translation>En pause lorsqu&apos;une application est lancée</translation>
    </message>
    <message>
        <location filename="../../src/app/app.cpp" line="222"/>
        <source>Paused on idle</source>
        <translation>En pause en cas d&apos;inactivité</translation>
    </message>
    <message>
        <source>Exit Reading Mode</source>
        <translation type="vanished">Quitter le mode Lecture</translation>
    </message>
</context>
<context>
    <name>WelcomeWindow</name>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="114"/>
        <source>Welcome to Sane Break</source>
        <translation>Bienvenue dans Sane Break</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="115"/>
        <source>&lt;h3 align=center&gt;Welcome to Sane Break!&lt;/h3&gt;&lt;p&gt;Sane Break is a cross-platform break reminder designed to help you take meaningful breaks without disrupting your workflow. Sane Break will stay in the system tray and remind you to take breaks at regular intervals. To quit, go to &quot;Postpone&quot; in the tray menu.&lt;/p&gt;</source>
        <translation>&lt;h3 align=center&gt;Bienvenue dans Sane Break&#x202f;!&lt;/h3&gt;&lt;p&gt;Sane Break est un logiciel trans-plateformes de rappel de pauses qui vous aide à prendre des pauses utiles sans perturber vos habitudes de travail. Sane Break se logera dans votre barre des tâches et vous rappellera de prendre des pauses à intervalle régulier. Pour quitter l&apos;appli, ouvrez le menu de l&apos;icône dans la barre des tâches et allez dans la section &quot;Retarder&quot;.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="94"/>
        <location filename="../../src/gui/welcome.cpp" line="124"/>
        <source>Read More</source>
        <translation>En savoir plus</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="96"/>
        <location filename="../../src/gui/welcome.cpp" line="125"/>
        <source>Ignore</source>
        <translation>Ignorer</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="97"/>
        <location filename="../../src/gui/welcome.cpp" line="126"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="100"/>
        <location filename="../../src/gui/welcome.cpp" line="127"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="../../src/gui/welcome.cpp" line="122"/>
        <source>Language</source>
        <translation>Langue</translation>
    </message>
</context>
</TS>
