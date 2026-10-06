# NPSPEC-CAPABILITY-UPDATE-0001 – Nova Capability Update

## Status

Angenommen

## Kategorie

Capability / Update

## Zweck

NovaOS definiert die kontrollierte Aktualisierung installierter Capability-Pakete, Provider und Implementierungen.

Updates dürfen Implementierungen ersetzen oder erweitern, ohne die stabile Capability-Identität unnötig zu verändern oder bestehende Sicherheits-, Kompatibilitäts- und Authority-Grenzen zu umgehen.

## Grundprinzipien

```text
Update ≠ New CapabilityID
Update ≠ Automatic Trust
Update ≠ Permission Grant
Update ≠ Authority Expansion
Package Update ≠ Capability Version Change
Installed Update ≠ Activated Update
```

## Update-Modell

Ein Update kann betreffen:

```text
CapabilityPackage
Provider
Implementation
Interface
Dependencies
Resources
Compatibility Metadata
```

Dabei bleiben folgende Versionen getrennt:

```text
CapabilityVersion
InterfaceVersion
PackageVersion
ImplementationVersion
```

## Ablauf

```text
Discover Update
      ↓
Download / Acquire
      ↓
Verify Integrity
      ↓
Verify Trust
      ↓
Validate Manifest
      ↓
Check Dependencies
      ↓
Check Compatibility
      ↓
Evaluate Permissions
      ↓
Stage
      ↓
Commit
      ↓
Register
      ↓
Activate / Live Replace
      ↓
Verify
```

Ein Fehler vor dem Commit darf die bestehende funktionierende Installation nicht beschädigen.

## Identität

Bleibt die semantische Bedeutung einer Capability erhalten, bleibt auch ihre `CapabilityID` erhalten.

```text
CapabilityID
├── Old Implementation
└── New Implementation
```

Eine grundlegend neue semantische Fähigkeit benötigt eine neue `CapabilityID`.

## Transaktionales Update

Updates müssen vorbereitet werden können, bevor die aktive Version ersetzt wird.

```text
Current Version
      +
Staged Update
      ↓
Validation
      ↓
Atomic Switch
      ↓
New Version
```

Die vorherige gültige Version soll bis zur erfolgreichen Verifikation verfügbar bleiben.

## Live Update

Unterstützt die Implementierung Live Evolution, darf sie ohne vollständige Deaktivierung des Capability-Systems ersetzt werden:

```text
Load New
   ↓
Validate
   ↓
Prepare
   ↓
Transfer Compatible State
   ↓
Switch
   ↓
Verify
   ↓
Retire Old
```

Nicht kompatibler Zustand darf nicht ungeprüft übertragen werden.

## Berechtigungen

Ein Update darf bestehende Permissions nicht automatisch erweitern.

Werden neue Operationen, Capability-Anforderungen oder sicherheitsrelevante Funktionen eingeführt, muss eine erneute Policy- und Permission-Bewertung erfolgen.

```text
Old Authority
     ≠
Automatic New Authority
```

## Trust

Jede neue Version muss eigenständig hinsichtlich Integrität, Signatur, Provenance, Trust Chain und Revocation prüfbar sein.

Der Trust-Zustand der vorherigen Version darf nicht automatisch übernommen werden.

## Abhängigkeiten

Geänderte Abhängigkeiten müssen vor Aktivierung aufgelöst und validiert werden.

Ein Update darf globale Systemkomponenten nicht unkontrolliert verändern, nur um private Capability-Abhängigkeiten zu erfüllen.

## Rollback

Schlägt Aktivierung oder Verifikation fehl, muss NovaOS auf die vorherige bekannte gültige Version zurückkehren können, sofern diese weiterhin sicher und kompatibel ist.

```text
Update
  ↓
Failure
  ↓
Deactivate New
  ↓
Restore Previous
  ↓
Verify
```

## Laufende Ausführungen

Bestehende Ausführungen dürfen je nach Update-Strategie:

```text
Complete on Old Version
Migrate
Restart
Cancel
```

Neue Ausführungen können nach erfolgreichem Switch auf die neue Version geleitet werden.

## Normative Anforderungen

1. Capability-Updates MÜSSEN transaktional durchführbar sein.
2. CapabilityID und Versionsinformationen MÜSSEN getrennt bleiben.
3. Updates MÜSSEN vor Aktivierung auf Integrität, Trust, Manifest, Abhängigkeiten und Kompatibilität geprüft werden.
4. Ein Update DARF keine neue Authority automatisch erzeugen.
5. Neue sicherheitsrelevante Anforderungen MÜSSEN eine erneute Permission- und Policy-Bewertung auslösen.
6. Neue Versionen MÜSSEN eigenständig auf Trust geprüft werden.
7. Die bestehende funktionierende Version DARF vor erfolgreichem Commit nicht zerstört werden.
8. Live Replacement SOLL bei geeigneten Implementierungen unterstützt werden.
9. Zustandsübertragung MUSS auf kompatiblen und validierten Zustand begrenzt sein.
10. Laufende Ausführungen MÜSSEN kontrolliert behandelt werden.
11. Fehlgeschlagene Updates MÜSSEN einen kontrollierten Rollback erlauben.
12. Private Abhängigkeiten DÜRFEN globale Systemkomponenten nicht unkontrolliert ersetzen.
13. Registry-Einträge MÜSSEN nach erfolgreichem Update konsistent aktualisiert werden.
14. Update-Version, Zustand, Herkunft und Ergebnis MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-PACKAGE-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-FSCAPABILITY-DEPENDENCY-0001`
- `NPSPEC-FSCAPABILITY-VERSIONING-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-REGISTRATION-0001`
- `NPSPEC-CAPABILITY-ACTIVATION-0001`
- `NPSPEC-CAPABILITY-DEACTIVATION-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`

## Ergebnis

NovaOS kann Capability-Pakete und ihre Implementierungen sicher, transaktional und möglichst ohne Betriebsunterbrechung aktualisieren. Stabile Capability-Identitäten bleiben erhalten, während neue Versionen unabhängig validiert werden und bei Fehlern auf einen bekannten gültigen Zustand zurückgekehrt werden kann.