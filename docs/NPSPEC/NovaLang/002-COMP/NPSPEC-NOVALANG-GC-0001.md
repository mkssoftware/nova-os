
# NPSPEC-NOVALANG-GC-0001 – NovaLang Garbage Collector

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Garbage Collection

## Zweck

Definiert den Garbage Collector (GC) zur automatischen Verwaltung nicht mehr erreichbarer Objekte im Managed Heap.

Ziel sind geringe Speicherbelastung, kurze Unterbrechungen und sichere Objektlebenszeiten.

## Architektur

NovaLang verwendet einen **generationalen, inkrementellen Garbage Collector** mit optional nebenläufiger Verarbeitung.

| Komponente | Aufgabe |
|---|---|
| Allocator | Schnelle Objektallokation |
| Young Generation | Verwaltung kurzlebiger Objekte |
| Old Generation | Verwaltung langlebiger Objekte |
| Root Scanner | Ermittlung erreichbarer Objekte |
| Marking | Markierung lebender Objekte |
| Sweeping | Freigabe nicht erreichbarer Objekte |
| Compaction | Optionale Speicherverdichtung |
| Write Barrier | Erfassung relevanter Referenzänderungen |

Die Runtime darf GC-Strategien abhängig von Hardware und Ausführungskontext konfigurieren.

## Speicherbereinigung

1. Speicheranforderung im Managed Heap durchführen.
2. GC bei Bedarf oder aufgrund definierter Grenzwerte auslösen.
3. Erreichbare Objekte über GC-Roots ermitteln.
4. Nicht erreichbare Objekte identifizieren.
5. Speicher sicher freigeben.
6. Speicherbereiche bei Bedarf verdichten.

Zirkuläre Referenzen werden durch Erreichbarkeitsanalyse erkannt und bereinigt.

## GC-Roots

Zu den GC-Roots gehören:

- Aktive Stack-Frames und Register
- Statische Referenzen
- Runtime-interne Referenzen
- Aktive Task- und Async-Zustände
- Explizit registrierte native Handles

AOT, JIT und Interpreter müssen gültige Referenzinformationen bereitstellen.

## Performance

Der GC unterstützt:

- Schnelle Allokation kurzlebiger Objekte
- Inkrementelle Bereinigung
- Optionale parallele und nebenläufige Verarbeitung
- Konfigurierbare Heap-Grenzen
- Anpassung an verfügbaren Arbeitsspeicher
- Speicherstatistiken und Diagnostik

Unterbrechungszeiten sollen möglichst gering bleiben.

## Objektlebenszeiten

- Nicht mehr erreichbare Objekte dürfen automatisch freigegeben werden.
- Schwache Referenzen halten Objekte nicht künstlich am Leben.
- Objektverschiebungen müssen für sicheren NovaLang-Code transparent bleiben.
- Native Zugriffe auf verschiebbare Objekte benötigen kontrolliertes Pinning.
- Externe Ressourcen werden über `Dispose` und `Using` freigegeben.

Der GC garantiert keinen bestimmten Zeitpunkt der Objektfreigabe.

## Deterministische Ausführung

Für zeitkritische oder deterministische Ausführungskontexte muss die Runtime kontrollierbare GC-Modi bereitstellen.

Dazu gehören begrenzte Allokationsbudgets und definierte GC-Unterbrechungspunkte.

Eine vollständige Unterdrückung der Speicherbereinigung ist nur zulässig, wenn ausreichende Speichergrenzen gewährleistet sind.

## NovaOS-Integration

- Der GC respektiert die Speicherbudgets von NovaOS.
- Schutzdomänen dürfen keine unautorisierten Objektreferenzen austauschen.
- Speicherknappheit muss kontrolliert behandelt werden.
- GC-Aktivitäten müssen diagnostizierbar sein.
- Logic-Graph-Komponenten unterliegen ihren jeweiligen Ressourcenlimits.

## Normative Anforderungen

1. NovaLang MUSS einen automatischen Garbage Collector bereitstellen.
2. Der GC MUSS generationales und inkrementelles Arbeiten unterstützen.
3. Zirkuläre Referenzen MÜSSEN korrekt behandelt werden.
4. AOT, JIT und Interpreter MÜSSEN GC-Roots zuverlässig bereitstellen.
5. Objektverschiebungen DÜRFEN keine gültigen Referenzen beschädigen.
6. Speicher- und Ressourcenlimits MÜSSEN eingehalten werden.
7. Externe Ressourcen DÜRFEN NICHT ausschließlich vom GC abhängig sein.
8. Deterministische Ausführungskontexte MÜSSEN kontrollierbare GC-Unterbrechungen ermöglichen.
9. Der GC DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält einen generationalen, inkrementellen und optional nebenläufigen Garbage Collector mit kurzen Unterbrechungen, sicherer Referenzverwaltung und anpassbarem Ressourcenverbrauch für NovaOS.
