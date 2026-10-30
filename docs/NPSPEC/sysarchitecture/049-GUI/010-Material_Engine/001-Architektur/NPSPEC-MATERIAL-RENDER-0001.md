# NPSPEC-MATERIAL-RENDER-0001 – Nova Material Rendering Pipeline

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Die Nova Material Rendering Pipeline definiert die Verarbeitung und Darstellung visueller Oberflächenmaterialien innerhalb von NovaOS.

Sie übersetzt deklarative Materialdefinitionen in ausführbare Rendering-Operationen und integriert diese in die bestehende Grafikpipeline.

Ziel ist eine einheitliche, effiziente und hardwareunabhängige Darstellung aller NovaOS-Materialien.

## 2. Architektur

Die Material Rendering Pipeline ist Bestandteil der Nova Material Engine und verwendet die vorhandene Nova Graphics Engine.

Sie besteht aus folgenden Verarbeitungsstufen:

1. **Material Resolution:** Ermittlung der Materialdefinition und ihrer aktuellen Eigenschaften.
2. **State Evaluation:** Auswertung von Zuständen und animierten Eigenschaften.
3. **Effect Preparation:** Bestimmung der benötigten Effekte und Ressourcen.
4. **Backdrop Acquisition:** Bereitstellung erforderlicher Hintergrundinformationen.
5. **Effect Processing:** Berechnung von Unschärfe, Lichtbrechung, Reflexionen und weiteren Effekten.
6. **Material Composition:** Zusammenführung der Effekte zum endgültigen Materialergebnis.
7. **Output Submission:** Übergabe des Ergebnisses an die bestehende Compositor-Pipeline.

Die Verarbeitung erfolgt ausschließlich für sichtbare und tatsächlich benötigte Materialbereiche.

## 3. Rendering-Modell

Materialien werden über ein gemeinsames Rendering-Modell verarbeitet.

Verbindliche Anforderungen:

- Materialdefinitionen bleiben unabhängig von Grafik-APIs und Hardware.
- Materialeigenschaften werden in Rendering-Parameter übersetzt.
- Effekte können einzeln oder kombiniert ausgeführt werden.
- Die Reihenfolge abhängiger Effekte muss eindeutig definiert sein.
- Identische Effekte und Ressourcen können gemeinsam genutzt werden.
- Materialinstanzen dürfen unterschiedliche Eigenschaften derselben Definition verwenden.

Die Pipeline muss sowohl statische als auch dynamische Materialien unterstützen.

## 4. Effektverarbeitung

Die Pipeline unterstützt folgende grundlegende Effektkategorien:

- **Color Processing:** Grundfarbe, Farbüberlagerung und Deckkraft.
- **Backdrop Blur:** Unschärfe des Hintergrundes.
- **Refraction:** Optische Lichtbrechung.
- **Reflection:** Oberflächenreflexionen.
- **Specular Highlights:** Glanzlichter und Kantenreflexionen.
- **Distortion:** Räumliche Verzerrung der Materialoberfläche.
- **Surface Processing:** Oberflächenstruktur und Rauheit.
- **Lighting:** Dynamische Beleuchtung.

Effekte werden nur ausgeführt, wenn sie durch das Material benötigt werden.

Nicht unterstützte Effekte müssen kontrolliert deaktiviert oder durch geeignete Alternativen ersetzt werden.

## 5. Hintergrundverarbeitung

Hintergrundabhängige Materialien benötigen Zugriff auf bereits komponierte Bildinhalte.

Anforderungen:

- Der Compositor stellt die benötigten Hintergrundbereiche bereit.
- Hintergrunddaten müssen dem korrekten Ebenen- und Darstellungszustand entsprechen.
- Materialeffekte dürfen keine unzulässigen Bildinhalte anderer Oberflächen auslesen.
- Änderungen des Hintergrundes müssen betroffene Materialbereiche invalidieren.
- Unveränderte Hintergrundinformationen dürfen wiederverwendet werden.
- Effekte mit erweitertem Abtastbereich müssen diesen bei der Damage-Berechnung berücksichtigen.

Zyklische Abhängigkeiten zwischen Hintergrundabtastung und Materialdarstellung müssen ausgeschlossen werden.

## 6. Materialkomposition

Die berechneten Effekte werden zu einem endgültigen Materialergebnis zusammengeführt.

Dabei gelten folgende Anforderungen:

