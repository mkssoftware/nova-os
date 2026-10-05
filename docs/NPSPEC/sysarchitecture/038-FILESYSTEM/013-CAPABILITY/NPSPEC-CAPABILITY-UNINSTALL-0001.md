# NPSPEC-CAPABILITY-UNINSTALL-0001 – Nova Capability Uninstall

## Status

Angenommen

## Kategorie

Capability / Uninstall

## Zweck

NovaOS definiert die kontrollierte Entfernung installierter Capability-Pakete und ihrer Implementierungen.

Die Deinstallation entfernt installierte Komponenten und zugehörige Registrierungen, ohne Capability-Identitäten, persistente Benutzerentscheidungen oder gemeinsam verwendete Abhängigkeiten unkontrolliert zu beschädigen.

## Grundprinzipien

```text
Uninstall ≠ Deactivation
Uninstall ≠ Revocation
Uninstall ≠ Capability Deletion
Uninstall ≠ Permission Reset
Package Removal ≠ CapabilityID Removal
Dependency Removal ≠ Automatic Cascade
```

## Ziel

Eine Deinstallation bezieht sich auf ein konkretes installierbares Paket:

```text
CapabilityPackage
├── PackageID
├── PackageVersion
├── ProviderID
└── Implementations[]
```

Die globale `CapabilityID` bleibt davon unabhängig.

Andere Provider dürfen dieselbe Capability weiterhin bereitstellen.

## Ablauf

```text
Uninstall Request
      ↓
Authorize
      ↓
Identify Package
      ↓
Dependency Analysis
      ↓
Check Active Executions
      ↓
Deactivate Implementations
      ↓
Remove Registrations
      ↓
Remove Package
      ↓
Cleanup Private Dependencies
      ↓
Verify
```

Die Entfernung muss transaktional erfolgen.

## Laufende Ausführungen

Aktive Implementierungen müssen vor ihrer Entfernung kontrolliert behandelt werden.

Je nach Policy können laufende Ausführungen:

```text
Complete
Cancel
Migrate
Switch Provider
Fail
```

Ein Paket darf nicht entfernt werden, solange seine Ressourcen noch unkontrolliert verwendet werden.

## Provider-Fallback

Existieren weitere kompatible Provider:

```text
CapabilityID
├── Provider A → Uninstall
└── Provider B → Available
```

darf NovaOS zukünftige Aufrufe auf einen anderen geeigneten Provider auflösen.

Der Wechsel muss weiterhin Compatibility, Trust, Policy und Execution Contract erfüllen.

## Abhängigkeiten

Vor der Entfernung muss geprüft werden, ob andere installierte Komponenten vom Paket abhängen.

```text
Package A
   ↓
Package B
```

Wird `Package B` noch benötigt, darf es nicht unkontrolliert entfernt werden.

Private, ausschließlich vom entfernten Paket verwendete Abhängigkeiten dürfen bereinigt werden.

Gemeinsam verwendete Abhängigkeiten bleiben erhalten.

## Registry

Registrierungen der entfernten Implementierungen müssen kontrolliert deaktiviert beziehungsweise entfernt werden.

```text
Implementation
      ↓
Unregister
      ↓
Discovery no longer returns it
```

Die semantische Capability selbst bleibt registrierbar, wenn andere Provider vorhanden sind.

## Permissions

Die Deinstallation erzeugt oder erweitert keine Authority.

Persistente Berechtigungsentscheidungen dürfen gemäß Policy erhalten bleiben, damit eine spätere Neuinstallation derselben validierten Identität nicht zwingend eine vollständige Neuerfassung benötigt.

Sie dürfen jedoch nicht auf eine andere oder manipulierte Identität übertragen werden.

## Daten

Capability-spezifische persistente Nutzerdaten oder Konfigurationen dürfen nicht automatisch gelöscht werden, sofern sie nicht eindeutig als paketinterne Daten klassifiziert sind oder der Benutzer ihre Entfernung ausdrücklich verlangt.

## Rollback

Solange der Uninstall-Commit nicht abgeschlossen ist, muss ein Abbruch auf den vorherigen konsistenten Zustand zurückführen können.

Nach erfolgreichem Commit gilt das Paket als entfernt.

## Normative Anforderungen

1. Capability-Pakete MÜSSEN kontrolliert deinstallierbar sein.
2. Deinstallation MUSS auf `PackageID` und konkrete Installation bezogen sein.
3. CapabilityID und Package-Installation MÜSSEN getrennt bleiben.
4. Aktive Implementierungen MÜSSEN vor Entfernung kontrolliert deaktiviert werden.
5. Laufende Ausführungen MÜSSEN kontrolliert behandelt werden.
6. Abhängigkeiten MÜSSEN vor Entfernung analysiert werden.
7. Gemeinsam verwendete Abhängigkeiten DÜRFEN nicht unkontrolliert entfernt werden.
8. Private unbenutzte Abhängigkeiten DÜRFEN bereinigt werden.
9. Zugehörige Registry-Einträge MÜSSEN konsistent entfernt oder deaktiviert werden.
10. Andere Provider derselben Capability DÜRFEN weiter verwendet werden.
11. Provider-Fallback MUSS weiterhin Trust, Compatibility, Policy und Execution Contract erfüllen.
12. Deinstallation DARF keine Authority erzeugen oder erweitern.
13. Persistente Permissions DÜRFEN nicht auf andere Identitäten übertragen werden.
14. Persistente Nutzerdaten SOLLEN standardmäßig erhalten bleiben.
15. Die Deinstallation MUSS bis zum Commit transaktional abbrechbar sein.
16. Ergebnis, entfernte Komponenten und verbleibende Abhängigkeiten MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-PACKAGE-0001`
- `NPSPEC-CAPABILITY-DEPENDENCY-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-REGISTRATION-0001`
- `NPSPEC-CAPABILITY-DEACTIVATION-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS kann Capability-Pakete und ihre Implementierungen sicher und transaktional entfernen. Andere Provider, gemeinsam verwendete Abhängigkeiten, persistente Daten und bestehende Sicherheitsentscheidungen bleiben kontrolliert erhalten, während entfernte Implementierungen nicht länger für Discovery und neue Ausführungen verfügbar sind.