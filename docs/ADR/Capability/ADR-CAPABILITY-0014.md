# ADR-CAPABILITY-0014: Die grundlegende Benutzeroberfläche bleibt stabil, während Inhalte kontextuell wechseln

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** User Experience / Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0007, ADR-CAPABILITY-0012, ADR-CAPABILITY-0013
- **Folgedokumente:** NPSPEC-SHELL-0001, NPSPEC-UI-CONTEXT-0001, NPSPEC-SOLUTION-0001, NPSPEC-INTENT-0001

---

# 1. Kontext

Traditionelle Desktop-Betriebssysteme bestehen aus zahlreichen Anwendungen, Fenstern und Menüs.

Der Benutzer muss ständig zwischen unterschiedlichen Oberflächen wechseln.

Dadurch entstehen:

- Medienbrüche,
- Orientierungsverlust,
- unterschiedliche Bedienkonzepte,
- redundante Navigation,
- unnötige Fensterverwaltung.

NovaOS verfolgt einen anderen Ansatz.

Die grundlegende Benutzeroberfläche bleibt dauerhaft erhalten.

Nicht die Oberfläche wechselt, sondern ausschließlich der dargestellte Arbeitskontext.

Dadurch entsteht eine konsistente und ruhige Benutzererfahrung.

---

# 2. Problemstellung

Ein häufig wechselndes Benutzerinterface verursacht zahlreiche Nachteile.

## 2.1 Orientierungsverlust

Jede Anwendung besitzt:

- eigene Menüs,
- eigene Symbolleisten,
- eigene Dialoge,
- eigene Tastenkombinationen.

Der Benutzer muss sich ständig neu orientieren.

---

## 2.2 Unterbrechung des Arbeitsflusses

Das Wechseln zwischen Anwendungen unterbricht den eigentlichen Arbeitsprozess.

---

## 2.3 Hohe Lernkurve

Jede Anwendung besitzt ein eigenes Bedienkonzept.

---

## 2.4 Inkonsistente Benutzererfahrung

Selbst ähnliche Funktionen sehen häufig unterschiedlich aus.

---

## 2.5 Erschwerte KI-Unterstützung

Eine KI muss zahlreiche unterschiedliche Oberflächen interpretieren.

Eine einheitliche Plattform vereinfacht dagegen Planung und Automatisierung.

---

# 3. Entscheidung

NovaOS definiert eine dauerhaft stabile Grundoberfläche.

Diese umfasst ausschließlich dauerhaft vorhandene Systemelemente.

Der sichtbare Arbeitsbereich wird dagegen dynamisch entsprechend:

- der Benutzerabsicht,
- der aktuellen Aufgabe,
- der aktiven Pipeline,
- des Kontextes,
- der ausgewählten Solution

aktualisiert.

Der Benutzer arbeitet somit stets innerhalb derselben Oberfläche.

---

# 4. Definition der Grundoberfläche

Die Grundoberfläche beschreibt die dauerhaft vorhandene Systemstruktur.

Hierzu gehören beispielsweise:

- Desktop Shell,
- Taskleiste,
- Systemleiste,
- Benachrichtigungen,
- Suchfunktion,
- Fensterverwaltung,
- Navigation,
- Assistent,
- Einstellungen.

Diese Elemente verändern ihre grundsätzliche Struktur nicht.

---

# 5. Definition des Kontextes

Ein Kontext beschreibt den aktuell bearbeiteten Arbeitsbereich.

Beispiele:

- Dokument bearbeiten,
- Bilder organisieren,
- Musik schneiden,
- CAD-Modell erstellen,
- Daten analysieren,
- Software entwickeln.

Der Kontext bestimmt ausschließlich die sichtbaren Inhalte.

---

# 6. Grundprinzipien

## 6.1 Die Grundoberfläche bleibt stabil

Die grundlegende Navigation verändert sich nicht.

Der Benutzer findet zentrale Funktionen immer an derselben Position.

---

## 6.2 Inhalte wechseln kontextabhängig

Der Arbeitsbereich passt sich automatisch an.

Beispiel:

```text
Desktop Shell

↓

Dokument bearbeiten
```

Später:

```text
Desktop Shell

↓

Bildbearbeitung
```

Die Shell bleibt unverändert.

---

## 6.3 Benutzerabsichten bestimmen den Kontext

Der aktive Kontext entsteht aus:

- Benutzeraktionen,
- geöffneten Pipelines,
- Solutions,
- Fähigkeiten,
- KI-Planung.

Nicht aus einer gestarteten Anwendung.

---

## 6.4 Fähigkeiten liefern Inhalte

Die dargestellten Inhalte stammen aus Fähigkeiten.

Die Shell selbst enthält keine fachliche Logik.

---

## 6.5 Lösungen definieren Arbeitsbereiche

Eine Solution beschreibt:

- Layout,
- Navigation,
- bevorzugte Fähigkeiten,
- gespeicherte Ansichten,
- Arbeitsabläufe.

Die zugrunde liegenden Fähigkeiten bleiben austauschbar.

---

## 6.6 Kontextwechsel erfolgen fließend

Ein Wechsel zwischen Arbeitskontexten soll möglichst ohne sichtbaren Bruch erfolgen.

Beispiele:

```text
Animationen

Übergänge

Sanfte Layoutanpassungen

Persistente Navigation
```

Dadurch bleibt die räumliche Orientierung erhalten.

---

## 6.7 Navigation bleibt konsistent

