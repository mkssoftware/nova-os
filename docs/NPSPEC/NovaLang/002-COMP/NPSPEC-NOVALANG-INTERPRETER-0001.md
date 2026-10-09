
# NPSPEC-NOVALANG-INTERPRETER-0001 – NovaLang Interpreter

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Interpreter

## Zweck

Definiert den nativen NovaLang Interpreter zur direkten Ausführung von verifiziertem Nova Bytecode innerhalb der Nova VM.

Er ermöglicht schnelle Programmstarts, geringe Ressourcenanforderungen und die Ausführung ohne JIT oder native Codegenerierung.

## Architektur

Der Interpreter ist eine eigenständige Ausführungskomponente der Nova VM.

| Komponente | Aufgabe |
|---|---|
| Instruction Decoder | Dekodierung der Bytecode-Instruktionen |
| Dispatch Engine | Auswahl und Ausführung der Operationen |
| Register Context | Verwaltung virtueller Register |
| Call Stack | Funktionsaufrufe und Rücksprünge |
| Runtime Bridge | Objekte, Speicher, Exceptions und Tasks |
| Capability Bridge | Autorisierte Systemoperationen |

Der Interpreter verwendet die gemeinsame NovaLang Runtime.

## Ausführungsmodell

Der Interpreter arbeitet registerbasiert.

1. Verifiziertes Bytecode-Modul übernehmen.
2. Execution Context initialisieren.
3. Instruktion anhand des Program Counters lesen.
4. Operanden aus virtuellen Registern auswerten.
5. Operation ausführen und Ergebnis speichern.
6. Kontrollfluss und Abbruchbedingungen prüfen.
7. Bis zur Rückgabe oder Beendigung fortfahren.

Funktionsaufrufe erzeugen definierte Aufrufrahmen.

## Sprachsemantik

Der Interpreter unterstützt sämtliche gültigen Nova-Bytecode-Instruktionen, insbesondere:

- Arithmetik, Vergleiche und Konvertierungen
- Kontrollfluss und Funktionsaufrufe
- Objekte, Collections und Generics
- Exceptions und Stack-Unwinding
- Async/Await und Task-Fortsetzung
- Kontrollierte Capability-Aufrufe

Das beobachtbare Verhalten muss mit AOT- und JIT-Ausführung übereinstimmen.

## Performance

Der Interpreter soll folgende Optimierungen ermöglichen:

- Effizienten Instruction Dispatch
- Vorab dekodierte Instruktionen
- Wiederverwendung verifizierter Module
- Minimale Register- und Stackverwaltung
- Lazy Loading benötigter Abhängigkeiten

Optimierungen dürfen keine Sicherheitsprüfungen umgehen.

## Sicherheit und Ressourcen

- Ausschließlich verifizierter Bytecode darf ausgeführt werden.
- Virtuelle Register und Speicherreferenzen müssen gültig sein.
- Systemzugriffe benötigen autorisierte Capability-Handles.
- CPU-, Speicher- und Instruktionsbudgets müssen kontrollierbar sein.
- Cancellation und Laufzeitfehler müssen sicher behandelt werden.
- Unterschiedliche Vertrauensbereiche benötigen geeignete NovaOS-Isolation.

## Deterministische Ausführung

Der Interpreter unterstützt einen deterministischen Modus mit definierter Instruktionssemantik und kontrollierten externen Einflüssen.

Instruktionszählung und Ausführungsprotokollierung sollen Debugging, Tests und reproduzierbare Simulationen ermöglichen.

## Normative Anforderungen

1. Der Interpreter MUSS gültigen Nova Bytecode ohne JIT ausführen können.
2. Er MUSS das registerbasierte Ausführungsmodell der Nova VM verwenden.
3. Er MUSS die vollständige definierte Bytecode-Semantik implementieren.
4. Nicht verifizierter Bytecode DARF NICHT ausgeführt werden.
5. Speicher- und Capability-Sicherheitsregeln MÜSSEN eingehalten werden.
6. Ressourcenlimits und kontrollierter Abbruch MÜSSEN unterstützt werden.
7. Ein deterministischer Ausführungsmodus MUSS verfügbar sein.
8. Der Interpreter MUSS unabhängig von .NET funktionieren.

## Ergebnis

NovaLang erhält einen kompakten, sicheren und ressourcenschonenden Bytecode-Interpreter als zuverlässige Ausführungsbasis der Nova VM, insbesondere für schwache Hardware, Solutions und Logic-Graph-Komponenten.
