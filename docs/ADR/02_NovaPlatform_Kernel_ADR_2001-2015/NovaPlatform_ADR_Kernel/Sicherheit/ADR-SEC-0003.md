# ADR-SEC-0003 – Stack Protector

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0003 |
| Titel | Stack Protector |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Kernel-Sicherheit |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0004 |
| Nachfolgende Spezifikation | NPSPEC-SEC-0003 |

---

## 1. Kontext

Stack-basierte Speicherfehler können Rücksprungadressen, Frame Pointer oder andere sicherheitsrelevante Daten überschreiben. Besonders im Kernel können solche Fehler zur Ausführung privilegierten Codes oder zur vollständigen Übernahme des Systems führen.

NovaOS benötigt deshalb einen Stack Protector, der entsprechende Manipulationen möglichst früh erkennt.

## 2. Problemstellung

Der Schutz muss:

- für Kernel und sicherheitskritische Systemkomponenten verfügbar sein,
- Manipulationen vor der Rückkehr aus einer Funktion erkennen,
- pro Systemstart nicht vorhersehbare Schutzwerte verwenden,
- mit mehreren Prozessoren und Threads funktionieren,
- einen sicheren Fehlerpfad auslösen,
- mit unterstützten Compilern kompatibel sein.

Der Stack Protector ersetzt keine Speichersicherheit, reduziert jedoch die Ausnutzbarkeit bestimmter Fehlerklassen.

## 3. Entscheidung

NovaOS verwendet compilerbasierte Stack Canaries.

Für den Kernel wird der Stack Protector standardmäßig aktiviert. Sicherheitskritische Userspace-Komponenten werden ebenfalls mit aktiviertem Stack Protector gebaut.

Mindestens folgende Build-Option wird vorausgesetzt:

```text
-fstack-protector-strong
```

Wenn Architektur, Compiler und Laufzeitumgebung es zuverlässig unterstützen, darf ein stärkerer Modus eingesetzt werden:

```text
-fstack-protector-all
```

Die konkrete Auswahl wird durch das Build-Profil und die unterstützte Toolchain bestimmt.

## 4. Stack Canary

Der Compiler platziert einen Schutzwert zwischen lokalen Stack-Daten und kontrollflussrelevanten Daten.

Vor dem Verlassen einer geschützten Funktion wird geprüft, ob dieser Wert unverändert ist.

```c
void protected_function(void)
{
    uintptr_t expected = stack_guard;

    /* Funktionslogik */

    if (stack_guard != expected) {
        __stack_chk_fail();
    }
}
```

Die tatsächliche Instrumentierung erfolgt durch den Compiler und muss nicht manuell implementiert werden.

## 5. Initialisierung

Der Stack Canary wird während des Kernel-Starts initialisiert, bevor nicht vertrauenswürdige oder komplexe Eingaben verarbeitet werden.

Die Entropiequellen werden in dieser Reihenfolge bevorzugt:

1. Hardware-Zufallszahlengenerator,
2. Bootloader- oder Firmware-Entropie,
3. CPU- und Plattformzustände,
4. interne Entropiesammlung als Rückfalllösung.

Ein statischer oder ausschließlich aus konstanten Werten abgeleiteter Canary ist unzulässig.

## 6. Canary-Gültigkeitsbereich

NovaOS verwendet abhängig von der Architektur vorzugsweise:

- einen Canary pro Thread,
- alternativ einen Canary pro CPU,
- während der frühen Bootphase vorübergehend einen globalen Boot-Canary.

Sobald die Thread-Infrastruktur verfügbar ist, muss auf den regulären Schutzwert umgestellt werden.

## 7. Thread-Integration

Der Schutzwert wird im architekturspezifischen Thread-Kontext oder in einer sicher erreichbaren Thread-Control-Struktur gespeichert.

```c
typedef struct np_thread_security {
    uintptr_t stack_canary;
    uint32_t flags;
} np_thread_security_t;
```

Beim Erzeugen eines Threads wird ein neuer Canary generiert. Das bloße Kopieren des Canarys des erzeugenden Threads ist nicht vorgesehen.

