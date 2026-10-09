
# NPSPEC-NOVALANG-JIT-0001 – NovaLang Just-in-Time Compiler

## Status

Angenommen

## Kategorie

NovaLang / Runtime / JIT

## Zweck

Definiert den nativen Just-in-Time-Compiler (JIT) von NovaLang. Er übersetzt verifizierten Nova Bytecode während der Programmausführung in optimierten Maschinencode.

Ziel sind hohe Ausführungsgeschwindigkeit, kurze Startzeiten und geringer Ressourcenverbrauch.

## Architektur

Der JIT ist eine optionale Komponente der Nova VM.

| Komponente | Aufgabe |
|---|---|
| Profiler | Erkennung häufig ausgeführter Codebereiche |
| Compilation Manager | Steuerung der JIT-Kompilierung |
| IR Translator | Überführung von Bytecode in JIT-IR |
| Optimizer | Laufzeitbasierte Optimierungen |
| Code Generator | Erzeugung nativen Maschinencodes |
| Code Cache | Verwaltung kompilierter Funktionen |
| Deoptimizer | Rückkehr zu allgemeinerem Code |

Ohne JIT übernimmt der Interpreter die vollständige Ausführung.

## Kompilierungsstrategie

NovaLang verwendet ein mehrstufiges Ausführungsmodell:

1. Bytecode verifizieren.
2. Programm zunächst interpretieren.
3. Häufig ausgeführte Funktionen identifizieren.
4. Funktionen bei ausreichendem Nutzen nativ kompilieren.
5. Kompilierten Code im Code Cache speichern.
6. Bei ungültigen Optimierungsannahmen kontrolliert zurückfallen.

Der JIT darf kleine oder selten ausgeführte Funktionen dauerhaft interpretieren.

## Optimierungen

Der JIT unterstützt insbesondere:

- Inlining
- Constant Folding
- Dead Code Elimination
- Register Allocation
- Loop Optimization
- Devirtualisierung
- Typspezialisierung
- Profilgesteuerte Optimierung

Spekulative Optimierungen müssen durch Laufzeitprüfungen abgesichert werden.

## Deoptimierung

Werden Optimierungsannahmen ungültig, muss der JIT einen gültigen Ausführungszustand wiederherstellen können.

Die Ausführung darf anschließend im Interpreter oder in weniger spezialisiertem Maschinencode fortgesetzt werden.

## Speicher und Sicherheit

- Generierter Code muss die NovaLang-Speichersemantik einhalten.
- Beschreibbare und ausführbare Speicherseiten dürfen nicht gleichzeitig freigegeben sein (W^X).
- Capability-Prüfungen dürfen nicht umgangen werden.
- Code Cache und JIT-Speicher müssen Ressourcenlimits unterliegen.
- Nicht verifizierter Bytecode darf nicht kompiliert werden.
- Native Codeausführung muss innerhalb der vorgesehenen NovaOS-Isolationsgrenzen erfolgen.

## Performance und Ressourcen

Der JIT muss konfigurierbare Kompilierungsbudgets unterstützen.

Auf leistungsschwacher Hardware darf er vollständig deaktiviert werden.

Die Runtime soll zwischen Startzeit, Speicherverbrauch und langfristiger Ausführungsgeschwindigkeit abwägen.

## Determinismus

Im deterministischen Ausführungsmodus müssen JIT-Optimierungen die definierte Programmsemantik erhalten.

Zeitabhängige Profilentscheidungen dürfen die funktionalen Ergebnisse nicht beeinflussen. Wo reproduzierbare Ausführungsabläufe erforderlich sind, muss der JIT deaktivierbar sein.

## Normative Anforderungen

1. Der JIT MUSS verifizierten Nova Bytecode in nativen Maschinencode übersetzen können.
2. Der JIT MUSS optional und vollständig deaktivierbar sein.
3. Interpreter und JIT MÜSSEN dieselbe beobachtbare Sprachsemantik einhalten.
4. Laufzeitbasierte Optimierungen MÜSSEN abgesichert sein.
5. Ungültige Optimierungsannahmen MÜSSEN kontrolliert behandelt werden.
6. Generierter Code MUSS Speicher- und Capability-Sicherheitsregeln einhalten.
7. Der Code Cache MUSS Ressourcenlimits unterstützen.
8. Der JIT MUSS eine erweiterbare Architektur für unterschiedliche Prozessoren besitzen.
9. Der JIT DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält einen optionalen, profilgesteuerten JIT-Compiler mit mehrstufiger Optimierung, sicherer Deoptimierung und kontrolliertem Ressourcenverbrauch. Der Interpreter bleibt jederzeit als vollständiger Ausführungspfad verfügbar.
