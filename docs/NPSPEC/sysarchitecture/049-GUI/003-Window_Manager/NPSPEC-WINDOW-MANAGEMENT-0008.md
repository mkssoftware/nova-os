# NPSPEC-WINDOW-MANAGEMENT-0008 – Nova Parent and Child Windows

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterhierarchien  
**System:** NovaOS

## 1. Zweck

Nova Parent and Child Windows definiert die Beziehungen zwischen übergeordneten und untergeordneten Fenstern innerhalb der NovaOS-Desktopumgebung.

Ziel ist eine konsistente Verwaltung zusammengehöriger Fenster hinsichtlich Positionierung, Sichtbarkeit, Stapelreihenfolge, Lebenszyklus und Eingabeverarbeitung.

## 2. Architektur

- Zentrale Verwaltung der Fensterhierarchien durch den Nova Window Manager.
- Trennung zwischen Fensterbesitz, Fensterhierarchie und Modalität.
- Unterstützung mehrstufiger Parent-Child-Beziehungen.
- Integration in Window Lifecycle und Window State Model.
- Synchronisation mit Window Positioning und Z-Order Management.
- Capability-basierte Autorisierung von Fensterbeziehungen.
- Unabhängigkeit von der grafischen Surface-Hierarchie.
- Transaktionale Änderung von Fensterbeziehungen.

## 3. Fensterbeziehungen

NovaOS unterscheidet folgende Beziehungen:

| Beziehung | Beschreibung |
|---|---|
| Parent Window | Übergeordnetes Fenster |
| Child Window | Räumlich und logisch eingebundenes Unterfenster |
| Owned Window | Eigenständiges Fenster mit einer Besitzerfensterbeziehung |
| Root Window | Fenster ohne übergeordnetes Fenster |
| Transient Window | Temporär zugeordnetes Fenster, beispielsweise ein Popup |

### Child Window

Ein Child Window ist Bestandteil der logischen Fensterhierarchie seines Parent Windows.

- Positionierung relativ zum Parent.
- Optionale Begrenzung auf den Parent-Inhaltsbereich.
- Abhängigkeit von der Sichtbarkeit des Parent.
- Definierte Eingabe- und Stapelregeln.

### Owned Window

Ein Owned Window bleibt ein eigenständiges Fenster.

- Eigene Fenstergeometrie.
- Eigene Surface.
- Logische Zuordnung zum Besitzerfenster.
- Gemeinsame Aktivierungs- und Stapelregeln, soweit vorgesehen.

Eine Owned-Beziehung ist keine Sicherheits- oder Eigentumsübertragung.

## 4. Kernfunktionen

- Erstellung von Parent-Child-Beziehungen.
- Verwaltung mehrstufiger Fensterhierarchien.
- Relative Positionierung untergeordneter Fenster.
- Gemeinsame Sichtbarkeitsverwaltung.
- Hierarchische Z-Order-Verwaltung.
- Weiterleitung und Zuordnung von Eingabeereignissen.
- Dynamische Änderung zulässiger Fensterbeziehungen.
- Unterstützung eingebetteter Fenster.
- Kontrollierte Entfernung abhängiger Fenster.
- Wiederherstellung konsistenter Fensterhierarchien.

## 5. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS Fensterhierarchien zentral verwalten.
- WINDOW-002: Jedes Fenster DARF höchstens ein direktes Parent Window besitzen.
- WINDOW-003: Fensterhierarchien MÜSSEN frei von zyklischen Beziehungen sein.
- WINDOW-004: Parent-Child-Beziehungen MÜSSEN vor ihrer Registrierung autorisiert werden.
- WINDOW-005: Eine Fensterbeziehung DARF keine zusätzlichen Sicherheitsrechte begründen.
- WINDOW-006: Child Windows MÜSSEN relative Koordinaten zu ihrem Parent unterstützen.
- WINDOW-007: Änderungen der Parent-Geometrie MÜSSEN bei abhängigen Child Windows berücksichtigt werden.
- WINDOW-008: Child Windows DÜRFEN bei ausgeblendetem Parent nicht unabhängig sichtbar werden, sofern keine ausdrücklich definierte Ausnahme besteht.
- WINDOW-009: Die Z-Order von Child Windows MUSS die geltenden Hierarchieregeln berücksichtigen.
- WINDOW-010: Eingabeereignisse MÜSSEN anhand der gültigen Fensterhierarchie zugeordnet werden.
- WINDOW-011: Die Entfernung eines Parent Windows MUSS seine abhängigen Fensterbeziehungen kontrolliert auflösen.
- WINDOW-012: Eine Änderung des Parent Windows MUSS atomar erfolgen.
- WINDOW-013: Ungültige oder veraltete Fensterreferenzen MÜSSEN zurückgewiesen werden.
- WINDOW-014: Fensterhierarchien DÜRFEN bestehende Modalitätsregeln nicht umgehen.
- WINDOW-015: Die logische Fensterhierarchie MUSS unabhängig von der Surface-Hierarchie verwaltbar sein.
- WINDOW-016: Änderungen der Fensterhierarchie MÜSSEN dem Compositor konsistent übermittelt werden.
- WINDOW-017: Anwendungsabstürze DÜRFEN keine verwaisten oder ungültigen Fensterbeziehungen hinterlassen.
- WINDOW-018: Die Hierarchieverwaltung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 6. Hierarchiemodell

