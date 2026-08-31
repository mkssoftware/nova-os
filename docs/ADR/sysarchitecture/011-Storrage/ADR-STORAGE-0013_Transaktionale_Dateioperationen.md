# ADR-STORAGE-0013 – Transaktionale Dateioperationen

## Status

Angenommen

## Kategorie

Storage / VFS / Transactions / File Operations / Consistency

## Kontext

NovaOS definiert mit `ADR-STORAGE-0004` Transactional Storage als native Grundlage für konsistente persistente Zustandsänderungen.

Auch klassische Dateioperationen können aus mehreren internen Änderungen bestehen. Ein Verschieben kann beispielsweise Namespace, Metadaten, Objektbeziehungen und Storage-Zustände betreffen.

Operationen wie:

- Create,
- Copy,
- Move,
- Rename,
- Replace,
- Delete

dürfen deshalb nicht unnötig als Folge voneinander unabhängiger Teilschritte behandelt werden.

Insbesondere bei Systemupdates, Installationen, Benutzeraktionen und Recovery muss verhindert werden, dass Fehler oder Abstürze inkonsistente Zwischenzustände hinterlassen.

---

## Entscheidung

NovaOS bildet **logisch zusammengehörende Dateioperationen auf native Storage Transactions ab**.

```text
File Operation
      ↓
VFS
      ↓
Storage Transaction
      ↓
Atomic State Transition
```

Die Dateisemantik bleibt dabei eine VFS-Projektion. Die eigentliche Konsistenz wird durch Transactional Storage gewährleistet.

---

## Grundprinzip

```text
File operations are state transitions,
not sequences of unrelated mutations.
```

---

## Transaktionale Operationen

Operationen, die aus mehreren zusammengehörenden Storage-Änderungen bestehen, sollen als eine Transaktion behandelt werden.

Beispiel:

```text
Rename

Old Namespace Entry
        ↓
Transaction
        ↓
New Namespace Entry
```

Andere Teilnehmer sollen grundsätzlich entweder den Zustand vor oder nach der Operation beobachten.

---

## Create und Replace

Das Erzeugen einer Datei soll erst sichtbar werden, wenn ihr erforderlicher Objektzustand konsistent bereitsteht.

Replace-Operationen sollen einen bestehenden Zustand atomar durch einen neuen ersetzen können.

```text
Old File
   ↓
Replace Transaction
   ↓
New File
```

Ein Fehler darf nicht dazu führen, dass weder der alte noch ein gültiger neuer Zustand verfügbar ist, sofern der Provider die erforderliche Garantie unterstützt.

---

## Move und Rename

Move und Rename sind logische Namespace-Operationen.

Wenn Quell- und Zielzustand innerhalb derselben Transaktionsdomäne liegen, sollen sie atomar ausgeführt werden.

Ein Rename verändert dabei grundsätzlich nicht:

- Object Identity,
- Content Identity,
- Provenance-Historie.

Bei providerübergreifenden Moves kann dagegen eine komplexere Operation erforderlich sein.

---

## Copy

Copy erzeugt ein neues logisches Objekt beziehungsweise eine neue zulässige Referenzstruktur.

Die Implementierung darf vorhandene Storage-Mechanismen wie:

- CAS,
- Copy-on-Write,
- Shared Backing

verwenden, um physische Datenkopien zu vermeiden.

Die logische Copy-Semantik darf dadurch nicht verändert werden.

---

## Delete

Delete entfernt die entsprechende logische Sicht beziehungsweise Referenz innerhalb einer Transaktion.

Dies bedeutet nicht zwangsläufig, dass physischer Content unmittelbar gelöscht wird.

```text
Delete Object Reference
        ↓
Commit
        ↓
Retention / GC
        ↓
Physical Reclamation
```

Versionierung, Snapshots, CAS-Referenzen oder Retention Policies können die physische Lebensdauer verlängern.

---

## Providergrenzen

Nicht jede Dateioperation kann über mehrere unabhängige Storage Provider vollständig atomar ausgeführt werden.

Beispielsweise kann ein Move zwischen zwei Providern intern zu:

```text
Copy
  ↓
Verify
  ↓
Publish Destination
  ↓
Remove Source
```

werden.

NovaOS darf in solchen Fällen keine stärkere Atomarität vortäuschen, als tatsächlich gewährleistet werden kann.

Die verfügbaren Garantien müssen über die Provider Contracts erkennbar sein.

---

## Versioned Storage

Transaktionale Dateioperationen integrieren sich in Versioned Storage.

Eine Änderung kann dadurch einen neuen Objektzustand erzeugen, ohne vorherige gültige Zustände sofort zu vernichten.

