# ADR-BOOTUI-0012 – Deterministisches Rendering ohne dynamische Systemabhängigkeiten

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0012 |
| Titel | Deterministisches Rendering ohne dynamische Systemabhängigkeiten |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Der Bootmanager läuft vor dem vollständigen Betriebssystem und muss unter kontrollierten Bedingungen reproduzierbar funktionieren. Dynamische Bibliotheken, externe Dienste, nicht deterministische Speicherallokationen oder unkontrollierte Nebenläufigkeit erhöhen das Risiko von Startfehlern.

## 2. Entscheidung

Das Rendering der Boot-UI wird deterministisch und ohne dynamische Systemabhängigkeiten implementiert.

Verpflichtende Regeln:

- keine dynamisch geladenen Bibliotheken,
- keine Netzwerkabhängigkeit für Kernressourcen,
- keine vom laufenden Betriebssystem bereitgestellten Dienste,
- feste oder begrenzte Speicherpools,
- monotone und normalisierte Zeitquelle,
- definierte Reihenfolge aller Render- und Updatephasen,
- keine unkontrollierten Hintergrundthreads,
- validierte und versionierte Ressourcen,
- reproduzierbare Fallback-Entscheidungen.

## 3. Technische Ausgestaltung

### Determinismus

Bei identischem Zustand, identischen Ressourcen, identischer Zeitfolge und identischem Backend muss dasselbe logische Frame-Ergebnis entstehen.

### Speicher

Dauerhafte Daten werden in Arenen oder Pools verwaltet. Temporäre Frame-Daten verwenden zurücksetzbare Frame-Arenen. Allokationsfehler müssen vorhersagbar behandelt werden.

### Zeit

Animationen basieren auf einer monotonen Uhr. Große Zeitsprünge werden geklemmt. Eine Animation darf nicht vom tatsächlichen CPU-Takt oder der Anzahl gerenderter Frames abhängen.

### Ressourcen

Alle Ressourcen besitzen Größen-, Typ-, Versions- und Integritätsinformationen. Fehlerhafte Ressourcen führen zu einem bekannten Ersatzobjekt, nicht zu undefiniertem Verhalten.

### Parallelität

Die Baseline ist single-threaded. Spätere Parallelisierung darf nur hinter klaren Synchronisationspunkten erfolgen und das sichtbare Ergebnis nicht verändern.

## 4. Konsequenzen

### Positive Folgen

- hohe Zuverlässigkeit und Reproduzierbarkeit,
- einfachere Tests und Fehleranalyse,
- kontrollierter Speicherverbrauch,
- geringere Angriffsfläche,
- stabile Animationen unabhängig von der Framerate.

### Negative Folgen

- weniger Flexibilität,
- einige Optimierungen und dynamische Erweiterungen entfallen,
- strikte Ressourcen- und Speicherplanung erforderlich.

## 5. Verworfene Alternativen

- **Dynamische Laufzeitabhängigkeiten:** im frühen Boot zu riskant.
- **Freie Heap-Nutzung:** schwer vorhersagbarer Speicherverbrauch.
- **Framebasierte Animationen:** abhängig von Leistung und Bildrate.
- **Mehrere Renderingthreads als Standard:** unnötige Komplexität.

## 6. Validierung und Abnahmekriterien

- reproduzierbare Render-Hashes für definierte Szenen,
- Allokationsfehler und erschöpfte Pools,
- manipulierte oder beschädigte Ressourcen,
- Zeitquellensprünge,
- wiederholte Ausführung auf Emulator und realer Hardware.

## 7. Sicherheits- und Robustheitsanforderungen

- Ein Fehler innerhalb der grafischen Oberfläche darf den eigentlichen Bootvorgang nicht dauerhaft blockieren.
- Alle extern oder aus dem Dateisystem geladenen Daten müssen vor der Nutzung validiert werden.
- Speicherzugriffe müssen auf reservierte und überprüfte Bereiche begrenzt bleiben.
- Für jede optionale Funktion muss ein definierter Fallback existieren.
- Diagnoseinformationen dürfen keine geheimen Schlüssel, Passwörter oder geschützten Bootparameter offenlegen.

## 8. Abhängigkeiten

- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
- ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements

## 9. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
| 1.0 | 2026-08-03 | Erstfassung |
