# NPSPEC-PROGRAM-PERMISSION-0001 – Nova Program Permissions

## Status

Angenommen

## Kategorie

Program / Security / Permissions

## Zweck

NovaOS definiert das Berechtigungsmodell klassischer Programme.

Programme erhalten ausschließlich explizit autorisierte Capabilities und übernehmen nicht automatisch die vollständige Authority des Benutzers.

## Grundprinzipien

```text
Program Identity ≠ User Identity
Installation ≠ Runtime Authority
Visibility ≠ Authority
Permission Requirement ≠ Granted Permission
Program Access ⊆ Granted Authority
```

## Programmkontext

Jede Programminstanz läuft in einem eigenen Sicherheitskontext:

```text
User
  ↓
Permission Policy
  ↓
Program Capability Context
  ↓
Process
```

Mehrere Programme desselben Benutzers besitzen dadurch nicht automatisch dieselben Berechtigungen.

## Capability-Anforderungen

Benötigte Systemzugriffe werden im Program Manifest deklariert.

Beispiele:

```text
Filesystem Read
Filesystem Write
Network Access
Device Access
Camera
Microphone
Location
```

Eine Deklaration beschreibt lediglich einen Bedarf und erzeugt keine Authority.

## Berechtigungsvergabe

```text
Capability Requirement
        ↓
Policy Evaluation
        ↓
User / System Decision
        ↓
Granted Capability
        ↓
Authorized Handle
```

NovaOS soll Berechtigungen möglichst granular und nach dem Least-Privilege-Prinzip vergeben.

## Persistente Berechtigungen

Erteilte Berechtigungen dürfen an die stabile `ProgramID` und die konkrete Programminstallation gebunden werden.

Dadurch muss eine bereits gültig erteilte Berechtigung nicht bei jedem Programmstart erneut abgefragt werden.

Sicherheitsrelevante Änderungen können eine erneute Prüfung erforderlich machen.

## Install Scope

```text
Install Scope ≠ Permission Scope
```

Eine System-Installation besitzt nicht automatisch mehr Laufzeitrechte als eine User-Installation.

## Private SYS-Abhängigkeiten

Code aus dem privaten `SYS` arbeitet innerhalb des Sicherheitskontexts des Programms.

```text
Program
├── App
└── SYS
      ↓
Same Program Authority
```

Eine private Dependency erhält keine eigenständige zusätzliche Authority.

## Updates

Bei einem Update werden alte und neue Capability-Anforderungen verglichen.

Neue oder erweiterte Anforderungen müssen erneut autorisiert werden.

Entfallene Anforderungen sollen aus dem effektiven Capability-Kontext entfernt werden können.

## Widerruf

Berechtigungen müssen jederzeit widerrufbar sein.

Der Widerruf muss auf bestehende Capability Handles entsprechend der jeweiligen Revocation-Policy wirken können.

## Normative Anforderungen

1. Jedes Programm MUSS in einem begrenzten Sicherheitskontext ausführbar sein.
2. Programme DÜRFEN NICHT automatisch die vollständige Benutzer-Authority übernehmen.
3. Systemzugriffe MÜSSEN über Capabilities autorisiert werden.
4. Manifest-Deklarationen DÜRFEN keine Authority erzeugen.
5. Berechtigungen DÜRFEN an die stabile `ProgramID` und Installation gebunden werden.
6. Install Scope DARF keine zusätzliche Runtime-Authority erzeugen.
7. Private `SYS`-Komponenten DÜRFEN keine zusätzliche Authority erhalten.
8. Neue oder erweiterte Capability-Anforderungen nach Updates MÜSSEN erneut geprüft werden.
9. Berechtigungen MÜSSEN widerrufbar sein.
10. Programme MÜSSEN nach dem Least-Privilege-Prinzip ausgeführt werden können.

## Abhängigkeiten

- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-INSTALLSCOPE-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-UPDATE-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`

## Ergebnis

NovaOS führt klassische Programme mit klar begrenzter, capability-basierter Authority aus. Installation, Benutzerkontext und private Abhängigkeiten erzeugen keine impliziten Rechte; benötigte Systemzugriffe werden explizit deklariert, autorisiert und widerrufbar verwaltet.