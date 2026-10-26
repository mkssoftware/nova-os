# NPSPEC-COMPOSITOR-EFFECTS-0006 – Nova Adaptive Effects Quality

**Status:** Angenommen  
**Bereich:** Compositor / Adaptive Effektqualität  
**System:** NovaOS

## 1. Zweck

Nova Adaptive Effects Quality definiert die automatische Anpassung visueller Effekte an die verfügbare Systemleistung.

Ziel ist eine dauerhaft flüssige, reaktionsschnelle und energieeffiziente Desktopdarstellung, ohne die grundlegende Bedienbarkeit oder visuelle Konsistenz zu beeinträchtigen.

## 2. Architektur

- Integration in Nova Compositor Scheduling und Desktop Visual Effects.
- Zentrale Steuerung durch einen Adaptive Quality Controller.
- Überwachung von CPU-, GPU- und Speicherressourcen.
- Berücksichtigung von Frame-Zeiten und Präsentationsdeadlines.
- Anpassung einzelner Effekte statt pauschaler Deaktivierung.
- Unterstützung unterschiedlicher Qualitätsstufen pro Effekt.
- Berücksichtigung von Energieprofilen und Benutzerpräferenzen.
- Unabhängige Bewertung der Rendering-Leistung pro Display.
- Deterministische Steuerung ohne KI-Abhängigkeit.

## 3. Kernfunktionen

- Dynamische Anpassung von Blur- und Acrylic-Qualität.
- Reduzierung aufwendiger Schattenberechnungen.
- Anpassung der Auflösung von Effekt-Zwischenbuffern.
- Vereinfachung dekorativer Animationen.
- Wiederverwendung zwischengespeicherter Effektergebnisse.
- Priorisierung wichtiger visueller Rückmeldungen.
- Automatische Erkennung von Leistungsengpässen.
- Stufenweise Qualitätsreduzierung und Wiederherstellung.
- Vermeidung häufiger Qualitätswechsel.
- Unterstützung manueller Qualitätsvorgaben.
- Bereitstellung von Leistungs- und Qualitätsdiagnosen.

## 4. Normative Anforderungen

- EFFECTS-001: Der Compositor MUSS unterschiedliche Qualitätsstufen für dekorative Effekte unterstützen.
- EFFECTS-002: Die Qualitätssteuerung MUSS auf messbaren Leistungsdaten basieren.
- EFFECTS-003: Frame-Zeiten und verpasste Präsentationsdeadlines MÜSSEN berücksichtigt werden.
- EFFECTS-004: Qualitätsanpassungen MÜSSEN unabhängig für einzelne Effektklassen möglich sein.
- EFFECTS-005: Die grundlegende Desktopbedienung MUSS unabhängig von der gewählten Qualitätsstufe funktionieren.
- EFFECTS-006: Funktionale und sicherheitsrelevante visuelle Rückmeldungen DÜRFEN nicht ersatzlos deaktiviert werden.
- EFFECTS-007: Benutzerdefinierte Qualitätsgrenzen MÜSSEN berücksichtigt werden.
- EFFECTS-008: Barrierefreiheitseinstellungen MÜSSEN Vorrang vor automatischen Qualitätsentscheidungen besitzen.
- EFFECTS-009: Qualitätsänderungen SOLLEN schrittweise erfolgen.
- EFFECTS-010: Häufige Wechsel zwischen Qualitätsstufen MÜSSEN durch Hysterese verhindert werden.
- EFFECTS-011: Bei anhaltender Ressourcenknappheit MÜSSEN dekorative Effekte deaktiviert werden können.
- EFFECTS-012: Nach ausreichender Leistungsreserve SOLL die Effektqualität automatisch wiederhergestellt werden.
- EFFECTS-013: Qualitätsänderungen DÜRFEN keine ungültigen Surface- oder Buffer-Zustände verursachen.
- EFFECTS-014: Die Qualitätssteuerung DARF den kritischen Rendering-Pfad nicht blockieren.
- EFFECTS-015: Entscheidungen und Qualitätswechsel MÜSSEN diagnostizierbar sein.
- EFFECTS-016: Bei Ausfall des Controllers MUSS ein definierter, ressourcenschonender Standardzustand verwendet werden.

