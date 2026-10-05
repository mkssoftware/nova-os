# NPSPEC-PROGRAM-UPDATE-0001 – Nova Program Update

## Status

Angenommen

## Kategorie

Program / Update

## Zweck

NovaOS definiert die sichere und transaktionale Aktualisierung klassischer Program Packages.

Ein Update ersetzt eine bestehende Programmversion durch eine neue Version derselben `ProgramID`, ohne Install Scope, Benutzerdaten oder andere Programme unbeabsichtigt zu verändern.

## Grundprinzipien

```text
Update ≠ Neuinstallation
Update ≠ New Program Identity
Update ≠ Global System Update
Update ≠ Permission Grant
Prepared ≠ Active
```

## Identität

Ein reguläres Update behält die bestehende `ProgramID`.

```text
ProgramID
├── Version 1.0
├── Version 1.1
└── Version 2.0
```

Ändert sich die Programmidentität, handelt es sich nicht um ein normales Update.

## Update-Ablauf

```text
Update Package
      ↓
Validate Manifest
      ↓
Verify Integrity / Trust
      ↓
Check ProgramID
      ↓
Resolve Dependencies
      ↓
Check Compatibility
      ↓
Prepare
      ↓
Commit
      ↓
Verify
```

Die bisherige Installation bleibt bis zum erfolgreichen Commit gültig.

## Install Scope

Das Update gilt ausschließlich für die ausgewählte Installation:

```text
ProgramID
+
Install Scope
```

Eine User-Installation darf nicht automatisch eine System-Installation aktualisieren und umgekehrt.

## Private Dependencies

Neue private Abhängigkeiten werden gemeinsam mit der neuen Programmversion bereitgestellt.

```text
New Package
├── App/
├── Resources/
├── SYS/
└── Manifest
```

Änderungen innerhalb des privaten `SYS` dürfen andere Programme oder das globale `/System` nicht beeinflussen.

## Berechtigungen

Bestehende Programmberechtigungen dürfen weiterverwendet werden, sofern Identität und sicherheitsrelevante Anforderungen kompatibel bleiben.

Neue oder erweiterte Capability-Anforderungen müssen erneut bewertet werden.

```text
Old Requirements
      ↓
Compare
      ↓
New Requirements
      ↓
Permission Evaluation
```

Ein Update darf sich keine zusätzliche Authority allein durch die Aktualisierung verschaffen.

## Transaktion und Rollback

Updates sollen atomar sichtbar werden.

```text
Current Version
      ↓
Stage New Version
      ↓
Validate
      ↓
Switch
      ↓
Verify
```

Schlägt das Update vor dem erfolgreichen Abschluss fehl, soll die vorherige gültige Version weiter verwendbar bleiben.

Ein Rollback darf nur auf eine validierte vorherige Version erfolgen.

## Laufende Instanzen

Laufende Programminstanzen müssen kontrolliert behandelt werden.

Je nach Programm und Update-Art kann NovaOS:

```text
Update verzögern
Neustart des Programms anfordern
Update beim nächsten Start aktivieren
```

Aktiv verwendete Komponenten dürfen nicht unkontrolliert ersetzt werden.

## Normative Anforderungen

1. Ein reguläres Update MUSS dieselbe `ProgramID` beibehalten.
2. Update Packages MÜSSEN vor Aktivierung validiert werden.
3. Integrität und Vertrauensstatus MÜSSEN prüfbar sein.
4. Der bestehende Install Scope MUSS erhalten bleiben.
5. Updates SOLLEN transaktional aktiviert werden.
6. Fehlgeschlagene Updates DÜRFEN die letzte gültige Installation nicht unbrauchbar machen.
7. Private Dependencies DÜRFEN andere Programme oder `/System` nicht beeinflussen.
8. Neue Capability-Anforderungen MÜSSEN erneut bewertet werden.
9. Ein Update DARF keine zusätzliche Authority implizieren.
10. Laufende Programminstanzen MÜSSEN kontrolliert berücksichtigt werden.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-DEPENDENCY-0002`
- `NPSPEC-PROGRAM-INSTALLSCOPE-0001`
- `NPSPEC-PROGRAM-INSTALL-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS kann klassische Programme sicher und transaktional aktualisieren. Programmidentität und Install Scope bleiben erhalten, private Abhängigkeiten bleiben isoliert und neue Berechtigungsanforderungen werden kontrolliert geprüft, während bei einem fehlgeschlagenen Update die letzte gültige Version erhalten bleiben kann.