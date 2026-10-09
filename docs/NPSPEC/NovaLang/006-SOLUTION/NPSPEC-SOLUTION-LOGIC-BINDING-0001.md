
# NPSPEC-SOLUTION-LOGIC-BINDING-0001 – NovaOS Solution Logic Binding

## Status

Angenommen

## Kategorie

NovaOS / Solution / Logic Binding

## Zweck

Definiert die verbindliche Zuordnung zwischen einer NovaOS-Solution, ihren Logic Graphs, der deklarativen Benutzeroberfläche und den verwendeten Capabilities.

Ziel ist eine typsichere, nachvollziehbare und sichere Kommunikation zwischen Benutzeroberfläche und Ausführungslogik.

## Architektur

Das Logic Binding besteht aus:

- **Binding Manager:** Verwaltung der Bindungen.
- **Binding Registry:** Registrierung und Auflösung von Bindings.
- **Type Resolver:** Prüfung der NovaLang-Datentypen.
- **Event Bridge:** Weiterleitung von UI-Ereignissen an Logic Graphs.
- **State Synchronizer:** Synchronisation gebundener Zustände.
- **Capability Resolver:** Auflösung benötigter Systemfähigkeiten.

Die Ausführung erfolgt über die bestehende Solution Logic Runtime.

## Binding-Modell

Ein Binding definiert:

- Eindeutige Binding-ID
- Quell- und Zielreferenz
- NovaLang-Datentyp
- Bindungsrichtung
- Optionalen Standardwert
- Aktualisierungsregeln

Unterstützte Bindungsrichtungen:

- **OneWay:** Quelle aktualisiert Ziel.
- **TwoWay:** Änderungen werden bidirektional synchronisiert.
- **OneTime:** Einmalige Wertübertragung.
- **Event:** Ereignis löst einen definierten Logic-Graph-Einstiegspunkt aus.

## UI-Integration

Deklarative `.nui`-Oberflächen können Eigenschaften, Zustände und Ereignisse mit Logic Graphs verbinden.

UI-Elemente greifen nicht unmittelbar auf privilegierte Systemfunktionen zu.

Systemoperationen erfolgen ausschließlich über autorisierte Capability Nodes im Logic Graph.

## Datenfluss

Bindings verwenden das reguläre NovaLang-Typsystem.

Typinkompatible Verbindungen müssen bei der Validierung erkannt werden.

Zyklische TwoWay-Bindings müssen durch definierte Änderungs- und Konfliktregeln kontrolliert werden.

## Zustandsverwaltung

Gebundene Zustände werden über den Solution State Manager verwaltet.

Aktualisierungen erfolgen konsistent und werden an betroffene UI-Elemente weitergegeben.

Asynchrone Ergebnisse dürfen die Benutzeroberfläche nicht blockieren.

## Sicherheit

Bindings gelten ausschließlich innerhalb des autorisierten Solution-Kontexts.

Ein Binding darf keine Capability-Berechtigungen erzeugen oder erweitern.

Custom Scripts erhalten ausschließlich die über den Logic Graph bereitgestellten Daten und Ressourcen.

## Normative Anforderungen

1. Jede Solution MUSS typisierte Logic Bindings unterstützen können.
2. Bindings MÜSSEN eindeutige IDs und gültige Referenzen besitzen.
3. OneWay, TwoWay, OneTime und Event MÜSSEN unterstützt werden.
4. UI-Ereignisse MÜSSEN definierte Graph-Einstiegspunkte auslösen können.
5. Binding-Typen MÜSSEN anhand des NovaLang-Typsystems geprüft werden.
6. Zustandsänderungen MÜSSEN konsistent synchronisiert werden.
7. Zyklische Bindings MÜSSEN kontrolliert behandelt werden.
8. Asynchrone Aktualisierungen DÜRFEN die UI nicht blockieren.
9. Ungültige Bindings MÜSSEN diagnostizierbar sein.
10. Bindings DÜRFEN keine Capability- oder Sicherheitsgrenzen umgehen.
11. Die Binding-Ausführung MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine typsichere und sichere Binding-Infrastruktur, die deklarative Benutzeroberflächen, Logic Graphs und Solution-Zustände miteinander verbindet.
