# NPSPEC-SYSTEM-LAYOUT-0001 – Nova System Layout

## Status

Angenommen

## Kategorie

System / Layout

## Zweck

NovaOS definiert die logische Struktur des systemweiten Bereichs `/System`.

`/System` enthält ausschließlich systemweite Komponenten und Ressourcen. Programmspezifische Abhängigkeiten verbleiben im jeweiligen Program Package und werden bei Bedarf über den privaten `SYS`-Overlay eingebunden.

## Grundprinzipien

```text
/System ≠ Program Storage
/System ≠ User Data
/System ≠ Boot Storage
System Path ≠ Object Identity
System Visibility ≠ Authority
Private SYS ≠ Global /System
```

## Logische Struktur

Die konkrete Struktur darf weiterentwickelt werden, folgt jedoch grundsätzlich funktionalen Systembereichen:

```text
/System/
├── Kernel/
├── Services/
├── Libraries/
├── Runtime/
├── Drivers/
├── Components/
├── Resources/
├── Configuration/
└── Security/
```

Die Verzeichnisstruktur dient der logischen Organisation und bestimmt nicht die Identität einer Komponente.

## Systemkomponenten

`/System` enthält ausschließlich Ressourcen, die systemweit bereitgestellt werden sollen.

Dazu können gehören:

```text
Kernel-nahe Komponenten
Systemdienste
Systembibliotheken
Runtimes
Treiber
Systemkomponenten
Gemeinsame Ressourcen
Systemkonfiguration
Sicherheitskomponenten
```

## Private Program Dependencies

Programmspezifische Abhängigkeiten werden nicht global in `/System` installiert.

```text
/Apps/<Program>/SYS/
        ↓
Program SYS Overlay
        ↓
Effective /System
```

Das Programm sieht dadurch eine effektive Systemsicht:

```text
Effective /System
=
Global /System
+
Program Private SYS
```

Der globale Systembereich bleibt unverändert.

## Objektidentität

Systemkomponenten besitzen stabile Identitäten unabhängig von ihrem Pfad.

```text
Path
  ↓
Namespace Resolution
  ↓
ObjectID
```

Umbenennung, interne Reorganisation oder Projektion darf nicht automatisch eine neue Objektidentität erzeugen.

## Physische Speicherung

Das logische `/System` muss nicht einer einzelnen physischen Partition oder einem einzelnen Volume entsprechen.

```text
Logical /System
      ≠
Physical Storage Layout
```

Storage Location Transparency ermöglicht eine von der logischen Struktur unabhängige physische Ablage.

## Schutz

`/System` ist ein besonders geschützter Namespace-Bereich.

Lesen, Verändern, Ersetzen oder Registrieren systemweiter Komponenten erfordert die jeweils notwendigen Capabilities.

Ein Programm darf durch seine Installation oder seinen privaten `SYS`-Bereich keine Schreibberechtigung auf das globale `/System` erhalten.

## Updates

Änderungen an systemweiten Komponenten sollen über kontrollierte Systemmechanismen erfolgen.

```text
Prepare
  ↓
Validate
  ↓
Commit
  ↓
Verify
```

Unkontrollierte direkte Änderungen durch Programme sind nicht zulässig.

## Normative Anforderungen

1. NovaOS MUSS einen logisch einheitlichen `/System`-Bereich bereitstellen.
2. `/System` MUSS von Benutzer-, Programm- und Bootdaten getrennt sein.
3. Programmspezifische Dependencies DÜRFEN nicht automatisch global installiert werden.
4. Private Dependencies MÜSSEN über den Program-`SYS`-Mechanismus integrierbar sein.
5. Die logische Systemstruktur DARF nicht an eine bestimmte physische Speicherstruktur gebunden sein.
6. Systemkomponenten SOLLEN über stabile Objektidentitäten referenziert werden.
7. Pfade DÜRFEN nicht als dauerhafte Komponentenidentität verwendet werden.
8. Änderungen an `/System` MÜSSEN explizit autorisiert sein.
9. Private `SYS`-Overlays DÜRFEN das globale `/System` nicht verändern.
10. Systemänderungen SOLLEN transaktional und verifizierbar erfolgen.
11. Die Systemstruktur MUSS introspektierbar sein.
12. Die Position innerhalb `/System` DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS besitzt mit `/System` einen klar abgegrenzten und geschützten Bereich für systemweite Komponenten. Programme können eigene Abhängigkeiten über private `SYS`-Overlays verwenden, ohne den globalen Systembestand zu verändern oder die logische Systemstruktur an die physische Speicherung zu koppeln.