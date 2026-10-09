
# NPSPEC-STUDIO-WORKSPACE-0001 – NovaLang Studio Workspace

## Status

Angenommen

## Kategorie

NovaLang Studio / Workspace Management

## Zweck

Definiert die Verwaltung von Arbeitsbereichen in NovaLang Studio.

Ein Workspace organisiert Solutions, Projekte, Quelldateien, Ressourcen und Entwicklungswerkzeuge innerhalb einer gemeinsamen Arbeitsumgebung.

Ziel sind übersichtliche Projektstrukturen, schnelle Navigation und die zuverlässige Wiederherstellung des Arbeitszustands.

## Workspace-Modell

Ein Workspace kann mehrere Solutions und eigenständige Projekte enthalten.

| Element | Beschreibung |
|---|---|
| Workspace | Übergeordneter Arbeitsbereich |
| Solution | Zusammenstellung von Capabilities, Logic Graph und UI |
| Projekt | Eigenständige Entwicklungseinheit |
| Dokument | Geöffnete oder referenzierte Datei |
| Ressource | Bilder, Daten und weitere Projektdateien |
| Session | Aktueller Arbeitszustand |

Ein Workspace ist eine organisatorische Einheit und keine zusätzliche Sicherheits- oder Ausführungsgrenze.

## Workspace-Struktur

Beispiel:

```text
MeinWorkspace/
├── workspace.xml
├── Solutions/
│   └── MeineSolution/
│       ├── solution.xml
│       ├── Logic/
│       │   └── main.nlf
│       ├── UI/
│       │   └── main.nui
│       └── Scripts/
│           └── helper.nova
├── Projects/
│   └── MeineBibliothek/
│       └── library.nova
└── Resources/
```

Die Ordnerstruktur ist eine mögliche Konvention und darf nicht zwingend vorgeschrieben werden.

## Workspace-Konfiguration

`workspace.xml` enthält die versionierte Workspace-Konfiguration.

Mindestens vorgesehen sind:

- Eindeutige Workspace-ID
- Formatversion
- Anzeigename
- Referenzen auf Solutions und Projekte
- Workspace-spezifische Einstellungen

Referenzen dürfen relativ oder über NovaOS-Ressourcenidentitäten aufgelöst werden.

Absolute Pfade sollen für portable Workspaces vermieden werden.

## Workspace Manager

Der Workspace Manager übernimmt:

- Erstellen, Öffnen und Schließen von Workspaces
- Hinzufügen und Entfernen von Solutions und Projekten
- Verwaltung von Dokumentreferenzen
- Überwachung externer Dateiänderungen
- Auflösung von Projektabhängigkeiten
- Wiederherstellung des Arbeitszustands

Das Entfernen eines Projekts aus dem Workspace darf dessen Dateien nicht automatisch löschen.

## Dokumentverwaltung

Dokumente werden über eine gemeinsame Dokumentverwaltung bereitgestellt.

Unterstützt werden:

- Mehrere gleichzeitig geöffnete Dateien
- Erkennung ungespeicherter Änderungen
- Undo und Redo
- Automatische Wiederherstellung nach Abstürzen
- Erkennung externer Änderungen
- Konfliktbehandlung bei paralleler Bearbeitung

Eine Datei darf nicht durch mehrere Studio-Komponenten unkoordiniert überschrieben werden.

## Workspace-Sitzung

NovaLang Studio kann folgende Sitzungsinformationen wiederherstellen:

- Geöffnete Dokumente
- Aktive Editor-Tabs
- Fenster- und Panelanordnung
- Cursor- und Auswahlpositionen
- Aktive Solution und Build-Konfiguration
- Debug- und Vorschaukonfigurationen

Sitzungsdaten werden getrennt von den eigentlichen Projektdateien gespeichert.

## Navigation und Suche

Der Workspace stellt eine gemeinsame Navigation bereit.

- Projekt- und Solution-Explorer
- Datei- und Symbolsuche
- Navigation zu Definitionen und Referenzen
- Zuordnung zwischen Code, Logic Graph und UI
- Zuletzt verwendete Dokumente

Suchindizes müssen inkrementell aktualisiert werden können.

## Zusammenarbeit

Workspaces unterstützen kontrollierte gemeinsame Bearbeitung.

- Änderungen werden dokumentbezogen synchronisiert.
- Gleichzeitige Änderungen müssen Konflikte erkennen.
- Zugriffsrechte gelten für die tatsächlich freigegebenen Ressourcen.
- Eine Freigabe des Workspace-Namens erteilt keinen Zugriff auf sämtliche enthaltenen Dateien.

## Performance

- Große Workspaces müssen schrittweise geladen werden können.
- Nicht geöffnete Dokumente dürfen nicht vollständig analysiert werden müssen.
- Dateisystemänderungen sollen ereignisbasiert verarbeitet werden.
- Indizes und Zwischenergebnisse müssen wiederverwendbar sein.
- Hintergrundaufgaben unterliegen Ressourcenlimits.

## Sicherheit

- Workspace-Konfigurationen dürfen keine Berechtigungen erteilen.
- Jede Solution behält ihre eigene verifizierte Identität.
- Capability-Berechtigungen bleiben an die jeweilige Solution gebunden.
- Externe Projektdateien müssen als potenziell nicht vertrauenswürdig behandelt werden.
- Automatische Build- oder Ausführungsschritte benötigen entsprechende Autorisierung.

## Normative Anforderungen

1. NovaLang Studio MUSS mehrere Solutions und Projekte innerhalb eines Workspace unterstützen.
2. Workspaces MÜSSEN unabhängig von der physischen Ordnerstruktur verwaltbar sein.
3. Workspace-Konfigurationen MÜSSEN versioniert werden.
4. Dokumentzustände MÜSSEN zentral und konsistent verwaltet werden.
5. Ungespeicherte Änderungen MÜSSEN vor Datenverlust geschützt werden.
6. Externe Dateiänderungen und Bearbeitungskonflikte MÜSSEN erkannt werden.
7. Workspace-Sitzungen MÜSSEN wiederherstellbar sein.
8. Große Workspaces MÜSSEN ressourcenschonend verarbeitet werden.
9. Workspace-Zugriffe DÜRFEN keine Capability- oder Sicherheitsgrenzen umgehen.
10. Workspace- und Solution-Identitäten MÜSSEN voneinander unabhängig bleiben.

## Ergebnis

NovaLang Studio erhält eine zentrale Workspace-Verwaltung für mehrere Solutions, Projekte und Dokumente mit konsistentem Arbeitszustand, schneller Navigation, Wiederherstellung und sicherer Zusammenarbeit.