Elemente wie:

- Zurück,
- Vorwärts,
- Suche,
- Teilen,
- Benachrichtigungen,
- Assistent,
- Einstellungen

befinden sich unabhängig vom aktuellen Kontext an derselben Stelle.

---

## 6.8 Mehrere Kontexte können gleichzeitig existieren

Ein Benutzer kann parallel mehrere Kontexte geöffnet haben.

Beispiel:

```text
Projekt A

Projekt B

Chat

Terminal
```

Jeder Kontext besitzt seinen eigenen Zustand.

Die Grundoberfläche bleibt unverändert.

---

## 6.9 Der Kontext besitzt einen eigenen Zustand

Gespeichert werden beispielsweise:

- geöffnete Dokumente,
- Auswahl,
- Zoom,
- Filter,
- Positionen,
- aktive Pipeline.

Beim erneuten Öffnen wird der Kontext vollständig wiederhergestellt.

---

## 6.10 KI darf Kontexte vorbereiten

Der Nova Assistant darf:

- passende Fähigkeiten auswählen,
- Arbeitsbereiche vorbereiten,
- relevante Informationen anzeigen,
- Layoutvorschläge machen.

Die endgültige Kontrolle verbleibt stets beim Benutzer.

---

# 7. Beispiele

## Dokumentenanalyse

Benutzer:

```text
"Analysiere diesen Vertrag."
```

Darstellung:

```text
Desktop Shell

↓

Dokumentenansicht

↓

Ergebnisse

↓

Zusammenfassung

↓

Empfehlungen
```

Die Grundoberfläche bleibt unverändert.

---

## Bildbearbeitung

Benutzer:

```text
"Optimiere dieses Foto."
```

Darstellung:

```text
Desktop Shell

↓

Bildansicht

↓

Werkzeuge

↓

Historie

↓

Export
```

Keine neue Anwendung wird gestartet.

---

## Softwareentwicklung

Benutzer:

```text
"Öffne das NovaOS-Projekt."
```

Darstellung:

```text
Desktop Shell

↓

Projektansicht

↓

Dateien

↓

Compiler

↓

Terminal

↓

Debugger
```

Die Navigation bleibt identisch.

---

# 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- konsistente Benutzerführung,
- geringere Lernkurve,
- weniger Kontextwechsel,
- bessere Orientierung,
- einheitliches Design,
- einfachere KI-Integration,
- höhere Produktivität,
- geringere kognitive Belastung,
- langfristig stabile Benutzeroberflächen.

---

# 9. Nachteile

Dieses Modell erfordert:

- eine leistungsfähige Shell,
- ein flexibles Layoutsystem,
- kontextabhängige Komponenten,
- Zustandsverwaltung,
- dynamische Navigation.

Diese Komplexität wird bewusst akzeptiert, da sie zentral innerhalb der Plattform umgesetzt wird und die Benutzeroberfläche erheblich vereinfacht. Adaptive Benutzeroberflächen profitieren von einer klaren Modellierung des Nutzungskontextes, wodurch sich Inhalte anpassen lassen, ohne die grundlegende Interaktionsstruktur zu verändern. :contentReference[oaicite:0]{index=0}

---

# 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Nova Shell
- Solution Framework
- Context Manager
- Intent Planner
- Capability Runtime
- Visual Pipeline Editor
- Window Manager
- Notification Center
- Nova Assistant
- Search Engine
- Workspace Manager

Alle diese Komponenten arbeiten mit einer stabilen Grundoberfläche und kontextabhängigen Inhalten.

---

# 11. Implementierungshinweise

Die Nova Shell sollte dauerhaft aktiv bleiben und den zentralen Einstiegspunkt des Systems bilden.

Kontexte werden als eigenständige Objekte verwaltet und können unabhängig geladen, gespeichert und wiederhergestellt werden.

Layoutänderungen sollten animiert erfolgen, um den räumlichen Zusammenhang zwischen verschiedenen Arbeitskontexten zu erhalten.

Die Shell sollte ausschließlich Navigations- und Präsentationsaufgaben übernehmen, während sämtliche Fachlogik durch Capabilities bereitgestellt wird.

---

# 12. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- die grundlegende Benutzeroberfläche dauerhaft konsistent bleibt,
- Arbeitskontexte unabhängig von Anwendungen dargestellt werden,
- Fähigkeiten die dargestellten Inhalte bereitstellen,
- Navigation systemweit konsistent bleibt,
- Kontextzustände gespeichert und wiederhergestellt werden können,
- Benutzerabsichten den dargestellten Kontext beeinflussen,
- Fachlogik vollständig von der Shell getrennt bleibt.

---

# 13. Zusammenfassung

NovaOS trennt konsequent zwischen einer dauerhaft stabilen Benutzeroberfläche und den darin dargestellten Arbeitskontexten.

Die Shell bildet einen konstanten Orientierungspunkt, während sich Inhalte dynamisch an Benutzerabsichten, Fähigkeiten und Solutions anpassen.

Dadurch entsteht eine ruhige, konsistente und aufgabenorientierte Benutzererfahrung, bei der der Wechsel zwischen Tätigkeiten nicht mehr durch unterschiedliche Anwendungen, sondern durch kontextabhängige Inhalte erfolgt. Dieses Architekturprinzip bildet eine wesentliche Grundlage für die langfristige Bedienbarkeit und KI-gestützte Interaktion innerhalb der Nova-Plattform. :contentReference[oaicite:1]{index=1}