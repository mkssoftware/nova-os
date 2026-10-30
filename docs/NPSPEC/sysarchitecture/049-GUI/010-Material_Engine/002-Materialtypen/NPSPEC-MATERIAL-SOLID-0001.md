# NPSPEC-MATERIAL-SOLID-0001 – Nova Solid Material

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Solid definiert ein undurchsichtiges Oberflächenmaterial ohne aufwendige Hintergrund- oder Glaseffekte.

Es ermöglicht eine schnelle, ressourcenschonende und zuverlässige Darstellung von Benutzeroberflächen.

Nova Solid dient gleichzeitig als grundlegendes Fallback-Material der Nova Material Engine.

## 2. Materialeigenschaften

Nova Solid verwendet das gemeinsame Materialmodell der Nova Material Engine.

Unterstützte Eigenschaften:

- **Color:** Grundfarbe des Materials.
- **Tint:** Optionale Farbüberlagerung.
- **Brightness:** Helligkeitsanpassung.
- **Saturation:** Farbsättigung.
- **Gradient:** Optionaler Farbverlauf.
- **BorderColor:** Farbe der Materialbegrenzung.
- **BorderThickness:** Stärke der Materialbegrenzung.

Nicht benötigte Eigenschaften des gemeinsamen Materialmodells werden ignoriert.

Nova Solid besitzt eine feste Deckkraft von 100 %.

## 3. Darstellung

Nova Solid erzeugt eine vollständig undurchsichtige Oberfläche.

Verbindliche Anforderungen:

- Die Darstellung erfolgt ohne Hintergrundabtastung.
- Hintergrundunschärfe, Lichtbrechung und Reflexionen werden nicht berechnet.
- Einfarbige Flächen müssen direkt dargestellt werden können.
- Optionale Farbverläufe werden unterstützt.
- Abgerundete Ecken und individuelle Masken werden unterstützt.
- Materialbegrenzungen dürfen unabhängig konfiguriert werden.
- Die Darstellung muss mit dem bestehenden Clipping-System kompatibel sein.

Nova Solid benötigt keine zusätzlichen Hintergrundinformationen des Compositors.

## 4. Farbverarbeitung

Die Farbverarbeitung verwendet die vorhandenen Funktionen der Nova Graphics Engine.

Anforderungen:

- Farben werden im definierten NovaOS-Farbraum verarbeitet.
- Farbwerte müssen mit dem Nova Theme System kompatibel sein.
- Farbverläufe können linear oder radial definiert werden.
- Helligkeit und Farbsättigung können unabhängig angepasst werden.
- Die Farbdarstellung muss mit dem bestehenden Color Management kompatibel sein.

## 5. Dynamisches Verhalten

Nova Solid unterstützt animierbare Materialeigenschaften.

Dazu gehören:

- Farbänderungen
- Helligkeitsübergänge
- Farbverlaufsänderungen
- Änderungen der Materialbegrenzung
- Übergänge zwischen Materialzuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne Eigenschaftsänderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 6. Integration

Nova Solid kann systemweit verwendet werden.

Typische Einsatzbereiche:

- Fensterhintergründe
- Dialogfenster
- Schaltflächen
- Menüs
- Taskbar und Startmenü
- Eingabefelder
- Systemoberflächen
- Fallback-Darstellungen anderer Materialien

Die Materialzuweisung erfolgt über das Nova Material System und das Nova UI Style System.

## 7. Performance

Nova Solid muss mit minimalem Ressourcenverbrauch dargestellt werden.

Verbindliche Anforderungen:

- Einfarbige Flächen werden über einfache Fülloperationen dargestellt.
- Hintergrundabtastungen sind nicht erforderlich.
- Unveränderte Materialbereiche werden nicht erneut verarbeitet.
- Unsichtbare Materialbereiche werden nicht gerendert.
- Gleichartige Zeichenoperationen können zusammengefasst werden.
- Zusätzliche Rendering-Buffer sollen vermieden werden.
- Die Darstellung muss sowohl GPU- als auch CPU-basiert möglich sein.

Die Verarbeitung muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

## 8. Fallback-Verhalten

Nova Solid stellt das grundlegende Fallback-Material der Nova Material Engine bereit.

Anforderungen:

- Nicht unterstützte Materialeffekte können durch Solid ersetzt werden.
- Bei deaktivierter Transparenz muss eine undurchsichtige Darstellung möglich sein.
- Bei fehlender GPU-Beschleunigung muss Solid weiterhin funktionieren.
- Fehlerhafte Materialdefinitionen dürfen die Darstellung grundlegender UI-Elemente nicht verhindern.
- Fallback-Farben müssen ausreichende Kontraste gewährleisten.

Die Fallback-Darstellung darf keine Hintergrundabtastung oder komplexe Effektverarbeitung voraussetzen.

## 9. Barrierefreiheit

Nova Solid muss eine kontrastreiche und gut lesbare Darstellung ermöglichen.

Anforderungen:

- Systemweite Kontrast- und Farbeinstellungen werden berücksichtigt.
- Texte und Bedienelemente müssen ausreichend unterscheidbar bleiben.
- Dekorative Farbverläufe müssen deaktivierbar sein.
- Die Darstellung muss auch ohne Animationen vollständig nutzbar sein.
- Solid muss als Alternative zu transparenten Materialien verfügbar sein.

## 10. Determinismus und Sicherheit

Nova Solid muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Materialeigenschaften müssen validiert werden.
- Ungültige Farbwerte werden durch definierte Standardwerte ersetzt.
- Fehlerhafte Materialinstanzen dürfen die Rendering-Pipeline nicht destabilisieren.
- Die Darstellung darf keine Sicherheitsgrenzen des Compositors umgehen.
- Solid muss auch bei eingeschränkten Grafikfunktionen zuverlässig verfügbar bleiben.

## 11. Ergebnis

Nova Solid stellt ein einfaches, vollständig undurchsichtiges und ressourcenschonendes Oberflächenmaterial bereit.

Es bildet die Grundlage für klassische Benutzeroberflächen und dient als zuverlässiges Fallback für komplexere Materialien.

Durch die Integration in die Nova Material Engine bleibt die Darstellung einheitlich, anpassbar und unabhängig von aufwendigen Grafikeffekten.