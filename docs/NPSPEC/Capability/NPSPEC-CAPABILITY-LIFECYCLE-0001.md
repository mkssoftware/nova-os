# NPSPEC-CAPABILITY-LIFECYCLE-0001: Installation, Aktivierung, Ausführung und Entfernung

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0005, ADR-CAPABILITY-0006, ADR-CAPABILITY-0009, ADR-CAPABILITY-0010, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001, NPSPEC-CAPABILITY-VERSIONING-0001

---

# 1. Zweck

Diese Spezifikation definiert den vollständigen Lebenszyklus einer Nova Capability.

Sie beschreibt alle Zustände und Übergänge von der Installation bis zur endgültigen Entfernung.

Der Lebenszyklus wird vollständig durch die Nova Runtime verwaltet und gewährleistet einen sicheren, reproduzierbaren und kontrollierten Betrieb jeder Capability.

---

# 2. Ziele

Der Capability-Lebenszyklus verfolgt folgende Ziele:

- reproduzierbare Installation
- kontrollierte Aktivierung
- sichere Ausführung
- deterministische Zustandswechsel
- vollständige Nachvollziehbarkeit
- saubere Deinstallation
- parallele Versionen
- minimale Ausfallzeiten
- langfristige Wartbarkeit

---

# 3. Grundprinzipien

Jede Capability besitzt einen eindeutig definierten Lebenszyklus.

Alle Zustandsänderungen erfolgen ausschließlich über die Runtime.

Direkte Manipulationen einzelner Zustände sind nicht zulässig.

---

# 4. Lebenszyklusübersicht

```text
Nicht installiert

↓

Installiert

↓

Validiert

↓

Registriert

↓

Aktiviert

↓

Geladen

↓

Initialisiert

↓

Bereit

↓

Ausgeführt

↓

Pausiert

↓

Fortgesetzt

↓

Beendet

↓

Entladen

↓

Deaktiviert

↓

Archiviert

↓

Entfernt
```

Jeder Zustand besitzt eine klar definierte Bedeutung.

---

# 5. Zustand: Nicht installiert

Die Capability befindet sich nicht auf dem System.

Eigenschaften:

- nicht vorhanden
- nicht registriert
- nicht ausführbar
- keine Metadaten geladen

---

# 6. Zustand: Installiert

Die Capability wurde auf dem System abgelegt.

Durchzuführende Schritte:

- Dateien kopieren
- Integrität prüfen
- Manifest speichern
- Signatur vorbereiten

Die Capability ist noch nicht ausführbar.

---

# 7. Zustand: Validiert

Vor der Registrierung erfolgt eine vollständige Validierung.

Geprüft werden:

- Descriptor
- Capability Contract
- Version
- Signatur
- Abhängigkeiten
- Datentypen
- Berechtigungen
- Manifest

Nur vollständig validierte Capabilities dürfen registriert werden.

---

# 8. Zustand: Registriert

Die Capability wird in der Capability Registry eingetragen.

Gespeichert werden:

- Identity
- Descriptor
- Contract
- Version
- Metadaten
- Berechtigungen
- Implementierungen

Die Capability ist nun systemweit bekannt.

---

# 9. Zustand: Aktiviert

Die Capability wird für die Nutzung freigegeben.

Während der Aktivierung können:

- Berechtigungen geprüft,
- Ressourcen reserviert,
- Abhängigkeiten geladen,
- Richtlinien angewendet

werden.

Eine deaktivierte Capability bleibt registriert, kann jedoch nicht ausgeführt werden.

---

# 10. Zustand: Geladen

Die Runtime lädt die erforderlichen Bestandteile.

Je nach Implementierung können geladen werden:

- Bibliotheken
- Module
- Modelle
- Ressourcen
- Hardwaretreiber

Die Capability befindet sich nun im Arbeitsspeicher.

---

# 11. Zustand: Initialisiert

Während der Initialisierung werden interne Komponenten vorbereitet.

Beispiele:

- Speicher reservieren
- Geräte öffnen
- Modelle laden
- Zwischenspeicher erzeugen
- Kommunikationskanäle vorbereiten

Nach erfolgreicher Initialisierung ist die Capability betriebsbereit.

---

# 12. Zustand: Bereit

Die Capability wartet auf einen Ausführungsauftrag.

Eigenschaften:

- vollständig initialisiert
- Ressourcen verfügbar
- ausführbar
- keine aktive Verarbeitung

---

# 13. Zustand: Ausgeführt

Die Capability verarbeitet Eingabedaten.

Während der Ausführung übernimmt die Runtime:

- Scheduling
- Ressourcenverwaltung
- Fehlerüberwachung
- Berechtigungsprüfung
- Ereignisverwaltung
- Metrikerfassung

Mehrere Instanzen derselben Capability dürfen parallel ausgeführt werden, sofern dies durch den Descriptor erlaubt ist.

---

# 14. Zustand: Pausiert

Eine laufende Ausführung kann angehalten werden.

Gründe:

- Benutzeraktion
- Energiesparen
- Ressourcenknappheit
- Scheduling
- Debugging

Der interne Zustand bleibt erhalten.

---

# 15. Zustand: Fortgesetzt

Eine pausierte Capability setzt ihre Verarbeitung mit dem zuvor gespeicherten Zustand fort.

Es erfolgt keine erneute Initialisierung.

