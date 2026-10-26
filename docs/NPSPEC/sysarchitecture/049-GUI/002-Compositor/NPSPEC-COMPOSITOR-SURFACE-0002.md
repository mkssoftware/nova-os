# NPSPEC-COMPOSITOR-SURFACE-0002 – Nova Surface Lifecycle

**Status:** Angenommen  
**Bereich:** Compositor / Surface-Lebenszyklus  
**System:** NovaOS

## 1. Zweck

Nova Surface Lifecycle definiert die Zustände, Zustandsübergänge und Lebensdauer grafischer Oberflächen innerhalb des Nova Compositors.

Ziel ist eine sichere, konsistente und ressourcenschonende Verwaltung von Surfaces während ihrer gesamten Existenz.

## 2. Architektur

- Zustandsbasierte Lebenszyklusverwaltung durch Nova Surface Management.
- Trennung zwischen Surface-Lebensdauer und Grafikbuffer-Lebensdauer.
- Atomare Zustandsänderungen durch Surface-Commits.
- Ereignisgesteuerte Verarbeitung.
- Capability-basierte Zugriffssteuerung.
- Asynchrone Ressourcenfreigabe.
- Integration in Nova Compositor Pipeline und Fault Recovery.
- Unterstützung hierarchischer Parent-Child-Surfaces.

## 3. Lebenszykluszustände

| Zustand | Beschreibung |
|---|---|
| Created | Surface wurde erstellt. |
| Configured | Grundlegende Eigenschaften sind festgelegt. |
| Attached | Ein gültiger Grafikbuffer ist zugeordnet. |
| Committed | Änderungen wurden atomar übernommen. |
| Active | Surface ist Bestandteil der aktiven Szene. |
| Hidden | Surface ist vorübergehend nicht sichtbar. |
| Destroying | Surface wird entfernt; ausstehende Zugriffe werden abgeschlossen. |
| Destroyed | Surface ist vollständig freigegeben. |

**Festlegung:** Committed beschreibt einen abgeschlossenen Zustandswechsel und muss nicht dauerhaft bestehen bleiben. Eine Surface kann auch ohne zugeordneten Buffer existieren.

## 4. Kernfunktionen

- Erstellung und Initialisierung von Surfaces.
- Validierung von Zustandsübergängen.
- Verwaltung ausstehender und bestätigter Änderungen.
- Aktivierung und Deaktivierung grafischer Oberflächen.
- Austausch und Entfernung zugeordneter Buffer.
- Verwaltung von Parent-Child-Abhängigkeiten.
- Behandlung von Client-Abstürzen.
- Kontrollierte Zerstörung und Ressourcenfreigabe.
- Wiederherstellung nach Compositor-Fehlern.

## 5. Normative Anforderungen

- SURFACE-001: Jede Surface MUSS einen eindeutig definierten Lebenszyklus besitzen.
- SURFACE-002: Zustandsübergänge MÜSSEN validiert werden.
- SURFACE-003: Änderungen MÜSSEN erst durch einen gültigen Commit wirksam werden.
- SURFACE-004: Zusammengehörige Änderungen MÜSSEN atomar übernommen werden.
- SURFACE-005: Eine Surface MUSS ohne zugeordneten Grafikbuffer existieren können.
- SURFACE-006: Buffer-Wechsel DÜRFEN die Surface-Identität nicht verändern.
- SURFACE-007: Sichtbarkeit und Buffer-Zuordnung MÜSSEN unabhängig verwaltet werden können.
- SURFACE-008: Ungültige Zustandsübergänge MÜSSEN kontrolliert zurückgewiesen werden.
- SURFACE-009: Parent-Child-Beziehungen MÜSSEN während des gesamten Lebenszyklus konsistent bleiben.
- SURFACE-010: Die Zerstörung einer Parent-Surface MUSS ihre Child-Surfaces nach einer definierten Richtlinie behandeln.
- SURFACE-011: Zerstörte Surfaces DÜRFEN nicht erneut aktiviert werden.
- SURFACE-012: Veraltete Surface-Handles MÜSSEN zuverlässig erkannt werden.
- SURFACE-013: Buffer DÜRFEN während laufender GPU- oder Compositor-Zugriffe nicht freigegeben werden.
- SURFACE-014: Die Zerstörung einer Surface MUSS für den aufrufenden Client nicht blockierend erfolgen können.
- SURFACE-015: Beim Absturz eines Clients MÜSSEN dessen Surfaces kontrolliert bereinigt werden.
- SURFACE-016: Wiederherstellbare Surface-Zustände SOLLEN nach einem Compositor-Neustart rekonstruiert werden können.
- SURFACE-017: Lebenszyklusereignisse MÜSSEN an betroffene Systemkomponenten übermittelt werden.
- SURFACE-018: Ressourcenfreigaben MÜSSEN vollständig und nachvollziehbar erfolgen.

## 6. Zustandsverwaltung

### Pending State
Enthält noch nicht bestätigte Änderungen einer Surface.

### Committed State
Enthält den zuletzt erfolgreich übernommenen Surface-Zustand.

### Presented State
Beschreibt den zuletzt tatsächlich dargestellten Zustand.

Diese Zustände MÜSSEN getrennt verwaltet werden können, damit Rendering und Client-Aktualisierungen unabhängig voneinander erfolgen.

## 7. Zerstörung und Ressourcenfreigabe

1. **Destroy Request:** Berechtigte Zerstörungsanforderung empfangen.
2. **Invalidate:** Surface-Handle für neue Operationen ungültig machen.
3. **Detach:** Surface aus der Szenenstruktur entfernen.
4. **Notify:** Betroffene Komponenten informieren.
5. **Synchronize:** Ausstehende Buffer- und GPU-Zugriffe abschließen.
6. **Release:** Zugehörige Ressourcen freigeben.
7. **Finalize:** Zustand auf Destroyed setzen.

Die Freigabe DARF erst erfolgen, wenn keine aktiven Referenzen oder Zugriffe mehr bestehen.

## 8. Fehlerbehandlung und Performance

- Vermeidung unnötiger Surface-Neuerstellungen.
- Wiederverwendung gültiger Grafikbuffer.
- Nicht blockierende Zustandsübergänge.
- Minimierung von Synchronisationsaufwand.
- Schutz vor Use-after-Free und doppelter Ressourcenfreigabe.
- Automatische Bereinigung verwaister Surfaces.
- Begrenzung ausstehender Zustandsänderungen.
- Kontrollierte Wiederherstellung nach Client- oder Compositor-Ausfällen.
- Protokollierung fehlerhafter Lebenszyklusoperationen.

## 9. Ergebnis

NovaOS erhält eine deterministische und sichere Lebenszyklusverwaltung für grafische Oberflächen.

Surfaces können unabhängig von ihren Grafikbuffern erstellt, aktualisiert, aktiviert und zerstört werden.

Atomare Zustandsübergänge und kontrollierte Ressourcenfreigaben gewährleisten eine stabile Grundlage für die gesamte Bildkomposition.