Jedes Fenster verwaltet mindestens folgende Informationen:

| Eigenschaft | Beschreibung |
|---|---|
| Window ID | Eindeutige Fensteridentität |
| Parent ID | Optionales übergeordnetes Fenster |
| Relation Type | Art der Fensterbeziehung |
| Relative Position | Position relativ zum Parent |
| Local Z-Order | Stapelposition innerhalb der Hierarchie |
| Visibility Policy | Regeln zur Sichtbarkeit |
| Clipping Policy | Begrenzung des Darstellungsbereichs |
| Lifetime Policy | Verhalten bei Entfernung des Parent |
| Relation Version | Version der Hierarchiebeziehung |

Die tatsächliche Bildschirmposition eines Child Windows ergibt sich aus seiner relativen Position und den Transformationen seiner übergeordneten Fenster.

## 7. Positionierung und Darstellung

### Relative Positionierung

Child Windows verwenden standardmäßig das Koordinatensystem ihres Parent Windows.

Positionsänderungen des Parent wirken sich auf die resultierende Bildschirmposition seiner Child Windows aus.

### Clipping

Child Windows können auf den Inhaltsbereich ihres Parent begrenzt werden.

Die Clipping-Regeln werden vom Window Manager festgelegt und durch den Compositor umgesetzt.

### Sichtbarkeit

Die effektive Sichtbarkeit eines Child Windows hängt von seiner eigenen Sichtbarkeit und der Sichtbarkeit seiner übergeordneten Fenster ab.

### Stapelreihenfolge

Child Windows werden innerhalb ihrer zulässigen Hierarchieebene angeordnet.

Sie dürfen nicht eigenmächtig in privilegierte Fenster- oder Systemebenen wechseln.

## 8. Lebenszyklus und Entfernung

Bei Entfernung eines Parent Windows:

1. Abhängige Fensterbeziehungen ermitteln.
2. Neue Hierarchieoperationen für den betroffenen Teilbaum sperren.
3. Festgelegte Lifetime Policies anwenden.
4. Abhängige Child Windows kontrolliert entfernen.
5. Eigenständige Owned Windows gemäß ihrer Richtlinie entfernen oder lösen.
6. Fokus-, Modalitäts- und Z-Order-Beziehungen aktualisieren.
7. Hierarchiereferenzen freigeben.

Eine automatische Übertragung von Child Windows auf andere Parent Windows erfolgt ausschließlich nach definierten Regeln.

Gemeinsam genutzte Ressourcen dürfen nicht vor Abschluss ihrer letzten Verwendung freigegeben werden.

## 9. Fehlerbehandlung und Zuverlässigkeit

- Zyklische Fensterbeziehungen werden verhindert.
- Ungültige Parent-Referenzen werden zurückgewiesen.
- Fehlgeschlagene Hierarchieänderungen werden vollständig zurückgenommen.
- Veraltete Hierarchieereignisse können neuere Zustände nicht überschreiben.
- Bei Anwendungsabsturz werden betroffene Fensterbeziehungen bereinigt.
- Ein Compositor-Neustart verändert die logische Fensterhierarchie nicht.
- Die Eingabeverarbeitung bleibt bei dynamischen Hierarchieänderungen konsistent.
- Sicherheitsgrenzen bleiben auch bei eingebetteten oder gemeinsam genutzten Fenstern erhalten.

## 10. Ergebnis

NovaOS erhält eine einheitliche und sichere Verwaltung hierarchischer Fensterbeziehungen.

Child Windows können relativ zu übergeordneten Fenstern positioniert, dargestellt und verwaltet werden, während Owned Windows eigenständige Fenster mit definierten Beziehungen bleiben.

Durch die Trennung von Fensterhierarchie, Sicherheitsbesitz und grafischer Surface-Struktur bleiben Fensterbeziehungen flexibel, nachvollziehbar und zuverlässig.

**Grundsatz:** Fenster können logisch zusammengehören, ohne dadurch ihre Sicherheitsgrenzen oder eigenständigen Zuständigkeiten zu verlieren.