## 8. Architekturspezifischer Zugriff

Der Zugriff auf den Canary darf über ein architekturspezifisches Register oder einen geschützten Speicherbereich erfolgen.

Beispiele sind:

- segmentbasierter Zugriff auf x86,
- Thread-Pointer-Register auf geeigneten Architekturen,
- CPU-lokaler Speicher während früher Bootphasen.

Die generische Kernel-Schicht darf keine festen Register oder Speicheradressen voraussetzen.

## 9. Fehlerbehandlung

Eine fehlgeschlagene Stack-Prüfung ruft unmittelbar folgende Funktion auf:

```c
_Noreturn void __stack_chk_fail(void);
```

Der Fehlerpfad muss:

- weitere Ausführung der betroffenen Funktion verhindern,
- Interrupt- und Rekursionseffekte begrenzen,
- ein minimales Diagnoseereignis erzeugen,
- sensible Stack-Inhalte nicht unkontrolliert ausgeben,
- die Kernel-Panic-Policy anwenden.

Eine Rückkehr aus `__stack_chk_fail()` ist unzulässig.

## 10. Diagnoseinformationen

Das Diagnoseereignis darf folgende Daten enthalten:

- betroffene CPU,
- Thread- und Prozesskennung,
- Instruction Pointer,
- Kernelmodul oder Komponente,
- Fehlerklasse `STACK_CANARY_CORRUPTION`.

Die Ausgabe vollständiger Stack-Inhalte ist nur in einem autorisierten sicheren Debug-Modus zulässig.

## 11. Kernelmodule und Treiber

Alle Kernelmodule und Kernel-Treiber müssen mit einer kompatiblen Stack-Protector-Konfiguration gebaut werden.

Ein Modul darf nicht geladen werden, wenn:

- erforderliche Stack-Protector-Symbole fehlen,
- seine ABI nicht zur Kernel-Konfiguration passt,
- vorgeschriebene Sicherheitsoptionen deaktiviert wurden.

Userspace-Treiber unterliegen den Schutzanforderungen ihres jeweiligen Sicherheitsprofils.

## 12. Frühe Bootphase

Vor der Initialisierung der regulären Entropie- und Thread-Systeme wird ein temporärer Boot-Canary verwendet.

Dieser muss:

- so früh wie technisch möglich erzeugt werden,
- nach Verfügbarkeit besserer Entropie ersetzt werden,
- nach seiner Ablösung nicht weiter für neue Threads verwendet werden.

Kann kein angemessener Schutzwert erzeugt werden, muss NovaOS diesen Zustand als reduzierte Sicherheitsstufe kennzeichnen.

## 13. Schutzgrenzen

Der Stack Protector schützt nicht vollständig gegen:

- Heap-Überläufe,
- Use-after-free-Fehler,
- rein datenorientierte Angriffe,
- Überschreibungen ohne Canary-Berührung,
- Informationslecks des Canary-Werts,
- fehlerhafte Kontrollflüsse außerhalb geschützter Funktionen.

Er wird deshalb mit NX Memory, Kernel ASLR, Capability-Prüfungen und weiteren Compiler-Härtungen kombiniert.

## 14. Build-Konfiguration

Produktive Kernel-Builds dürfen den Stack Protector nicht global deaktivieren.

Ausnahmen sind nur für eng begrenzte Funktionen zulässig, wenn die Instrumentierung technisch unmöglich ist, beispielsweise:

- früheste Startsequenzen,
- Context-Switch-Assembler,
- Interrupt-Einstiegscode,
- spezielle Laufzeitfunktionen des Stack Protectors.

Jede Ausnahme muss im Quellcode begründet und überprüfbar markiert werden.

```c
NP_NO_STACK_PROTECTOR
void arch_early_entry(void);
```

## 15. Sicherheitsanforderungen

Folgende Anforderungen sind verbindlich:

