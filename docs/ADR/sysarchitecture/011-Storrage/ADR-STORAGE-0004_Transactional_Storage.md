# ADR-STORAGE-0004 – Transactional Storage

## Status

Angenommen

## Kategorie

Kernel / Storage / Transactions / Consistency / Recovery

## Kontext

Storage-Operationen bestehen häufig aus mehreren logisch zusammengehörenden Änderungen.

Beispiele sind:

- Erzeugen oder Löschen mehrerer Objekte,
- Aktualisieren von Daten und Metadaten,
- Änderungen von Objektbeziehungen,
- Verschieben oder Umbenennen,
- Aktualisieren von Versionen und Content-Referenzen.

Ein Fehler während einer solchen Operation darf das Storage-System nicht in einem teilweise aktualisierten Zustand hinterlassen.

NovaOS definiert transaktionale Systemoperationen bereits als übergeordnetes Architekturprinzip. Die Storage-Architektur muss dieses Modell nativ unterstützen.

---

## Entscheidung

NovaOS unterstützt **Transactional Storage als native Storage-Eigenschaft**.

Zusammengehörende Storage-Änderungen können innerhalb einer Storage Transaction ausgeführt werden.

```text
Begin
  ↓
Storage Changes
  ↓
Validate
  ↓
Commit
```

Bei einem Fehler vor erfolgreichem Commit werden nicht veröffentlichte Änderungen verworfen beziehungsweise zurückgerollt.

Eine erfolgreich bestätigte Transaktion stellt einen konsistenten neuen Storage-Zustand dar.

---

## Grundprinzip

```text
Publish complete state transitions,
not partial updates.
```

---

## Transaktionsumfang

Eine Storage Transaction kann mehrere Operationen zusammenfassen, beispielsweise:

```text
Create Object
Update Metadata
Change Relations
Update Content Reference
Delete Object
```

Diese Änderungen bilden gemeinsam eine logische Zustandsänderung.

---

## Atomare Sichtbarkeit

Änderungen einer Transaktion sollen für andere Teilnehmer erst mit erfolgreichem Commit als neuer konsistenter Zustand sichtbar werden.

```text
State A
   ↓ Transaction
State B
```

Ein beobachtbarer Zwischenzustand zwischen A und B soll vermieden werden.

---

## Object Storage

Transactional Storage ist direkt mit dem nativen Object-Storage-Modell verbunden.

Transaktionen können insbesondere:

- Object Data,
- Metadata,
- Relations,
- Content References

gemeinsam aktualisieren.

Damit können Objektzustände konsistent verändert werden.

---

## VFS

VFS-Operationen können auf Storage Transactions abgebildet werden.

Beispielsweise können komplexe:

- Move,
- Replace,
- Rename,
- Install,
- Update

Operationen transaktional umgesetzt werden.

Das VFS bleibt dabei Projektionsschicht und definiert nicht selbst die grundlegende Transaktionssemantik.

---

## Content-Addressed Storage

CAS-Inhalte bleiben immutable.

Eine Transaktion verändert daher nicht bestehenden CAS-Inhalt, sondern kann atomar eine Referenz auf einen neuen Content Identifier veröffentlichen.

```text
Object
  ↓
Content A

Transaction

Object
  ↓
Content B
```

Nicht mehr benötigte Inhalte werden unabhängig davon später reclaimt.

---

## Provider

Storage Provider können unterschiedliche native Transaktionsfähigkeiten besitzen.

NovaOS definiert deshalb eine gemeinsame Transaktionssemantik, während die konkrete Umsetzung providerabhängig bleibt.

Provider müssen ihre Garantien explizit deklarieren.

Fehlende Garantien dürfen nicht still als vollständige Transaktionsfähigkeit dargestellt werden.

---

## Crash Consistency

Ein Systemabsturz oder Stromverlust darf keine bereits bestätigte Storage Transaction in einen logisch undefinierten Teilzustand überführen.

Die konkrete Umsetzung kann beispielsweise durch:

- Journaling,
- Copy-on-Write,
- Log-Structured Storage,
- Atomic Metadata Updates

erfolgen.

Die ADR legt keinen einzelnen Mechanismus verbindlich fest.

---

## Verteilte Storage-Transaktionen

Eine lokale Storage Transaction bedeutet nicht automatisch eine globale verteilte Transaktion.

Sind mehrere unabhängige Storage Provider oder entfernte Systeme beteiligt, müssen deren Garantien explizit berücksichtigt werden.

NovaOS darf keine globale Atomarität vortäuschen, wenn die beteiligten Provider diese nicht gewährleisten können.

