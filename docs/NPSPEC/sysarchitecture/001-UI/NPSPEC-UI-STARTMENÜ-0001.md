# NPSPEC-UI-STARTMENÜ-0001 – Nova Startmenü

## Status

Angenommen

## Kategorie

UI / Startmenü / Navigation / Capabilities / Search

## Zweck

Das Nova Startmenü ist der zentrale Einstiegspunkt für Anwendungen, Capabilities, Dateien, Einstellungen, Suche und häufig benötigte Systemfunktionen.

Es verbindet klassische Startmenü-Navigation mit der objekt- und capability-orientierten Architektur von NovaOS.

```text
Start
 ├── Suche
 ├── Angeheftet
 ├── Anwendungen
 ├── Capabilities
 ├── Zuletzt verwendet
 ├── Empfehlungen
 └── Systemaktionen
```

Das Startmenü soll übersichtlich bleiben und den Nutzer nicht mit allen verfügbaren Systemfunktionen gleichzeitig konfrontieren.

## Grundprinzip

Das Startmenü arbeitet nicht ausschließlich nach dem klassischen Modell:

```text
Programm auswählen
      ↓
Programm starten
```

sondern zusätzlich:

```text
Was möchte der Nutzer tun?
      ↓
Objekt / Intent
      ↓
Capability
      ↓
Geeigneter Provider
```

Beispiel:

```text
Bild bearbeiten
      ↓
Image.Edit
      ↓
Passender Capability Provider
```

## Architektur

```text
User
  ↓
Startmenü
  ├── Search
  ├── Objects
  ├── Applications
  ├── Capabilities
  └── System Actions
          ↓
Semantic UI
          ↓
Capability System
```

Das Startmenü ist geschützte NovaOS-System-UI.

## Hauptansicht

Die Standardansicht soll bewusst kompakt bleiben.

```text
┌──────────────────────────────────────┐
│ 🔍 Suchen                            │
├──────────────────────────────────────┤
│ Angeheftet                           │
│                                      │
│ [App] [App] [App] [App]             │
│ [App] [App] [App] [App]             │
├──────────────────────────────────────┤
│ Empfohlen / Zuletzt                  │
│ Dokument                             │
│ Projekt                              │
│ Letzte Aktivität                     │
├──────────────────────────────────────┤
│ Benutzer                       Power │
└──────────────────────────────────────┘
```

Weitere Funktionen werden bei Bedarf eingeblendet.

## Suche

Die Suche ist ein zentraler Bestandteil des Startmenüs.

Sie kann durchsuchen:

```text
Applications
NovaOS Objects
Files
Settings
Capabilities
Commands
Recent Content
Help
```

Beispiel:

```text
"PDF bearbeiten"
       ↓
Search Intent
       ↓
Document.PDF.Edit
       ↓
Available Capability
```

Die Suche soll damit nicht nur Namen finden, sondern auch Funktionen.

## Angeheftete Elemente

Der Nutzer kann häufig benötigte Elemente anheften.

```text
Application
Object
Folder
Capability
System Function
Web Resource
```

Angeheftete Objekte sollen stabile Identitäten verwenden.

Ein verschobenes Dokument soll deshalb nicht allein wegen eines geänderten Dateipfades aus dem Startmenü verschwinden.

## Anwendungen

Installierte oder verfügbare Anwendungen können klassisch aufgelistet werden.

```text
Alle Anwendungen
 ├── A
 ├── B
 ├── C
 └── ...
```

Das klassische App-Modell bleibt damit verfügbar, ist aber nicht die einzige Form des Funktionszugriffs.

## Capabilities

Systemfähigkeiten können direkt auffindbar sein.

Beispiele:

```text
Dokument erstellen
PDF bearbeiten
Bild bearbeiten
Datei verschlüsseln
Bildschirm aufnehmen
Drucken
Scannen
Übersetzen
Berechnen
```

Der Nutzer muss nicht zwingend wissen, welches Programm diese Funktion bereitstellt.

## Zuletzt verwendet

Das Startmenü kann zuletzt verwendete Objekte und Aktivitäten anzeigen.

```text
Recent Documents
Recent Projects
Recent Applications
Recent Locations
Recent Activities
```

Diese Informationen unterliegen den Privacy- und Session-Regeln.

## Empfehlungen

NovaOS darf kontextbezogene Vorschläge anzeigen.

Beispiele:

```text
Weiterarbeiten an …
Zuletzt geöffnet …
Häufig verwendet …
Passende Aktion …
```

Dabei gilt:

```text
Explizite Nutzerentscheidung
        >
Adaptive Empfehlung
```

Empfehlungen dürfen die stabile Grundstruktur des Startmenüs nicht ständig verändern.

## Kontextbezogene Aktionen

Ein Objekt kann direkt passende Aktionen anbieten.

```text
Document.pdf
   ↓
Open
Edit
Print
Share
Encrypt
```

Die Aktionen werden über semantische Capabilities bestimmt.

```text
Object Type
    ↓
Capability Discovery
    ↓
Available Actions
```

## Startmenü und Dashboard

Startmenü und Dashboard bleiben getrennte Systembereiche.

```text
Startmenü
→ Funktionen, Anwendungen, Suche und Navigation

Dashboard
→ Wetter, Nachrichten, Termine und Informationen
```

Das Startmenü darf einen direkten Zugang zum Dashboard besitzen.

Es soll jedoch nicht selbst mit Wetter-, Nachrichten- oder Widget-Inhalten überladen werden.