- Transparenz und Alpha-Komposition müssen korrekt verarbeitet werden.
- Die Farbraumverarbeitung muss mit der Nova Graphics Engine kompatibel sein.
- Materialeffekte müssen definierte Clipping- und Maskierungsbereiche berücksichtigen.
- Überlagernde Materialien müssen in der korrekten Ebenenreihenfolge verarbeitet werden.
- Das Ergebnis muss unabhängig von der Anzahl verwendeter Materialinstanzen konsistent bleiben.

Die endgültige Komposition erfolgt über die bestehende Compositor-Infrastruktur.

## 7. Dynamische Materialien

Die Pipeline unterstützt Änderungen von Materialeigenschaften zur Laufzeit.

Dazu gehören:

- Animierte Transparenz und Unschärfe
- Dynamische Lichtreflexionen
- Flüssige Oberflächenverformungen
- Interaktionsabhängige Materialeffekte
- Übergänge zwischen Materialzuständen

Animationswerte werden durch das bestehende NovaOS-Animationssystem bereitgestellt.

Die Pipeline berechnet ausschließlich die tatsächlich veränderten Materialzustände neu.

## 8. Damage Tracking

Die Material Rendering Pipeline muss vollständig mit dem Damage Tracking des Nova Compositors zusammenarbeiten.

Verbindliche Anforderungen:

- Materialänderungen erzeugen gezielte Damage-Regionen.
- Hintergrundänderungen invalidieren abhängige Materialbereiche.
- Effektbedingte Abtastradien werden bei der Invalidierung berücksichtigt.
- Unveränderte Materialergebnisse können aus dem Cache übernommen werden.
- Vollständiges Neuzeichnen darf nur erfolgen, wenn eine partielle Aktualisierung nicht möglich oder unwirtschaftlich ist.

Eine permanente vollständige Neuberechnung des Bildschirms ist nicht zulässig.

## 9. Hardwarebeschleunigung

Die Pipeline verwendet die vorhandenen Rendering-Backends der Nova Graphics Engine.

Anforderungen:

- GPU-Beschleunigung wird bevorzugt, sofern verfügbar.
- Ein CPU-Fallback muss grundlegende Materialdarstellungen ermöglichen.
- Hardwareabhängige Optimierungen bleiben innerhalb des jeweiligen Rendering-Backends.
- Materialdefinitionen dürfen keine direkte Abhängigkeit von bestimmten GPU-Funktionen besitzen.
- Nicht verfügbare Effekte müssen durch definierte Qualitätsstufen oder Fallbacks behandelt werden.

Ein Wechsel des Rendering-Backends darf keine Änderungen an Materialdefinitionen erfordern.

## 10. Performance

Die Pipeline muss unnötige Berechnungen und Speicheroperationen vermeiden.

Dazu gelten folgende Anforderungen:

- Unsichtbare Materialien werden nicht verarbeitet.
- Unveränderte Materialergebnisse werden wiederverwendet.
- Zwischenresultate können zwischengespeichert werden.
- Kompatible Rendering-Operationen können zusammengefasst werden.
- Temporäre Ressourcen werden nach Möglichkeit wiederverwendet.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.
- Die Effektqualität kann abhängig von Hardwareleistung und Systemauslastung angepasst werden.

Die Optimierungen dürfen keine sichtbaren Darstellungsfehler verursachen.

## 11. Determinismus und Fehlerbehandlung

Bei identischen Eingaben, Materialzuständen und Rendering-Bedingungen muss die Pipeline reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Effektparameter müssen validiert werden.
- Ungültige Materialdefinitionen dürfen die Rendering-Pipeline nicht destabilisieren.
- Fehlerhafte Effekte müssen isoliert behandelt werden.
- Bei Rendering-Fehlern wird ein definiertes Fallback-Material verwendet.
- Ressourcenverluste und ungültige GPU-Zustände müssen kontrolliert behandelt werden.

## 12. Ergebnis

Die Nova Material Rendering Pipeline stellt eine einheitliche Verarbeitung sämtlicher NovaOS-Oberflächenmaterialien bereit.

Sie verbindet deklarative Materialdefinitionen mit der bestehenden Graphics Engine und dem Compositor.

Durch gezielte Effektverarbeitung, Damage Tracking, Caching und Hardwarebeschleunigung ermöglicht sie hochwertige Materialeffekte bei möglichst geringem Ressourcenverbrauch.