---

## Security und Data Sovereignty

Eine Transaktion darf ausschließlich Änderungen enthalten, für die die ausführende Domain autorisiert ist.

Security-, Trust- und Data-Sovereignty-Prüfungen bleiben Hard Constraints.

Ein Commit darf keine zuvor verbotene Zustandsänderung legitimieren.

---

## Resource Economy

Transaktionen können temporären Speicher, Logs, Snapshots oder zusätzliche Versionen benötigen.

Diese Ressourcen müssen der verursachenden Resource Domain zugerechnet werden.

Eine Transaktion darf Resource Budgets nicht umgehen.

---

## Recovery

Nach einem ungeplanten Abbruch muss NovaOS eindeutig bestimmen können, ob eine Transaktion:

```text
Committed
```

oder:

```text
Not Committed
```

ist.

Unvollständige interne Transaktionszustände müssen kontrolliert bereinigt werden können.

---

## Architecture Introspection

NovaOS muss Storage Transactions kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- Transaction Identity,
- beteiligte Storage Provider,
- Status,
- betroffene Objekte,
- Commit-Ergebnis,
- verwendete Garantien,
- Recovery-Zustand.

---

## Normative Anforderungen

1. NovaOS MUSS native transaktionale Storage-Operationen unterstützen.
2. Mehrere Storage-Änderungen MÜSSEN zu einer logischen Transaktion zusammenfassbar sein.
3. Erfolgreicher Commit MUSS einen konsistenten neuen Storage-Zustand veröffentlichen.
4. Nicht erfolgreich abgeschlossene Transaktionen DÜRFEN keinen bestätigten Teilzustand hinterlassen.
5. Object Data, Metadata, Relations und Content References MÜSSEN gemeinsam transaktional aktualisierbar sein.
6. VFS-Operationen SOLLEN die native Storage-Transaktionsarchitektur verwenden können.
7. CAS-Inhalte DÜRFEN durch Transaktionen NICHT nachträglich verändert werden.
8. Storage Provider MÜSSEN ihre tatsächlichen Transaktionsgarantien explizit deklarieren.
9. NovaOS DARF keine stärkere Atomarität vortäuschen, als die beteiligten Provider gewährleisten.
10. Bestätigte Transaktionen MÜSSEN definierte Crash-Consistency-Garantien besitzen.
11. Security- und Data-Sovereignty-Constraints MÜSSEN vor Commit erfüllt sein.
12. Transaktionsressourcen MÜSSEN in die Resource Economy integriert sein.
13. Nach Recovery MUSS der Commit-Zustand einer Transaktion eindeutig bestimmbar sein.
14. Transaktionszustände und Garantien MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- konsistente Storage-Zustände,
- robuste Installations- und Update-Operationen,
- sichere Aktualisierung komplexer Storage Objects,
- bessere Crash Recovery,
- direkte Integration in das transaktionale Systemmodell von NovaOS.

### Negative Konsequenzen

- zusätzlicher Storage- und Verwaltungsaufwand,
- Provider besitzen unterschiedliche Transaktionsfähigkeiten,
- große Transaktionen können zusätzliche temporäre Ressourcen benötigen,
- providerübergreifende Transaktionen sind komplexer als lokale Transaktionen.

---

## Verworfene Alternativen

### Nur einzelne atomare Dateioperationen

Verworfen.

NovaOS benötigt Transaktionen über mehrere Objekte, Metadaten und Beziehungen.

### Transaktionen ausschließlich im VFS

Verworfen.

Transactional Storage ist eine Eigenschaft des nativen Storage-Modells und nicht nur der Dateisystemprojektion.

### Globale Atomarität über jeden Storage Provider voraussetzen

Verworfen.

Nicht jeder lokale oder entfernte Provider kann dieselben Garantien bereitstellen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-TRANSACTION-COMMIT-0001`
- `NPSPEC-STORAGE-TRANSACTION-RECOVERY-0001`
- `NPSPEC-STORAGE-TRANSACTION-PROVIDER-0001`
- `NPSPEC-STORAGE-TRANSACTION-TEST-0001`

---

## Ergebnis

NovaOS behandelt persistente Änderungen als kontrollierte Zustandsübergänge:

```text
Consistent State A
       ↓
Storage Transaction
       ↓
Atomic Commit
       ↓
Consistent State B
```

Damit wird Transactional Storage zur Storage-seitigen Umsetzung des systemweiten transaktionalen Architekturmodells.

Die zentrale Architekturregel lautet:

```text
Commit complete state transitions.

Never expose partial storage updates.
```