- Canaries dürfen nicht als öffentliche API verfügbar sein.
- Diagnoseausgaben dürfen den Canary nicht offenlegen.
- Crash Dumps müssen Canary-Werte als sensible Daten behandeln.
- Neue Threads müssen eigene Schutzwerte erhalten.
- Ein erkannter Fehler darf nicht ignoriert werden.
- Release-Builds müssen die Schutzinstrumentierung verifizieren.

## 16. Alternativen

### 16.1 Kein Stack Protector

Abgelehnt, da einfache Stack-Überläufe leichter zur Kontrollflussübernahme führen könnten.

### 16.2 Globaler konstanter Canary

Abgelehnt, da ein bekannter oder wiederverwendbarer Wert den Schutz erheblich schwächt.

### 16.3 Ausschließlich manuelle Prüfungen

Abgelehnt, da sie fehleranfällig sind und nicht zuverlässig alle relevanten Funktionen abdecken.

### 16.4 Instrumentierung jeder Funktion

Als optionales Härtungsprofil zulässig, aber nicht zwingend für alle Plattformen, da Codegröße und Laufzeitkosten steigen.

## 17. Konsequenzen

### 17.1 Vorteile

- frühzeitige Erkennung vieler Stack-Überschreibungen,
- erschwerte Kontrollflussübernahme,
- geringer Entwicklungsaufwand durch Compilerunterstützung,
- einheitliche Fehlerbehandlung.

### 17.2 Nachteile

- zusätzliche Instruktionen bei Funktionsaufruf und Rückkehr,
- leicht erhöhte Stack- und Codegrößenanforderungen,
- Abhängigkeit von Compiler und ABI,
- zusätzlicher Initialisierungsaufwand während des Bootvorgangs.

## 18. Implementierungsrichtlinien

- Stack-Protector-Unterstützung muss bereits beim Toolchain-Bootstrap berücksichtigt werden.
- Die Laufzeitfunktionen müssen Bestandteil des Kernel-Kerns sein.
- Der Fehlerpfad darf selbst nicht stack-protector-abhängig sein.
- Canary-Speicherorte müssen vor unprivilegiertem Zugriff geschützt werden.
- Architekturcode muss den Wechsel von CPU- oder Thread-Kontexten korrekt berücksichtigen.
- Link- und Build-Prüfungen müssen ungeschützte Objekte erkennen.

## 19. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. kontrollierte Canary-Manipulation löst `__stack_chk_fail()` aus,
2. normale Funktionen verursachen keine Fehlalarme,
3. jeder neu erzeugte Thread erhält einen gültigen Canary,
4. Kernelmodule verwenden die erwartete Schutzkonfiguration,
5. Diagnoseausgaben enthalten keinen Canary-Wert,
6. frühe Bootpfade funktionieren mit dem temporären Canary,
7. Release-Builds enthalten nachweisbare Stack-Protector-Instrumentierung,
8. der Fehlerpfad endet entsprechend der Kernel-Panic-Policy.

## 20. Verbindliche Invarianten

1. Ein erkannter Canary-Fehler führt niemals zur normalen Ausführung zurück.
2. Produktive Kernel-Builds verwenden Stack-Protector-Instrumentierung.
3. Canary-Werte werden nicht über öffentliche Schnittstellen offengelegt.
4. Neue Threads erhalten unabhängig erzeugte Schutzwerte.
5. Der Fehlerhandler benötigt keinen funktionierenden Zustand des beschädigten Stacks.
6. Nicht instrumentierbare Funktionen bleiben auf das technisch notwendige Minimum begrenzt.

## 21. Zusammenfassung

NovaOS verwendet compilerbasierte Stack Canaries, um Stack-Überschreibungen im Kernel und in sicherheitskritischen Komponenten frühzeitig zu erkennen. Die Schutzwerte werden während des Bootvorgangs aus geeigneter Entropie erzeugt und vorzugsweise pro Thread verwaltet.

Eine erkannte Beschädigung führt unmittelbar in einen nicht rückkehrenden, diagnosefähigen Fehlerpfad und wird gemäß der Kernel-Panic-Policy behandelt.