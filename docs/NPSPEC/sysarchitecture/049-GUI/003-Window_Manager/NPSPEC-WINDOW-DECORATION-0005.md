# NPSPEC-WINDOW-DECORATION-0005 – Nova Client-Side and Server-Side Decorations

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterdekorationen  
**System:** NovaOS

## 1. Zweck

Nova Client-Side and Server-Side Decorations definiert die Zuständigkeiten für systemseitige und anwendungsseitige Fensterdekorationen.

Ziel ist eine flexible Gestaltung von Fenstern bei gleichzeitig einheitlicher Bedienung, Sicherheit und zuverlässiger Fensterverwaltung.

## 2. Architektur

NovaOS unterstützt zwei Dekorationsmodelle:

| Modell | Beschreibung |
|---|---|
| Server-Side Decorations (SSD) | Fensterdekorationen werden systemseitig bereitgestellt und dargestellt. |
| Client-Side Decorations (CSD) | Anwendungen gestalten und rendern ihre Fensterdekorationen selbst. |

Grundsätze:

- Der Window Manager verwaltet Fensterzustände und Fensteroperationen.
- Nova UI stellt systemseitige Dekorationen bereit.
- Der Compositor übernimmt die Darstellung der zugehörigen Oberflächen.
- Anwendungen können eigene Dekorationen über definierte Schnittstellen bereitstellen.
- Das Display Server Protocol koordiniert die Dekorationsmodi und Eingabebereiche.
- Die Dekorationsart verändert weder Fensterbesitz noch Berechtigungen.

**Server-Side Decorations bilden den Standard von NovaOS.**

## 3. Server-Side Decorations

Bei SSD übernimmt NovaOS:

- Titelleiste und Fenstertitel.
- Fensterbedienelemente.
- Fensterrahmen und Resize-Bereiche.
- Darstellung aktiver und inaktiver Fenster.
- Theme- und DPI-Anpassungen.
- Eingabeverarbeitung der Dekoration.
- Optionale Schatten-, Transparenz- und Blur-Effekte.

Die Anwendung stellt ausschließlich ihren Fensterinhalt bereit.

SSD MUSS unabhängig von der Reaktionsfähigkeit der Anwendung bedienbar bleiben.

## 4. Client-Side Decorations

Bei CSD übernimmt die Anwendung die visuelle Gestaltung ihrer Fensterdekoration.

Unterstützt werden:

- Benutzerdefinierte Titelleisten.
- Integrierte Werkzeugleisten und Registerkarten.
- Individuelle Fensterrahmen.
- Eigene Fensterbedienelemente.
- Deklarative Nova UI-Templates.
- Rahmenlose Fenster.

Die Anwendung MUSS interaktive Bereiche über das Display Server Protocol kennzeichnen können.

Der Window Manager bleibt für die tatsächliche Ausführung von Fensteroperationen zuständig.

## 5. Dekorationsaushandlung

Beim Erstellen eines Fensters wird der Dekorationsmodus festgelegt.

1. Anwendung übermittelt ihren bevorzugten Modus.
2. Display Server prüft die unterstützten Modi.
3. Window Manager berücksichtigt Fensterart und Sicherheitsrichtlinien.
4. Ein gültiger Dekorationsmodus wird ausgewählt.
5. Anwendung und Dekorationssystem erhalten die Konfiguration.
6. Fenster wird mit dem vereinbarten Modus dargestellt.

NovaOS DARF einen angeforderten CSD-Modus ablehnen.

Bei fehlender Unterstützung wird SSD verwendet.

Ein späterer Wechsel des Dekorationsmodus MUSS über eine definierte Konfigurationsänderung erfolgen.

## 6. Normative Anforderungen

- WINDOW-001: NovaOS MUSS SSD und CSD unterstützen.
- WINDOW-002: SSD MUSS der Standardmodus für reguläre Fenster sein.
- WINDOW-003: Der Window Manager MUSS unabhängig vom Dekorationsmodus die Kontrolle über Fensteroperationen behalten.
- WINDOW-004: Anwendungen DÜRFEN ihren bevorzugten Dekorationsmodus anfordern.
- WINDOW-005: Der tatsächlich verwendete Modus MUSS durch das System bestätigt werden.
- WINDOW-006: Dekorationswechsel MÜSSEN konsistent und ohne ungültige Zwischenzustände erfolgen.
- WINDOW-007: CSD-Anwendungen MÜSSEN Drag- und Resize-Bereiche über definierte Schnittstellen bereitstellen können.
- WINDOW-008: Clientseitige Fensteroperationen MÜSSEN durch den Window Manager validiert werden.
- WINDOW-009: CSD DARF keine zusätzlichen Fensterberechtigungen begründen.
- WINDOW-010: Geschützte Systemfenster DÜRFEN einen vorgeschriebenen Dekorationsmodus nicht umgehen.
- WINDOW-011: SSD und CSD MÜSSEN unterschiedliche DPI-Skalierungen unterstützen.
- WINDOW-012: Die Fenstergeometrie MUSS unabhängig vom Dekorationsmodus eindeutig definiert sein.
- WINDOW-013: Dekorationswechsel DÜRFEN den Anwendungszustand nicht verändern.
- WINDOW-014: Bei fehlerhaften CSD-Konfigurationen MUSS eine sichere Wiederherstellung der Fensterbedienung möglich sein.
- WINDOW-015: Dekorationen MÜSSEN ohne KI und ohne Netzwerkverbindung funktionieren.

