# NPSPEC-USERSPACE-TEMP-0001 – Nova Userspace Temporary Data

## Status

Angenommen

## Kategorie

Userspace / Temporary Data

## Zweck

NovaOS stellt kontrollierten temporären Speicher für kurzlebige Daten von Benutzern, Programmen, Solutions und Prozessen bereit.

Temporäre Daten sind nicht für dauerhafte Speicherung vorgesehen und besitzen eine definierte Lebensdauer.

## Grundprinzipien

```text
Temporary ≠ Persistent
Temporary ≠ Cache
Temporary ≠ User Data
Process Temp ≠ Global Temp
```

Temporäre Daten müssen einem eindeutigen Owner und Scope zugeordnet sein.

## Scopes

Mindestens folgende Scopes werden unterstützt:

```text
Process
Program
Solution
User
System
```

Kurzlebige Prozessdaten sollen automatisch mit dem zugehörigen Prozesslebenszyklus verbunden werden können.

## Temp-Modell

```text
TempObject
├── ObjectID
├── OwnerID
├── Scope
├── Lifetime
└── State
```

Optional können Limits, Ablaufzeit und Cleanup-Policy definiert werden.

## Lebensdauer

Temporäre Daten können beispielsweise gelten:

```text
Until Process Exit
Until Session End
Until Reboot
Until Expiration
Explicit Cleanup
```

NovaOS darf abgelaufene temporäre Daten automatisch entfernen.

## Speicherort

Programme dürfen keinen bestimmten physischen Temp-Pfad voraussetzen.

```text
Request Temporary Storage
        ↓
Temp Service
        ↓
Suitable Storage Location
```

NovaOS kann dadurch Speicherort und Medium anhand von Ressourcen-, Performance- und Sicherheitsanforderungen auswählen.

## Isolation

Temporäre Bereiche verschiedener Programme, Solutions oder Benutzer müssen voneinander isolierbar sein.

```text
Program A Temp
≠
Program B Temp
```

Gemeinsamer temporärer Speicher darf nur über explizite Freigabe oder geeignete Capabilities verwendet werden.

## Ressourcensteuerung

Temporäre Daten unterliegen Resource Accounting.

NovaOS darf Limits für:

```text
Capacity
Object Count
Lifetime
```

festlegen und bei Speicherdruck abgelaufene oder freigegebene Temp-Daten bevorzugt zurückfordern.

## Sicherheit

Temp-Zugriffe erfolgen capability-basiert.

Das Erzeugen eines Temp-Objekts gewährt nur die dafür vorgesehene Authority und keinen allgemeinen Zugriff auf andere temporäre Bereiche.

Sensible temporäre Daten müssen beim Cleanup sicher behandelt werden können.

## Normative Anforderungen

1. NovaOS MUSS kontrollierten temporären Speicher bereitstellen.
2. Temp-Daten MÜSSEN einem Owner und Scope zugeordnet sein.
3. Temp-Daten MÜSSEN eine definierbare Lebensdauer besitzen.
4. Programme SOLLEN keine festen physischen Temp-Pfade voraussetzen.
5. Temporäre Bereiche MÜSSEN zwischen Sicherheitskontexten isolierbar sein.
6. Abgelaufene Temp-Daten MÜSSEN automatisch bereinigbar sein.
7. Temp-Speicher MUSS in Resource Accounting integrierbar sein.
8. Temp-Zugriffe MÜSSEN capability-basiert autorisierbar sein.
9. Temp-Daten DÜRFEN NICHT automatisch als persistente Benutzerdaten behandelt werden.
10. Cleanup DARF keine noch gültigen Temp-Objekte außerhalb der geltenden Policy entfernen.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`

## Ergebnis

NovaOS erhält einen isolierten und lebenszyklusgebundenen temporären Speicher. Programme und Solutions können kurzlebige Daten verwenden, ohne feste Temp-Pfade oder physische Speicherorte vorauszusetzen, während NovaOS Cleanup, Sicherheit und Ressourcenverbrauch kontrolliert.