## Systemaktionen

Wichtige Session- und Energieaktionen sind direkt erreichbar.

```text
Lock
Sign Out
Switch User
Sleep
Restart
Shutdown
```

Sicherheitskritische Aktionen müssen durch NovaOS selbst ausgeführt und autorisiert werden.

## Benutzerbereich

Das Startmenü kann einen kompakten Benutzerbereich enthalten.

```text
User
 ├── Account
 ├── Session
 ├── Profile
 └── Sign Out
```

Dieser Bereich darf keine unnötigen persönlichen Informationen offenlegen.

## Responsive Verhalten

Das Startmenü passt sich an:

```text
Display Size
Window Environment
Scaling
Touch
Keyboard
Accessibility
```

an.

Beispiel:

```text
Desktop
→ kompaktes schwebendes Startmenü

Small Display
→ größere Startansicht

Touch
→ größere Interaktionsflächen
```

Die grundlegende Struktur und Semantik bleiben erhalten.

## Tastatur und Suche

Das Startmenü muss vollständig per Tastatur bedienbar sein.

Nach dem Öffnen soll eine direkte Texteingabe die Suche aktivieren können.

```text
Open Start
    ↓
Type
    ↓
Search
    ↓
Select
    ↓
Execute
```

Dadurch kann ein Nutzer Funktionen sehr schnell aufrufen.

## Capability Integration

Das Startmenü kann Capabilities direkt aus dem Capability Registry beziehen.

```text
Capability Registry
       ↓
Discoverable Capabilities
       ↓
Semantic Classification
       ↓
Startmenü / Suche
```

Capability Provider dürfen jedoch nicht beliebig eigene Startmenü-Oberflächen injizieren.

## Adaptive Integration

Adaptive Mechanismen dürfen:

```text
Frequently Used Items hervorheben
Relevant Objects vorschlagen
Likely Actions vorschlagen
Search Ranking verbessern
```

Sie dürfen nicht:

```text
Nutzer-Pins entfernen
Sicherheitsregeln umgehen
Capabilities automatisch autorisieren
Grundlegende Navigation verstecken
```

## Datenschutz

Startmenü-Inhalte können sensible Informationen enthalten.

Insbesondere:

```text
Recent Files
Recent Activities
Search History
Recommendations
```

müssen entsprechend der Privacy-Regeln behandelt werden.

Private Inhalte dürfen nicht ohne entsprechende Autorisierung zwischen Sessions oder Benutzern sichtbar werden.

## Fehlerverhalten

Fällt ein Provider oder Suchdienst aus, muss das Startmenü weiterhin grundlegende Funktionen anbieten.

```text
Capability Provider Failure
        ↓
Remove / Disable Result
        ↓
Startmenü remains operational
```

Ein defekter Eintrag darf das Startmenü nicht blockieren.

## Accessibility

Alle Startmenü-Bereiche müssen über den Accessibility Tree erreichbar sein.

Dazu gehören:

```text
Search
Pinned Items
Applications
Recommendations
System Actions
User Controls
```

Die Bedienung darf nicht ausschließlich von Pointer-Interaktion abhängen.

## Normative Anforderungen

1. Das Startmenü MUSS als geschützte NovaOS-System-UI ausgeführt werden.
2. Das Startmenü MUSS Anwendungen, Objekte und Systemfunktionen auffindbar machen können.
3. Capabilities SOLLEN direkt über Suche und Navigation erreichbar sein.
4. Die Suche SOLL semantische Intents unterstützen.
5. Nutzer MÜSSEN Elemente anheften und entfernen können.
6. Explizite Nutzer-Pins MÜSSEN gegenüber adaptiven Empfehlungen Vorrang besitzen.
7. Empfehlungen DÜRFEN die stabile Grundnavigation nicht verdrängen.
8. Objektaktionen SOLLEN über semantische Capabilities ermittelt werden.
9. Capability Provider DÜRFEN keine unkontrollierte Startmenü-UI injizieren.
10. Startmenü und Dashboard MÜSSEN logisch getrennt bleiben.
11. Das Startmenü MUSS vollständig per Tastatur bedienbar sein.
12. Das Startmenü MUSS responsive und touchfähig sein.
13. Persönliche und zuletzt verwendete Inhalte MÜSSEN Privacy-Regeln unterliegen.
14. Die Sichtbarkeit einer Capability DARF keine Autorisierung implizieren.
15. Der Ausfall einzelner Provider DARF das Startmenü nicht blockieren.
16. Alle wesentlichen Funktionen MÜSSEN im Accessibility Tree repräsentiert werden.

## Abhängigkeiten

- `NPSPEC-UI-DESKTOP-0001`
- `NPSPEC-UI-DASHBOARD-0001`
- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `ADR-CAPABILITY-0001`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-OBJECT-0001`

## Ergebnis

Das Nova Startmenü verbindet die vertraute Bedienung eines modernen Desktop-Startmenüs mit der Capability-Architektur von NovaOS:

```text
Start
  ↓
Search / Browse / Recent
  ↓
Application / Object / Intent
  ↓
Semantic Action
  ↓
Capability
  ↓
Authorized Provider
```

Der Nutzer kann weiterhin klassisch Anwendungen starten, gleichzeitig aber direkt nach Aufgaben wie „PDF bearbeiten“, „Bild zuschneiden“ oder „Datei verschlüsseln“ suchen, ohne wissen zu müssen, welche konkrete Anwendung diese Funktion bereitstellt.