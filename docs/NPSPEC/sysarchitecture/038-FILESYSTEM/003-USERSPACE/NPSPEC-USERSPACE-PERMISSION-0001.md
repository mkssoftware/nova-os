# NPSPEC-USERSPACE-PERMISSION-0001 – Nova Userspace Permissions

## Status

Angenommen

## Kategorie

Userspace / Security / Permissions

## Zweck

NovaOS definiert die Berechtigungsgrenzen des Userspace für Benutzer, Programme, Solutions und Prozesse.

Userspace-Komponenten erhalten ausschließlich die Authority, die sie für ihre jeweilige Aufgabe benötigen.

## Grundprinzipien

```text
Visibility ≠ Authority
User Context ≠ Unlimited User Access
Application Identity ≠ User Identity
Permission ≠ Ownership
Granted Authority ≠ Permanent Authority
```

Das Filesystem-Permission-Modell bildet die Grundlage; der Userspace definiert darauf die jeweiligen Sicherheitskontexte.

## Sicherheitskontexte

Mindestens folgende Kontexte werden unterschieden:

```text
User
Program
Solution
Process
Workspace
```

Jeder Kontext besitzt seine eigene effektive Authority.

Ein gestartetes Programm übernimmt nicht automatisch sämtliche Berechtigungen des Benutzers.

## Benutzerzugriff

Benutzer dürfen auf ihre autorisierten Ressourcen zugreifen.

```text
User
  ↓
Capability / Policy
  ↓
Authorized Resources
```

Andere Benutzerbereiche bleiben standardmäßig isoliert.

## Programme

Programme erhalten nur explizit gewährte Capabilities.

Beispiele:

```text
Datei lesen
Datei schreiben
Netzwerk verwenden
Gerät verwenden
Benutzerdaten auswählen
```

Private Programmressourcen unter `/Apps/<Program>` erzeugen keine Authority über andere Programme oder `/System`.

## Solutions

Solutions erhalten ihre Authority ausschließlich über die von ihnen verwendeten Capabilities.

Custom-NovaLang-Code darf keine zusätzliche Systemberechtigung selbst erzeugen.

```text
Capability
    ↓
Custom Logic
    ↓
Capability
```

Die Solution-Identität dient zur dauerhaften Zuordnung gewährter Berechtigungen.

## Delegation

Authority darf kontrolliert zwischen Userspace-Komponenten übertragen werden.

```text
Source Authority
      ↓
Attenuation
      ↓
Delegated Authority
```

Delegierte Rechte dürfen die ursprüngliche Authority nicht überschreiten.

## Revocation

Berechtigungen müssen widerrufbar sein.

Ein Widerruf muss bestehende Authority entsprechend der jeweiligen Capability- und Handle-Policy ungültig machen können.

## Systemgrenzen

Userspace-Berechtigungen dürfen keine implizite Authority über kritische Bereiche erzeugen:

```text
/System
/Boot
Kernel
Raw Devices
Security Configuration
```

Solche Zugriffe benötigen explizite privilegierte Capabilities.

## Berechtigungsabfrage

Benutzerinteraktion soll nur erfolgen, wenn eine benötigte Authority noch nicht gültig erteilt wurde.

Bereits gültige Berechtigungen können über die stabile Identität von Programm oder Solution wiederverwendet werden.

Sicherheitsrelevante Änderungen können eine erneute Prüfung erforderlich machen.

## Normative Anforderungen

1. NovaOS MUSS Userspace-Komponenten in getrennten Sicherheitskontexten ausführen können.
2. Programme DÜRFEN NICHT automatisch die vollständige Authority des Benutzers übernehmen.
3. Programme und Solutions MÜSSEN benötigte Systemzugriffe über Capabilities erhalten.
4. Solutions MÜSSEN Berechtigungen an ihre stabile Solution-Identität binden können.
5. Custom Logic DARF keine zusätzliche Authority selbst erzeugen.
6. Delegierte Authority DARF die ursprüngliche Authority nicht überschreiten.
7. Berechtigungen MÜSSEN widerrufbar sein.
8. Userspace-Authority DARF keine implizite Kernel-, Boot-, System- oder Raw-Device-Authority erzeugen.
9. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Berechtigungsprüfung auslösen können.
10. Sichtbarkeit einer Userspace-Ressource DARF keine Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-CAPABILITY-APPLICATION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`

## Ergebnis

NovaOS erhält ein konsequent capability-basiertes Userspace-Berechtigungsmodell. Benutzer, Programme, Solutions und Prozesse arbeiten mit klar begrenzter Authority, ohne dass Benutzerkontext, Dateisichtbarkeit oder Programmausführung automatisch umfassende Systemrechte erzeugen.