## 5. Qualitätsstufen

| Stufe | Beschreibung |
|---|---|
| Ultra | Maximale Effektqualität innerhalb der Hardwaregrenzen |
| High | Hochwertige Effekte mit moderater Optimierung |
| Balanced | Ausgewogenes Verhältnis zwischen Qualität und Leistung |
| Low | Vereinfachte Effekte und reduzierte Berechnungsauflösung |
| Minimal | Nur notwendige visuelle Rückmeldungen |

Die Qualitätsstufen definieren Obergrenzen. Einzelne Effekte DÜRFEN unabhängig davon effizientere Rendering-Verfahren verwenden.

## 6. Adaptive Steuerung

Der Adaptive Quality Controller berücksichtigt:

- Tatsächliche Frame-Zeiten.
- Verfügbare Zeit bis zur nächsten Präsentationsdeadline.
- CPU- und GPU-Auslastung.
- Speicherverbrauch und Speicherbandbreite, sofern messbar.
- Anzahl und Komplexität aktiver Effekte.
- Displayauflösung und Bildwiederholrate.
- Energieprofil und thermische Einschränkungen.
- Benutzerdefinierte Qualitätsvorgaben.

### Qualitätsreduzierung

Bei anhaltender Überlastung werden bevorzugt:

1. Unnötige Effektberechnungen vermieden.
2. Zwischenergebnisse stärker wiederverwendet.
3. Effektauflösungen reduziert.
4. Unschärferadien und Schattenqualität vereinfacht.
5. Dekorative Animationen reduziert.
6. Nicht notwendige Effekte deaktiviert.

### Qualitätswiederherstellung

Bei stabiler Leistungsreserve werden reduzierte Effekte schrittweise wieder aktiviert.

Für Reduzierung und Wiederherstellung MÜSSEN unterschiedliche Schwellenwerte und geeignete Beobachtungsintervalle verwendet werden.

## 7. Verarbeitungsablauf

1. **Measure:** Rendering-Zeiten und Ressourcenverbrauch erfassen.
2. **Evaluate:** Leistungsreserve und Engpässe bestimmen.
3. **Policy:** Benutzerpräferenzen und Systemrichtlinien berücksichtigen.
4. **Select:** Geeignete Qualitätsstufen ermitteln.
5. **Apply:** Änderungen an betroffene Effektmodule übergeben.
6. **Validate:** Darstellung und Ressourcenverbrauch überwachen.
7. **Stabilize:** Weitere Änderungen bis zur nächsten Bewertung begrenzen.
8. **Recover:** Qualität bei ausreichender Leistungsreserve wiederherstellen.

## 8. Performance und Zuverlässigkeit

- Geringer Eigenaufwand des Adaptive Quality Controllers.
- Keine blockierenden Messungen im Rendering-Pfad.
- Verwendung geglätteter Leistungswerte.
- Hysterese zur Vermeidung sichtbarer Qualitätsschwankungen.
- Begrenzung zusätzlicher Speicheranforderungen.
- Priorisierung niedriger Eingabelatenzen.
- Unterstützung unterschiedlicher Display-Frequenzen.
- Berücksichtigung des frühen Desktopstarts.
- Sofortiger Minimalmodus bei kritischer Ressourcenknappheit.
- Definierter Fallback bei fehlenden Leistungsdaten.

## 9. Ergebnis

NovaOS erhält eine adaptive Qualitätssteuerung für sämtliche dekorativen Compositor-Effekte.

Die Darstellung passt sich automatisch an Hardwareleistung, Systemlast und Energiezustand an.

Leistungsreserven werden für hochwertige Effekte genutzt, während bei Ressourcenknappheit die Reaktionsfähigkeit des Desktops Vorrang erhält.

**Grundsatz:** Flüssige Bedienung und Systemstabilität haben immer Vorrang vor dekorativer Effektqualität.