## 7. Geometrie und Eingabebereiche

NovaOS unterscheidet:

| Bereich | Beschreibung |
|---|---|
| Content Bounds | Nutzbarer Fensterinhalt |
| Decoration Bounds | Gesamter Dekorationsbereich |
| Window Bounds | Äußere Fenstergeometrie |
| Drag Regions | Bereiche zum Verschieben |
| Resize Regions | Bereiche zur Größenänderung |
| Interactive Regions | Bedienelemente der Dekoration |

Bei SSD berechnet das System die Dekorationsgeometrie.

Bei CSD liefert die Anwendung ihre Dekorations- und Eingabebereiche.

Der Window Manager MUSS alle Angaben validieren.

Bei einem Dekorationswechsel MUSS die Fenstergeometrie entsprechend angepasst werden, ohne die gültigen Größenbeschränkungen zu verletzen.

## 8. Sicherheit und Zuständigkeiten

- Anwendungen dürfen ausschließlich eigene Fensterdekorationen gestalten.
- Fensteroperationen werden über autorisierte Protokollanforderungen ausgeführt.
- Geschützte Systemdialoge können SSD verbindlich vorschreiben.
- Clientseitige Dekorationen dürfen keine privilegierten Systemoberflächen vortäuschen.
- Fokus-, Z-Order- und Modalitätsregeln gelten unabhängig vom Dekorationsmodus.
- Anwendungen dürfen keine Eingabeereignisse außerhalb ihrer autorisierten Bereiche abfangen.

CSD ist ausschließlich eine Darstellungsentscheidung und keine Erweiterung der Anwendungsrechte.

## 9. Darstellung und Performance

### Server-Side Decorations

- Gemeinsame Dekorationsressourcen.
- Einheitliche Theme-Verarbeitung.
- Unabhängige Aktualisierung der Systemdekoration.
- Keine vollständige Neuzeichnung des Fensterinhalts bei reinen Dekorationsänderungen erforderlich.

### Client-Side Decorations

- Dekoration kann gemeinsam mit dem Anwendungsinhalt gerendert werden.
- Anwendung verwaltet ihre visuellen Dekorationsressourcen.
- Systemseitige Fensteroperationen bleiben unabhängig vom Client-Rendering.
- Fehlerhafte oder verzögerte Client-Frames dürfen andere Fenster nicht blockieren.

Beide Modelle MÜSSEN mit den Buffer- und Synchronisationsregeln des Compositors kompatibel sein.

## 10. Fehlerbehandlung

- Ungültige Dekorationsanforderungen werden zurückgewiesen.
- Nicht unterstützte Modi werden durch SSD ersetzt.
- Fehlerhafte Eingabebereiche dürfen keine unautorisierten Fensteroperationen ermöglichen.
- Bei einem fehlgeschlagenen Moduswechsel bleibt die letzte gültige Konfiguration bestehen.
- Nicht reagierende CSD-Anwendungen können über systemseitige Fensterbefehle verwaltet werden.
- Ein Compositor-Neustart verändert den vereinbarten logischen Dekorationsmodus nicht.
- Bei beschädigten Dekorationsdaten wird eine sichere Standardkonfiguration verwendet.

## 11. Ergebnis

NovaOS erhält ein einheitliches System zur Unterstützung systemseitiger und anwendungsseitiger Fensterdekorationen.

Anwendungen können ihre Fenster vollständig individuell gestalten, während NovaOS weiterhin Fensterzustände, Berechtigungen und grundlegende Fensteroperationen kontrolliert.

SSD gewährleistet eine konsistente Standarddarstellung. CSD ermöglicht die flexible Integration von Fensterdekorationen in moderne Anwendungsoberflächen.

**Grundsatz:** Anwendungen bestimmen auf Wunsch das Aussehen ihrer Fenster, NovaOS behält die Kontrolle über deren Verwaltung und Sicherheit.