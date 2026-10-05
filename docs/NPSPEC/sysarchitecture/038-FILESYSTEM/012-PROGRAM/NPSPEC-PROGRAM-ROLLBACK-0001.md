# NPSPEC-PROGRAM-ROLLBACK-0001 – Nova Program Rollback

## Status

Angenommen

## Kategorie

Program / Update / Rollback

## Zweck

NovaOS definiert die kontrollierte Rückkehr von einer aktualisierten Programmversion zu einer zuvor gültigen Version derselben Programminstallation.

Rollback dient insbesondere der Wiederherstellung nach fehlerhaften oder inkompatiblen Updates.

## Grundprinzipien

```text
Rollback ≠ Neuinstallation
Rollback ≠ New Program Identity
Rollback ≠ User Data Rollback
Rollback ≠ Permission Reset
Previous Version ≠ Automatically Trusted Version
```

Die `ProgramID` und der Install Scope bleiben erhalten.

## Rollback-Modell

NovaOS kann vorherige validierte Paketversionen als Rollback-Kandidaten erhalten:

```text
ProgramID
├── Version 1.0
├── Version 1.1
└── Version 1.2 ← Current
```

Ein Rollback kann beispielsweise:

```text
1.2 → 1.1
```

aktivieren.

## Ablauf

```text
Rollback Request
      ↓
Select Previous Version
      ↓
Validate Package
      ↓
Check Compatibility
      ↓
Check Permissions
      ↓
Prepare
      ↓
Switch Version
      ↓
Verify
```

Die Umschaltung soll transaktional erfolgen.

## Paket und Dependencies

Rollback betrifft das vollständige Program Package:

```text
App/
Resources/
SYS/
Manifest
```

Dadurch werden auch die zu dieser Version gehörenden privaten Dependencies wiederhergestellt.

Andere Programme und das globale `/System` bleiben unberührt.

## Benutzerdaten

Benutzerdaten werden durch einen Program Rollback grundsätzlich nicht zurückgesetzt.

```text
Program Version → Rollback
User Data       → Unchanged
```

Falls eine ältere Programmversion mit neueren Datenformaten nicht kompatibel ist, muss dieser Zustand erkannt und kontrolliert behandelt werden.

## Berechtigungen

Rollback stellt keine frühere Authority automatisch wieder her.

Die Capability-Anforderungen der Zielversion müssen mit den aktuell gültigen Berechtigungen abgeglichen werden.

Widerrufene Berechtigungen bleiben widerrufen.

## Laufende Instanzen

Laufende Instanzen der aktuellen Version müssen vor der Umschaltung kontrolliert behandelt werden.

NovaOS darf den Rollback bis zum nächsten Programmstart verzögern, wenn ein sicherer Live-Wechsel nicht möglich ist.

## Fehlerbehandlung

Schlägt die Aktivierung der Zielversion fehl, darf NovaOS nicht in einem undefinierten Mischzustand verbleiben.

Die zuletzt gültige Programminstallation soll weiterhin verfügbar bleiben.

## Normative Anforderungen

1. Rollback MUSS auf dieselbe `ProgramID` und denselben Install Scope begrenzt sein.
2. Nur vorhandene und validierbare Programmversionen DÜRFEN als Rollback-Ziel verwendet werden.
3. Das vollständige versionsgebundene Program Package MUSS berücksichtigt werden.
4. Private Dependencies MÜSSEN passend zur Zielversion wiederherstellbar sein.
5. Rollback DARF andere Programme oder das globale `/System` nicht verändern.
6. Benutzerdaten DÜRFEN standardmäßig nicht zurückgesetzt werden.
7. Datenformat-Inkompatibilitäten MÜSSEN erkannt werden können.
8. Frühere Berechtigungen DÜRFEN nicht automatisch wiederhergestellt werden.
9. Die Versionsumschaltung SOLL transaktional erfolgen.
10. Ein fehlgeschlagener Rollback DARF keinen undefinierten Mischzustand hinterlassen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-DEPENDENCY-0002`
- `NPSPEC-PROGRAM-INSTALLSCOPE-0001`
- `NPSPEC-PROGRAM-UPDATE-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`

## Ergebnis

NovaOS kann nach problematischen Programmupdates kontrolliert auf eine zuvor gültige Version zurückkehren. Programmidentität und Install Scope bleiben erhalten, private Dependencies werden versionskonsistent zurückgesetzt und Benutzerdaten sowie aktuelle Sicherheitsentscheidungen bleiben geschützt.