Dies ermöglicht insbesondere:

- Undo,
- Rollback,
- Recovery,
- nachvollziehbare Änderungen.

Ob und wie lange frühere Versionen erhalten bleiben, bestimmt die jeweilige Retention Policy.

---

## Fehler und Recovery

Nach einem Fehler oder Systemabsturz muss für eine transaktionale Dateioperation ein definierter Zustand wiederherstellbar sein.

Grundsätzlich gilt:

```text
Committed
```

oder:

```text
Not Committed
```

Unvollständige interne Zwischenzustände dürfen nicht als erfolgreich abgeschlossene Benutzeroperation erscheinen.

---

## Security und Data Sovereignty

Eine Transaktion darf keine Zugriffsrechte erweitern.

Bei Dateioperationen müssen alle betroffenen:

- Objekte,
- Namespaces,
- Volumes,
- Storage Provider

die erforderlichen Security- und Data-Sovereignty-Constraints erfüllen.

Insbesondere provider- oder volumeübergreifende Moves dürfen keine Daten in unzulässige Storage Domains übertragen.

---

## Normative Anforderungen

1. NovaOS MUSS transaktionale Dateioperationen über das native Transactional-Storage-Modell unterstützen.
2. Logisch zusammengehörende Dateiänderungen SOLLEN als eine Storage Transaction ausgeführt werden.
3. Erfolgreiche Operationen MÜSSEN einen konsistenten neuen Zustand veröffentlichen.
4. Unvollständige Operationen DÜRFEN NICHT als erfolgreich abgeschlossen sichtbar werden.
5. Rename innerhalb einer geeigneten Transaktionsdomäne SOLL atomar sein.
6. Replace SOLL den alten Zustand atomar durch einen neuen Zustand ersetzen können.
7. Rename und Move DÜRFEN die stabile Object Identity nicht allein aufgrund einer Namespace-Änderung verändern.
8. Copy DARF CAS, Copy-on-Write oder Shared Backing zur Vermeidung unnötiger physischer Kopien verwenden.
9. Delete MUSS von physischer Content-Reclamation getrennt behandelt werden können.
10. Providerübergreifende Operationen DÜRFEN keine nicht vorhandene Atomarität vortäuschen.
11. Provider MÜSSEN ihre relevanten Transaktionsgarantien deklarieren.
12. Dateioperationen SOLLEN mit Versioned Storage und Recovery kombinierbar sein.
13. Security- und Data-Sovereignty-Constraints MÜSSEN während der gesamten Operation erhalten bleiben.
14. Nach Recovery MUSS der Commit-Zustand einer transaktionalen Dateioperation eindeutig bestimmbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger inkonsistente Dateizustände,
- robuste Move-, Rename- und Replace-Operationen,
- bessere Crash Recovery,
- Grundlage für Undo und Rollback,
- direkte Integration in das native Storage-Modell.

### Negative Konsequenzen

- komplexere VFS- und Provider-Implementierung,
- providerübergreifende Operationen besitzen teilweise schwächere Garantien,
- Transaktionen benötigen zusätzliche temporäre Ressourcen.

---

## Verworfene Alternativen

### Dateioperationen ausschließlich als einzelne primitive Schritte

Verworfen.

Fehler zwischen diesen Schritten könnten inkonsistente Zustände erzeugen.

### Eigene Transaktionslogik ausschließlich im VFS

Verworfen.

Das VFS soll die gemeinsame Transactional-Storage-Infrastruktur verwenden.

### Providerübergreifende Atomarität grundsätzlich voraussetzen

Verworfen.

Unterschiedliche Provider können unterschiedliche Garantien besitzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0011_Legacy_Dateiprojektion_ohne_Duplikation`
- `ADR-STORAGE-0012_Named_Volumes_statt_Laufwerksbuchstaben`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-FILE-TRANSACTION-0001`
- `NPSPEC-STORAGE-FILE-CREATE-0001`
- `NPSPEC-STORAGE-FILE-MOVE-0001`
- `NPSPEC-STORAGE-FILE-COPY-0001`
- `NPSPEC-STORAGE-FILE-REPLACE-0001`
- `NPSPEC-STORAGE-FILE-DELETE-0001`
- `NPSPEC-STORAGE-FILE-TRANSACTION-TEST-0001`

---

## Ergebnis

Klassische Dateioperationen werden in NovaOS auf konsistente Änderungen des nativen Storage-Modells abgebildet:

```text
User File Operation
        ↓
       VFS
        ↓
Storage Transaction
        ↓
Consistent Object + Namespace State
```

Die zentrale Architekturregel lautet:

```text
A file operation either commits
as a consistent state change
or it does not commit.
```