---

# 16. Zustand: Beendet

Die Verarbeitung ist abgeschlossen.

Mögliche Ergebnisse:

- erfolgreich
- abgebrochen
- fehlerhaft
- zeitüberschritten

Alle Ergebnisse werden an die Pipeline Runtime zurückgegeben.

---

# 17. Zustand: Entladen

Nicht mehr benötigte Ressourcen werden freigegeben.

Beispiele:

- Speicher
- Geräte
- Modelle
- Handles
- Netzwerkverbindungen

Die Registrierung bleibt erhalten.

---

# 18. Zustand: Deaktiviert

Eine Capability bleibt installiert, darf jedoch nicht ausgeführt werden.

Mögliche Gründe:

- Administratorentscheidung
- Lizenz
- Sicherheitsrichtlinie
- Benutzerwunsch
- Wartungsmodus

Alle Metadaten bleiben erhalten.

---

# 19. Zustand: Archiviert

Eine Capability wird nicht mehr aktiv verwendet.

Sie bleibt jedoch:

- reproduzierbar,
- referenzierbar,
- dokumentiert.

Archivierte Capabilities können für ältere Pipelines weiterhin benötigt werden.

---

# 20. Zustand: Entfernt

Die Capability wird vollständig deinstalliert.

Entfernt werden:

- Dateien
- Registry-Einträge
- zwischengespeicherte Daten
- lokale Ressourcen

Historische Provenance-Daten und Audit-Protokolle dürfen erhalten bleiben.

---

# 21. Zustandsübergänge

Zulässige Übergänge:

```text
Installiert

↓

Validiert

↓

Registriert

↓

Aktiviert

↓

Geladen

↓

Initialisiert

↓

Bereit

↓

Ausgeführt

↓

Beendet

↓

Entladen
```

Optionale Übergänge:

```text
Ausgeführt

↓

Pausiert

↓

Fortgesetzt

↓

Beendet
```

Nicht zulässige Übergänge:

```text
Nicht installiert

↓

Ausgeführt
```

oder

```text
Registriert

↓

Ausgeführt
```

Alle Voraussetzungen müssen erfüllt sein.

---

# 22. Fehlerbehandlung

Tritt während eines Zustandswechsels ein Fehler auf, muss die Runtime:

- den Fehler protokollieren,
- den vorherigen konsistenten Zustand wiederherstellen,
- Ressourcen freigeben,
- die Pipeline informieren.

Teilweise abgeschlossene Zustandswechsel sind unzulässig.

---

# 23. Versionen

Mehrere Versionen derselben Capability dürfen gleichzeitig installiert sein.

Beispiel:

```text
nova.image.resize

├── 1.0

├── 1.5

└── 2.0
```

Jede Version besitzt ihren eigenen Lebenszyklus.

---

# 24. Aktualisierung

Eine Aktualisierung erfolgt nicht durch Überschreiben.

Stattdessen:

```text
Version 1.2

↓

Installation Version 1.3

↓

Validierung

↓

Registrierung

↓

Aktivierung

↓

Neue Version verfügbar
```

Erst danach kann die ältere Version deaktiviert oder entfernt werden.

Dieses Vorgehen minimiert Ausfallzeiten und unterstützt reproduzierbare Pipelines.

---

# 25. Ereignisse

Während des Lebenszyklus können folgende Ereignisse erzeugt werden:

```text
Installed

Validated

Registered

Activated

Loaded

Initialized

Ready

Started

Paused

Resumed

Completed

Failed

Unloaded

Deactivated

Archived

Removed
```

Diese Ereignisse können von Monitoring- und Verwaltungsdiensten ausgewertet werden.

---

# 26. YAML-Beispiel

```yaml
lifecycle:

  install: required

  validation: strict

  activation: automatic

  allowPause: true

  allowResume: true

  persistent: false

  removable: true

  archiveBeforeRemove: true
```

---

# 27. Qualitätsanforderungen

Der Capability-Lebenszyklus muss:

- deterministisch sein,
- reproduzierbar sein,
- vollständig protokollierbar sein,
- fehlertolerant sein,
- transaktionale Zustandswechsel unterstützen,
- parallele Versionen verwalten können,
- Ressourcen zuverlässig freigeben,
- sichere Rollbacks ermöglichen.

---

# 28. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- alle definierten Lebenszykluszustände unterstützt werden,
- Zustandsübergänge ausschließlich über die Runtime erfolgen,
- Validierung vor der Registrierung durchgeführt wird,
- Aktivierung und Deaktivierung unabhängig von Installation und Entfernung möglich sind,
- Ressourcen beim Entladen vollständig freigegeben werden,
- mehrere Versionen parallel verwaltet werden können,
- Zustandswechsel reproduzierbar und nachvollziehbar protokolliert werden.

---

# 29. Zusammenfassung

Der Capability-Lebenszyklus definiert den vollständigen Ablauf einer Nova Capability – von der Installation über Validierung, Registrierung, Aktivierung und Ausführung bis hin zur Archivierung oder Entfernung.

Durch klar definierte Zustände, transaktionale Zustandswechsel und eine zentrale Verwaltung durch die Runtime entsteht ein reproduzierbares, sicheres und langfristig wartbares Ausführungsmodell, das die Grundlage für den zuverlässigen Betrieb aller Capabilities innerhalb der Nova Platform bildet.