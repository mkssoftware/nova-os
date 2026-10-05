# NPSPEC-WORKSPACE-UISTATE-0001 – Nova Workspace UI State

## Status

Angenommen

## Kategorie

Workspace / UI State

## Zweck

NovaOS definiert den UI State eines Workspace als wiederherstellbare Beschreibung seiner aktuellen Benutzeroberfläche.

Der UI State speichert Darstellung und Interaktionskontext, jedoch keine eigentlichen Benutzerdaten oder Berechtigungen.

## Grundprinzipien

```text
UI State ≠ User Data
UI State ≠ Application Data
UI State ≠ Authority
UI State ≠ Process State
```

Die Oberfläche soll nach dem erneuten Öffnen eines Workspace möglichst im vorherigen Arbeitszustand erscheinen.

## UI-State-Modell

```text
WorkspaceUIState
├── WorkspaceID
├── Version
├── Windows
├── Views
├── Layout
├── Navigation
├── Selection
└── ActiveContext
```

Programme und Solutions dürfen eigene UI-State-Bereiche innerhalb des Workspace besitzen.

## Wiederherstellbarer Zustand

Der UI State kann beispielsweise enthalten:

```text
Fensterpositionen
Fenstergrößen
geöffnete Ansichten
aktive Ansicht
Panel-Zustände
Navigation
Scrollpositionen
Auswahlzustände
Tabs
UI-Layout
```

Nur für die Wiederherstellung relevante Informationen sollen persistent gespeichert werden.

## Ressourcenreferenzen

UI-Elemente, die auf persistente Ressourcen verweisen, sollen stabile Identitäten verwenden.

```text
UI Element
    ↓
ObjectID
```

Pfade dürfen ergänzend gespeichert werden, sind jedoch nicht die primäre Identität.

## Solutions

Generierte und deklarative Solution-Oberflächen dürfen ihren Zustand im Workspace UI State hinterlegen.

```text
Workspace UI State
├── System UI
├── Solution A UI State
└── Solution B UI State
```

Die gespeicherte UI muss mit der jeweiligen Solution- und UI-Version validiert werden.

## Persistenz

UI-State-Änderungen dürfen automatisch gespeichert werden.

Zusammengehörige Änderungen sollen konsistent beziehungsweise transaktional persistiert werden.

Kurzlebige Effekte wie Animationen oder Hover-Zustände müssen nicht gespeichert werden.

## Wiederherstellung

```text
Load Workspace
      ↓
Load UI State
      ↓
Validate Version
      ↓
Resolve Resources
      ↓
Restore Available UI
```

Nicht mehr vorhandene Fenster, Ressourcen oder UI-Komponenten werden übersprungen oder durch einen definierten Fallback ersetzt.

## Sicherheit

UI State erzeugt keine Authority.

Das Wiederherstellen einer Ansicht darf nur Ressourcen öffnen, auf die weiterhin gültiger Zugriff besteht.

Credentials, Capability Tokens und aktive Capability Handles dürfen nicht als gewöhnlicher UI State gespeichert werden.

## Normative Anforderungen

1. NovaOS MUSS UI State einem eindeutigen `WorkspaceID` zuordnen können.
2. Persistenter UI State MUSS versionierbar sein.
3. UI State MUSS von Benutzerdaten und Authority getrennt bleiben.
4. Ressourcenreferenzen SOLLEN stabile Identitäten verwenden.
5. Programme und Solutions DÜRFEN eigene UI-State-Bereiche besitzen.
6. Nicht wiederherstellbare UI-Komponenten MÜSSEN kontrolliert übersprungen werden können.
7. Kurzlebige visuelle Zustände MÜSSEN nicht persistiert werden.
8. UI-Wiederherstellung DARF keine Berechtigungsprüfung umgehen.
9. Capability Tokens und Credentials DÜRFEN NICHT als gewöhnlicher UI State gespeichert werden.
10. Beschädigter UI State DARF die grundlegende Wiederherstellung des Workspace nicht verhindern.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-WORKSPACE-STATE-0001`
- `NPSPEC-WORKSPACE-SOLUTION-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`

## Ergebnis

NovaOS kann den visuellen Arbeitszustand eines Workspace unabhängig von Nutzdaten, Prozessen und Berechtigungen speichern und wiederherstellen. Dadurch kann der Benutzer seine Arbeit mit weitgehend identischer Oberfläche und gleichem Arbeitskontext fortsetzen.