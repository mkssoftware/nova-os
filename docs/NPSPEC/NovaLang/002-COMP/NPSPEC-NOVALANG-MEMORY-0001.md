
# NPSPEC-NOVALANG-MEMORY-0001 – NovaLang Memory Management

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Speicherverwaltung

## Zweck

Definiert die sichere und automatische Speicherverwaltung von NovaLang für native Programme, Bytecode, Solutions und Systemkomponenten.

Ziel sind Speichersicherheit, geringer Ressourcenverbrauch und vorhersehbares Laufzeitverhalten.

## Speichermodell

NovaLang unterscheidet:

| Speicherbereich | Verwendung |
|---|---|
| Stack | Lokale Werte und Aufrufrahmen |
| Managed Heap | Automatisch verwaltete Objekte |
| Static Storage | Globale und statische Daten |
| Native Memory | Explizit verwalteter Speicher |
| Shared Memory | Kontrolliert gemeinsam genutzter Speicher |

Die konkrete Speicherplatzierung darf durch Compiler und Runtime optimiert werden, sofern die Sprachsemantik erhalten bleibt.

## Objektverwaltung

- Referenztypen besitzen eine definierte Objektidentität.
- Werttypen folgen ihrer definierten Kopiersemantik.
- Verwaltete Objekte werden automatisch freigegeben, sobald sie nicht mehr erreichbar sind.
- Zirkuläre Referenzen müssen korrekt behandelt werden.
- Referenzen dürfen nicht auf bereits freigegebenen Speicher zugreifen.
- Objektverschiebungen durch die Speicherverwaltung dürfen gültige Referenzen nicht beschädigen.

## Garbage Collection

Die Runtime verwendet einen automatischen Garbage Collector für verwaltete Objekte.

Vorgesehene Eigenschaften:

- Inkrementelle beziehungsweise nebenläufige Speicherbereinigung
- Möglichst kurze Unterbrechungen
- Anpassung an verfügbare Systemressourcen
- Unterstützung für schwache Referenzen
- Konfigurierbare Speichergrenzen

Die konkrete GC-Strategie wird separat spezifiziert.

## Deterministische Ressourcenfreigabe

Externe Ressourcen werden unabhängig vom Garbage Collector freigegeben.

```vb
Using datei As Stream = DateiOeffnen()
    datei.Write(daten)
End Using
```

`Using` garantiert den Aufruf von `Dispose`, auch bei Exceptions.

Die automatische Speicherbereinigung ersetzt keine deterministische Freigabe von Dateien, Handles oder anderen Systemressourcen.

## Native Speicherzugriffe

Direkte Speicheroperationen sind ausschließlich in ausdrücklich zugelassenen Unsafe-Kontexten erlaubt.

- Native Speicherzugriffe benötigen geeignete Berechtigungen.
- Speichergrenzen und Lebenszeiten müssen eingehalten werden.
- Sichere NovaLang-Referenzen dürfen nicht unkontrolliert in native Zeiger umgewandelt werden.
- Unsafe-Code muss entsprechend isoliert und überprüft werden.

## NovaOS-Integration

Die Runtime verwendet die Speicherverwaltung von NovaOS über definierte Schnittstellen.

- Speicherbudgets werden pro Ausführungskontext durchgesetzt.
- Speicherbereiche unterschiedlicher Schutzdomänen bleiben isoliert.
- Shared Memory benötigt explizite Freigabe und Zugriffsrechte.
- Speicherknappheit muss kontrolliert behandelt werden.
- Logic-Graph-Skripte dürfen keine fremden Speicherbereiche direkt adressieren.

## Normative Anforderungen

1. NovaLang MUSS automatische Speicherverwaltung für verwaltete Objekte unterstützen.
2. Die Runtime MUSS Speichersicherheit im sicheren Sprachmodus gewährleisten.
3. Zirkuläre Referenzen MÜSSEN korrekt bereinigt werden können.
4. Externe Ressourcen MÜSSEN deterministisch freigegeben werden können.
5. Native Speicherzugriffe MÜSSEN auf autorisierte Unsafe-Kontexte beschränkt sein.
6. Speicherlimits und Schutzdomänen MÜSSEN eingehalten werden.
7. AOT, JIT und Interpreter MÜSSEN dieselbe Speichersemantik verwenden.
8. Die Speicherverwaltung DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält eine sichere, automatische und ressourcenschonende Speicherverwaltung mit Garbage Collection, deterministischer Ressourcenfreigabe und kontrolliertem Zugriff auf nativen Speicher.
