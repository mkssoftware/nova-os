# NovaOS Vision

## Zweck dieses Dokuments

Dieses Dokument beschreibt die langfristige Vision von NovaOS. Es dient Codex und anderen Entwicklungstools als Leitlinie dafür, **was NovaOS werden soll**, welche Grundprinzipien gelten und welche Richtung bei Architektur, Oberfläche und Funktionen einzuhalten ist.

NovaOS soll kein weiterer Windows-, Linux- oder macOS-Klon werden. Bewährte Bedienmuster dürfen erhalten bleiben, aber NovaOS soll neue Grundlagen schaffen, wie Menschen mit Computern, Daten, Aufgaben und Software arbeiten.

---

# 1. Grundidee

NovaOS stellt nicht Programme in den Mittelpunkt, sondern:

- die Aufgabe des Nutzers,
- den aktuellen Kontext,
- die vorhandenen Daten,
- verfügbare Fähigkeiten,
- und das gewünschte Ergebnis.

Der Nutzer soll möglichst wenig darüber nachdenken müssen, **welches Programm** er benötigt oder **welches Dateiformat** gerade verwendet wird.

Stattdessen soll NovaOS verstehen:

> Was möchte der Nutzer erreichen und welche Daten und Fähigkeiten werden dafür benötigt?

---

# 2. Nutzergefühl

NovaOS soll sich:

- warm,
- vertraut,
- ruhig,
- hochwertig,
- ausgeglichen,
- modern,
- aber nicht hektisch

anfühlen.

Das System soll den Nutzer bei seiner Arbeit unterstützen, ohne sich ständig in den Vordergrund zu drängen.

Die Oberfläche soll visuelles Rauschen vermeiden.

Der Nutzer soll das Gefühl haben:

> NovaOS arbeitet mit mir und nicht gegen mich.

---

# 3. Vertraute Bedienung, neue Grundlagen

Grundlegende bekannte Bedienmuster bleiben erhalten, solange sie sinnvoll sind:

- Fenster
- Dateien
- Drag & Drop
- Rechtsklick
- Suche
- Einstellungen
- Taskleiste / Startbereich
- Maus- und Tastaturbedienung

NovaOS darf diese Grundlagen jedoch erweitern oder an geeigneten Stellen neue Bedienprinzipien schaffen.

Ziel ist:

> Neu im Denken, vertraut in der Bedienung.

---

# 4. Kontextbewusste Oberfläche

NovaOS soll erkennen können, woran der Nutzer gerade arbeitet.

Die Oberfläche darf sich dabei dynamisch anpassen, jedoch niemals chaotisch oder unvorhersehbar.

Grundregel:

> Relevantes tritt hervor. Unwichtiges tritt zurück. Nichts Wesentliches verschwindet.

Beispiele:

- aktuell weniger relevante Funktionen können auf etwa 70–80 % Sichtbarkeit reduziert werden,
- nähert sich der Mauszeiger, werden diese Funktionen weich wieder vollständig sichtbar,
- Position und Größe von Bedienelementen bleiben stabil,
- wichtige Funktionen dürfen nicht plötzlich verschwinden oder ihre Position wechseln.

Die Oberfläche soll den Fokus unterstützen, nicht den Nutzer bevormunden.

---

# 5. Unterschied zwischen Relevanz und Disabled

NovaOS unterscheidet klar zwischen:

- **verfügbar, aber momentan weniger relevant**
- **nicht verfügbar / disabled**

Weniger relevante Funktionen werden leicht transparenter dargestellt.

Disabled-Funktionen sollen optisch deutlich anders wirken, beispielsweise durch einen dezenten Blur-Effekt.

Transparenz bedeutet in NovaOS:

> geringere aktuelle Relevanz, nicht fehlende Verfügbarkeit.

---

# 6. Commandbar

NovaOS erhält eine zentrale Commandbar.

Sie verbindet:

- Suche
- Prompt
- Chat
- Befehle
- Aufgabenstellung
- Navigation
- Systemaktionen

Die Commandbar soll systemweit funktionieren.

Beispiele:

- „Öffne die Netzwerkeinstellungen.“
- „Finde die Tabelle von gestern.“
- „Vergleiche diese Daten.“
- „Erstelle aus diesen Informationen ein Diagramm.“
- „Fasse diese Dokumente zusammen.“
- „Organisiere diese Dateien nach Projekt.“

Die Commandbar ist eine zusätzliche Bedienebene und ersetzt nicht zwangsweise die klassische Bedienung.

---

# 7. Aufgabe statt Anwendung

NovaOS soll langfristig stärker auf Aufgaben als auf Anwendungen ausgerichtet sein.

Der Nutzer denkt:

> „Ich möchte dieses Ergebnis erreichen.“

Nicht:

> „Welche App muss ich dafür öffnen?“

Programme bleiben weiterhin möglich und wichtig, aber sie sind nicht mehr die einzige Organisationsform von Software.

---

