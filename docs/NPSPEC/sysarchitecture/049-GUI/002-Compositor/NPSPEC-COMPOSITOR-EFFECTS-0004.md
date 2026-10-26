# NPSPEC-COMPOSITOR-EFFECTS-0004 – Nova Window Transitions

**Status:** Angenommen  
**Bereich:** Compositor / Fensterübergänge  
**System:** NovaOS

## 1. Zweck

Nova Window Transitions definiert die visuellen Übergänge zwischen verschiedenen Fensterzuständen innerhalb des Nova Compositors.

Ziel sind flüssige, dezente und ressourcenschonende Animationen, die Zustandsänderungen verständlich machen, ohne die Bedienung zu verzögern.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Window Manager.
- Trennung zwischen Fensterzustand und visueller Übergangsanimation.
- Verwaltung durch einen zentralen Transition Controller.
- GPU-beschleunigte Animation mit Software-Fallback.
- Nutzung vorhandener Surface- und Layer-Strukturen.
- Integration in Nova Compositor Scheduling.
- Unterstützung unterbrechbarer und umkehrbarer Animationen.
- Konfiguration über das native Nova UI Designsystem.
- Adaptive Qualitätssteuerung und vollständige Deaktivierbarkeit.

## 3. Kernfunktionen

- Animation beim Öffnen und Schließen von Fenstern.
- Übergänge beim Minimieren und Wiederherstellen.
- Animation beim Maximieren und Verkleinern.
- Übergänge zwischen Fensterpositionen und -größen.
- Animation beim Wechsel zwischen virtuellen Desktops.
- Unterstützung von Opacity-, Scale- und Transform-Animationen.
- Kombination mehrerer Animationseigenschaften.
- Unterschiedliche Übergänge für Fenster, Dialoge und Popups.
- Unterbrechung laufender Animationen durch neue Benutzeraktionen.
- Unterstützung reduzierter Bewegung für barrierefreie Bedienung.

## 4. Normative Anforderungen

- EFFECTS-001: Fensterübergänge MÜSSEN unabhängig von der eigentlichen Fensterzustandsverwaltung erfolgen.
- EFFECTS-002: Der Window Manager MUSS den verbindlichen Fensterzustand bestimmen.
- EFFECTS-003: Übergänge MÜSSEN durch definierte Zustandsänderungen ausgelöst werden.
- EFFECTS-004: Animationen DÜRFEN die Verarbeitung von Benutzeraktionen nicht blockieren.
- EFFECTS-005: Laufende Animationen MÜSSEN unterbrochen oder durch neue Übergänge ersetzt werden können.
- EFFECTS-006: Unterbrechungen DÜRFEN keine ungültigen Fensterzustände erzeugen.
- EFFECTS-007: Position, Größe, Skalierung und Deckkraft MÜSSEN animierbar sein.
- EFFECTS-008: Übergänge MÜSSEN mit Nova Compositor Scheduling synchronisiert werden.
- EFFECTS-009: Animationen SOLLEN möglichst ohne erneutes Rendering des Fensterinhalts erfolgen.
- EFFECTS-010: Änderungen animierter Bereiche MÜSSEN im Damage Tracking berücksichtigt werden.
- EFFECTS-011: Eingabe- und Fokusverhalten MÜSSEN während eines Übergangs eindeutig definiert sein.
- EFFECTS-012: Geschützte Systemoberflächen DÜRFEN durch Animationen nicht unzulässig verdeckt oder manipuliert werden.
- EFFECTS-013: Übergangsdauer und Animationskurven MÜSSEN konfigurierbar sein.
- EFFECTS-014: Eine systemweite Einstellung für reduzierte Bewegung MUSS unterstützt werden.
- EFFECTS-015: Sämtliche Übergänge MÜSSEN deaktivierbar sein.
- EFFECTS-016: Bei Ressourcenknappheit MÜSSEN Animationen vereinfacht oder übersprungen werden können.
- EFFECTS-017: Bei Ausfall des Animationssystems MUSS der gültige Fensterendzustand unmittelbar dargestellt werden.
- EFFECTS-018: Fensterübergänge MÜSSEN ohne KI-Abhängigkeit funktionieren.

