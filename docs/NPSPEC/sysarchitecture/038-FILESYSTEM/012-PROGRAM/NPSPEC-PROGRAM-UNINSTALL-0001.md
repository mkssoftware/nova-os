# NPSPEC-PROGRAM-UNINSTALL-0001 – Nova Program Uninstallation

## Status

Angenommen

## Kategorie

Program / Installation

## Zweck

NovaOS definiert die sichere und transaktionale Entfernung klassischer Program Packages.

Eine Deinstallation entfernt das Programm und seine ausschließlich privaten Komponenten, ohne Benutzerdaten, gemeinsam genutzte Systemressourcen oder andere Installationen unbeabsichtigt zu verändern.

## Grundprinzipien

```text
Uninstall ≠ Delete User Data
Uninstall ≠ Revoke User Ownership
Private Dependency ≠ Shared Dependency
Remove Package ≠ Remove User Settings
Install Scope A ≠ Install Scope B
```

## Deinstallationsablauf

```text
Uninstall Request
      ↓
Resolve Installation
      ↓
Check Authority
      ↓
Analyze Dependencies
      ↓
Prepare Removal
      ↓
Commit
      ↓
Unregister
      ↓
Verify
```

## Installationsinstanz

Die zu entfernende Installation wird über:

```text
ProgramID
+
Install Scope
```

eindeutig bestimmt.

Eine User-Installation und eine System-Installation desselben Programms müssen unabhängig voneinander entfernt werden können.

## Program Package

Bei der Deinstallation dürfen die zur Installation gehörenden Paketbestandteile entfernt werden:

```text
App/
Resources/
SYS/
Manifest
```

Private Dependencies innerhalb von `SYS` werden zusammen mit dem Program Package entfernt, sofern sie ausschließlich zu dieser Installation gehören.

## Laufende Prozesse

Ist das Programm noch aktiv, muss NovaOS dessen laufende Instanzen erkennen.

Abhängig von Policy und Situation kann die Deinstallation:

```text
warten
Beenden anfordern
abgebrochen werden
für später vorgemerkt werden
```

Aktiv verwendete Komponenten dürfen nicht unkontrolliert entfernt werden.

## Benutzerdaten

Benutzerdaten bleiben standardmäßig erhalten.

Dazu gehören insbesondere Dokumente und andere vom Benutzer erzeugte Inhalte.

Programmspezifische Einstellungen oder Cache-Daten dürfen separat zur Entfernung angeboten werden.

```text
Program Package → entfernen
User Data       → erhalten
```

## Berechtigungen

Die Deinstallation benötigt Authority für den jeweiligen Install Scope.

Mit dem Programm verbundene Capability-Zuweisungen dürfen anschließend entfernt oder ungültig gemacht werden, sofern sie ausschließlich dieser Installation zugeordnet sind.

## Transaktion

Die Deinstallation soll transaktional erfolgen.

```text
Begin
  ↓
Prepare
  ↓
Remove Registration
  ↓
Remove Package
  ↓
Commit
  ↓
Verify
```

Ein Fehler darf keinen undefinierten teilweise deinstallierten Zustand hinterlassen.

## Normative Anforderungen

1. NovaOS MUSS Programminstallationen gezielt deinstallieren können.
2. Die Deinstallation MUSS `ProgramID` und Install Scope berücksichtigen.
3. Eine Installation DARF andere Install Scopes nicht unbeabsichtigt entfernen.
4. Private Dependencies SOLLEN mit ihrer Programminstallation entfernt werden.
5. Globale oder gemeinsam genutzte Komponenten DÜRFEN nicht unbeabsichtigt entfernt werden.
6. Benutzerdaten MÜSSEN standardmäßig erhalten bleiben.
7. Laufende Programminstanzen MÜSSEN vor der Entfernung berücksichtigt werden.
8. Die Deinstallation SOLL transaktional erfolgen.
9. Fehlgeschlagene Deinstallationen DÜRFEN keinen inkonsistenten Systemzustand hinterlassen.
10. Nach erfolgreicher Entfernung MUSS die Programminstallation aus der Registrierung entfernt sein.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-DEPENDENCY-0002`
- `NPSPEC-PROGRAM-INSTALLSCOPE-0001`
- `NPSPEC-PROGRAM-INSTALL-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS kann klassische Programme vollständig und kontrolliert entfernen. Program Package, private Abhängigkeiten und Registrierung werden bereinigt, während Benutzerdaten, andere Install Scopes und gemeinsam genutzte Systemressourcen geschützt bleiben.