# 8. Fähigkeiten

Fähigkeiten sind wiederverwendbare Funktionen des Systems.

Beispiele:

- Text verarbeiten
- Bild analysieren
- OCR
- Netzwerkzugriff
- Diagramme erstellen
- Daten konvertieren
- Dateien öffnen
- Inhalte teilen
- Berechnungen durchführen

Fähigkeiten sollen miteinander kombinierbar sein.

Sie bilden einen grundlegenden Baustein von NovaOS.

---

# 9. Solutions

Solutions verbinden:

- Fähigkeiten,
- Benutzeroberfläche,
- Logik,
- Daten,
- und bei Bedarf Agenten.

Eine Solution wird für eine bestimmte Aufgabe oder einen Arbeitsablauf zusammengestellt.

Sie ist nicht zwangsläufig ein klassisches monolithisches Programm.

Solutions sollen es ermöglichen, Arbeitsabläufe flexibel aus vorhandenen Systemfähigkeiten zusammenzusetzen.

---

# 10. Klassische Programme

NovaOS unterstützt weiterhin klassische monolithische Programme.

Programme und Solutions existieren nebeneinander.

Das Ziel ist nicht, klassische Programme künstlich abzuschaffen.

NovaOS soll dem Entwickler und Nutzer die jeweils sinnvollste Form ermöglichen.

---

# 11. Daten statt Dateiformate

NovaOS soll Daten möglichst nach ihrer Bedeutung verstehen und nicht nur anhand ihres Dateiformats.

Daten können beispielsweise stammen aus:

- Tabellen
- Bildern
- PDFs
- Webseiten
- Textdokumenten
- Datenbanken
- Sensoren
- Geräten
- Netzwerkquellen
- klassischen Dateien

NovaOS soll diese Daten systemweit miteinander kombinieren können.

---

# 12. Semantisches Drag & Drop

Drag & Drop soll in NovaOS über das reine Verschieben von Dateien hinausgehen.

NovaOS soll möglichst erkennen, **was** gezogen wird und **wofür** es am Ziel verwendet werden kann.

Beispiel:

1. In einer Tabelle befinden sich Daten über Sonnenstunden.
2. In einem Bild befindet sich eine Tabelle mit CO₂-Werten.
3. Der Nutzer markiert die CO₂-Tabelle im Bild.
4. Er zieht den markierten Bereich auf die Tabelle mit den Sonnenstunden.
5. NovaOS erkennt die Tabelle im Bild.
6. OCR extrahiert die Werte.
7. NovaOS erkennt eine gemeinsame Zeit- oder Datumsachse.
8. Die Daten werden passend zugeordnet.
9. NovaOS bietet eine gemeinsame Darstellung oder ein Diagramm an.

Der Nutzer arbeitet dabei nicht mit Dateiformaten, sondern mit Bedeutung.

Das Ziel lautet:

> Informationen sollen systemweit miteinander interagieren können.

---

# 13. Automatische Konvertierung

Wenn es zum Kontext passt, darf NovaOS Daten automatisch in eine passende Form überführen.

Beispiele:

- Bild → erkannte Tabelle
- Text → strukturierte Daten
- CSV → Diagrammdaten
- Einheit A → Einheit B
- Datumsformat A → Datumsformat B
- Dokumentinhalt → extrahierte Informationen

Automatische Umwandlungen müssen nachvollziehbar sein.

Wichtige Änderungen sollen möglichst:

- sichtbar,
- kontrollierbar,
- bestätigbar,
- und rückgängig machbar

sein.

---

# 14. Systemweites Kontextverständnis

NovaOS soll langfristig Daten- und Arbeitszusammenhänge systemweit erkennen.

Das Konzept darf nicht auf den Browser oder einzelne Programme beschränkt sein.

Die gleiche Interaktionslogik soll grundsätzlich zwischen unterschiedlichen Quellen funktionieren.

Das Betriebssystem selbst soll verstehen können:

- welche Daten vorliegen,
- wie sie zusammenhängen könnten,
- welche Fähigkeiten darauf anwendbar sind,
- und welches Ergebnis der Nutzer wahrscheinlich erreichen möchte.

---

# 15. Agenten

Agenten dürfen den Nutzer bei Aufgaben unterstützen.

Sie sollen jedoch nicht unkontrolliert handeln.

Grundprinzip:

> KI darf planen und verstehen. Die Ausführung bleibt kontrolliert.

Agenten verwenden dieselben kontrollierten Systemfähigkeiten wie andere Teile von NovaOS.

Sie erhalten keine versteckten unbegrenzten Rechte.

---

# 16. KI ist optional

NovaOS darf nicht vollständig von KI abhängig sein.

Zentrale Funktionen müssen auch ohne KI funktionieren.

Wenn KI verfügbar ist, kann sie:

- Kontext besser verstehen,
- Aufgaben interpretieren,
- Vorschläge machen,
- Arbeitsabläufe planen,
- Benutzeroberflächen unterstützen,
- Datenbeziehungen erkennen.