## 5. Übergangstypen

### Open Transition
Ein Fenster wird eingeblendet, optional kombiniert mit einer dezenten Skalierung.

### Close Transition
Ein Fenster wird ausgeblendet. Die zugehörigen Ressourcen werden nach Abschluss oder Abbruch des Übergangs sicher freigegeben.

### Minimize Transition
Ein Fenster wechselt visuell zu seinem Zielbereich, beispielsweise einem Taskleisteneintrag.

### Restore Transition
Ein minimiertes Fenster wird an seiner vorgesehenen Position wiederhergestellt.

### Maximize Transition
Ein Fenster wechselt animiert zwischen normaler und maximierter Geometrie.

### Move and Resize Transition
Positions- und Größenänderungen werden bei geeigneten systemgesteuerten Übergängen interpoliert. Direkte Benutzerinteraktionen SOLLEN ohne künstliche Verzögerung umgesetzt werden.

### Workspace Transition
Fenstergruppen wechseln beim Umschalten virtueller Desktops gemeinsam ihren visuellen Zustand.

## 6. Animationsmodell

Jeder Übergang besitzt folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Transition ID | Eindeutige Übergangsidentität |
| Target | Zugehöriges Fenster oder Layer |
| Start State | Visueller Ausgangszustand |
| End State | Visueller Zielzustand |
| Duration | Übergangsdauer |
| Easing | Interpolationskurve |
| Properties | Animierte Eigenschaften |
| Priority | Priorität bei konkurrierenden Übergängen |
| Interrupt Policy | Verhalten bei Unterbrechungen |

Die Animation verändert ausschließlich den visuellen Zustand. Der verbindliche Fensterzustand verbleibt beim Window Manager.

## 7. Verarbeitungsablauf

1. **Trigger:** Fensterzustandsänderung empfangen.
2. **Resolve:** Ausgangs- und Zielzustand bestimmen.
3. **Configure:** Übergangseigenschaften und Animationsrichtlinien laden.
4. **Schedule:** Animation in den Frame Scheduler eintragen.
5. **Interpolate:** Visuelle Eigenschaften für den aktuellen Zeitpunkt berechnen.
6. **Compose:** Aktualisierte Fensterdarstellung komponieren.
7. **Present:** Animationsframe synchronisiert ausgeben.
8. **Complete:** Zielzustand übernehmen und Übergangsressourcen freigeben.

Bei Unterbrechung wird der aktuelle visuelle Zustand als möglicher Ausgangspunkt eines neuen Übergangs verwendet.

## 8. Performance und Zuverlässigkeit

- Bevorzugung von GPU-Transformationen und Opacity-Animationen.
- Wiederverwendung vorhandener Fensterbuffer.
- Vermeidung unnötiger Neuberechnungen des Fensterinhalts.
- Partielle Aktualisierung animierter Bildschirmbereiche.
- Begrenzung gleichzeitig aktiver Übergänge.
- Adaptive Animationsqualität bei hoher Systemlast.
- Keine zusätzliche Frame-Erzeugung nach Abschluss einer Animation.
- Sofortige Darstellung des Endzustands bei deaktivierten Effekten.
- Sichere Behandlung unterbrochener oder abgebrochener Übergänge.
- Freigabe temporärer Ressourcen nach Abschluss.
- Integration in Nova Compositor Fault Recovery.

## 9. Ergebnis

NovaOS erhält eine einheitliche Architektur für flüssige und dezente Fensterübergänge.

Öffnen, Schließen, Minimieren, Maximieren und Wiederherstellen werden durch konsistente Animationen visuell unterstützt.

Die Übergänge bleiben jederzeit unterbrechbar, ressourcenschonend und vollständig optional.

Damit vermittelt NovaOS eine moderne und natürliche Bedienung, ohne Geschwindigkeit, Reaktionsfähigkeit oder Benutzerkontrolle einzuschränken.