Der grundlegende Systembetrieb bleibt deterministisch möglich.

---

# 17. Generative und dynamische Benutzeroberflächen

NovaOS darf Benutzeroberflächen abhängig von Aufgabe und Kontext erzeugen oder anpassen.

Dabei gelten feste Regeln:

- bestehende Bedienlogik darf nicht chaotisch wechseln,
- wichtige Bedienelemente behalten stabile Positionen,
- dynamische Änderungen erfolgen ruhig und nachvollziehbar,
- größere Änderungen benötigen gegebenenfalls Bestätigung,
- der Nutzer behält jederzeit die Kontrolle.

Die intelligente Oberfläche soll sich um eine stabile Grundoberfläche herum entwickeln.

---

# 18. Sicherheit und Berechtigungen

NovaOS verwendet ein Capability-orientiertes Sicherheitsmodell.

Programme, Solutions, Skripte und Agenten erhalten nur die Fähigkeiten, die sie tatsächlich benötigen.

Berechtigungen sollen:

- nachvollziehbar,
- fein abgestuft,
- kontrollierbar,
- persistent,
- und sicher gebunden

sein.

Sicherheit ist ein Grundbestandteil der Architektur und kein nachträgliches Zusatzmodul.

---

# 19. Self-Healing und Zuverlässigkeit

NovaOS soll Fehler möglichst erkennen und sinnvoll darauf reagieren.

Langfristige Ziele sind unter anderem:

- Integritätsprüfung
- Recovery
- Self-Healing
- Rollback
- Diagnose
- nachvollziehbare Fehlerberichte
- robuste Systemzustände

Das System soll Fehler nicht nur anzeigen, sondern möglichst bei der Wiederherstellung unterstützen.

---

# 20. Designphilosophie

NovaOS soll eine eigene visuelle Identität besitzen.

Die Oberfläche darf modern, futuristisch und glasartig wirken, soll aber nicht kühl oder überladen sein.

Wichtige Eigenschaften:

- ruhige Animationen
- klare Hierarchie
- hochwertige Typografie
- großzügige Abstände
- dezente Transparenz
- Glas-/Acrylic-Effekte
- weiche Übergänge
- wenig visuelles Rauschen
- klare Zustände
- konsistente Interaktionen

Die Designsprache soll langfristig stabil bleiben und nicht jedem kurzfristigen Trend folgen.

---

# 21. Experimentelle Funktionen

NovaOS darf bewusst experimentelle Bedienkonzepte enthalten.

Diese sollen jedoch:

- einen echten Nutzen haben,
- testbar sein,
- bestehende Bedienung nicht unnötig zerstören,
- und bei Erfolg später zu festen NovaOS-Standards werden können.

NovaOS soll neue Wege ausprobieren, ohne Bewährtes grundlos zu verwerfen.

---

# 22. Entwicklungsprinzip

NovaOS ist als langfristiges Lebenswerk gedacht.

Neue Ideen dürfen gesammelt und später umgesetzt werden.

Wichtig ist jedoch:

- Kernbereiche nicht ständig ohne zwingenden Grund neu erfinden,
- notwendige Architekturarbeit sauber abschließen,
- stabile Grundlagen schaffen,
- neue Ideen schrittweise darauf aufbauen,
- langfristige Konsistenz vor kurzfristige Effekte stellen.

---

# 23. Leitfragen für neue Funktionen

Jede größere neue Idee sollte mindestens folgende Fragen beantworten:

1. Unterstützt sie Aufgabe, Kontext oder Daten?
2. Vereinfacht sie den Arbeitsablauf des Nutzers?
3. Passt sie zur ruhigen NovaOS-Bedienphilosophie?
4. Kann sie mit vorhandenen Fähigkeiten kombiniert werden?
5. Bleibt der Nutzer in Kontrolle?
6. Ist sie systemweit sinnvoll oder nur ein isoliertes Feature?
7. Muss dafür wirklich eine bestehende Grundlage geändert werden?
8. Kann sie später zu einem allgemeinen NovaOS-Prinzip werden?

---

# 24. Langfristiges Ziel

NovaOS soll nicht nur ein anderes Betriebssystem sein.

Es soll langfristig eine andere Art schaffen, mit Computern zu arbeiten.

Der Nutzer soll sich weniger mit:

- Programmen,
- Dateiformaten,
- Konvertierungen,
- technischen Grenzen,
- und ständigen App-Wechseln

beschäftigen müssen.

Stattdessen soll er mit:

- Aufgaben,
- Informationen,
- Daten,
- Zusammenhängen,
- und Ergebnissen

arbeiten.

Die langfristige Vision lautet:

> NovaOS versteht nicht nur Programme und Dateien, sondern den Zusammenhang zwischen Aufgabe, Kontext und Daten und hilft dem Nutzer aktiv dabei, daraus ein Ergebnis zu erzeugen.
