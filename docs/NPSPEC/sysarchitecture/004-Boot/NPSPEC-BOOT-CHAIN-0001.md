# NPSPEC-BOOT-CHAIN-0001 – Nova Boot Chain

## Status

Angenommen

## Kategorie

Boot / Boot Chain / Integrity / Kernel Handoff

## Zweck

NovaOS definiert eine kontrollierte Boot-Kette vom Firmware-Einstieg bis zur Übernahme durch den Kernel.

```text
Firmware
   ↓
Boot Entry
   ↓
Nova Bootloader
   ↓
Kernel
   ↓
NovaOS
```

Jede Stufe muss ihren eigenen Zustand validieren und die nächste Stufe eindeutig identifizieren.

## Grundprinzip

Die Boot Chain besteht aus klar getrennten Übergängen.

```text
Stage N
   ↓
Validate
   ↓
Load Stage N+1
   ↓
Prepare Handoff
   ↓
Transfer Control
```

Eine Stufe darf die Kontrolle nur an eine ausreichend validierte nächste Stufe übergeben.

## Boot-Pfade

Unterschiedliche Firmwarepfade werden auf dieselbe logische Boot-Kette normalisiert.

```text
BIOS ──→ BIOS Loader ──┐
                       ├──→ Nova Boot Model → Kernel
UEFI ──→ EFI Loader ───┘
```

Der Kernel soll nicht wissen müssen, über welchen Firmwarepfad er gestartet wurde, außer wenn diese Information ausdrücklich benötigt wird.

## Stufen

Eine Boot-Kette kann beispielsweise bestehen aus:

```text
Firmware
   ↓
Boot Entry
   ↓
Primary Loader
   ↓
Extended Loader
   ↓
Kernel Loader
   ↓
Kernel
```

Nicht jede Plattform benötigt dieselbe Anzahl physischer Stufen.

Entscheidend ist das gemeinsame logische Handoff-Modell.

## Übergabevertrag

Jeder Übergang muss definieren:

```text
Entry Point
Execution State
Memory State
Input Data
Boot Information
Ownership
Failure Behaviour
```

Nicht dokumentierte Annahmen zwischen Boot-Stufen sind zu vermeiden.

## Integrität

Boot-Komponenten können vor ihrer Ausführung überprüft werden.

```text
Component
   ↓
Integrity / Signature Check
   ↓
Load
   ↓
Execute
```

Bei aktivierter vertrauenswürdiger Boot-Kette darf eine fehlgeschlagene kritische Prüfung nicht stillschweigend ignoriert werden.

## Boot Information

Informationen werden entlang der Boot-Kette gesammelt, validiert und normalisiert.

```text
Firmware Information
Memory Map
Framebuffer
Platform Information
Boot Mode
Kernel Information
        ↓
Nova Boot Information
```

Die endgültige Struktur wird dem Kernel übergeben.

## Ownership

Nach erfolgreichem Übergang muss klar sein, welche Komponente für Ressourcen verantwortlich ist.

Beispiel:

```text
Bootloader owns resource
        ↓
Kernel Handoff
        ↓
Kernel owns resource
```

Nicht mehr benötigte Bootressourcen sollen freigegeben oder als wiederverwendbar markiert werden.

## Recovery

Ein Fehler innerhalb der Boot Chain kann einen definierten Recovery-Pfad auslösen.

```text
Boot Failure
     ↓
Recovery Decision
     ↓
Retry / Alternative Kernel / Recovery Environment
```

Fehlerhafte Boot-Schleifen sollen vermieden werden.

## Normative Anforderungen

1. NovaOS MUSS eine klar definierte Boot Chain besitzen.
2. Jeder Boot-Übergang MUSS einen definierten Handoff-Vertrag besitzen.
3. Eine Boot-Stufe MUSS die nächste Stufe vor der Übergabe ausreichend validieren.
4. BIOS und UEFI SOLLEN auf dieselbe logische Boot Chain normalisiert werden.
5. Bootinformationen MÜSSEN entlang der Kette kontrolliert aufgebaut werden.
6. Speicher- und Ressourcenownership MUSS bei Übergängen eindeutig sein.
7. Kritische Integritätsfehler DÜRFEN NICHT stillschweigend ignoriert werden.
8. Fehlgeschlagene Boot-Stufen MÜSSEN einen definierten Fehlerpfad besitzen.
9. Recovery-Pfade SOLLEN alternative Bootziele unterstützen können.
10. Der Kernel DARF keine undokumentierten Zustände früherer Boot-Stufen voraussetzen.
11. Boot-Stufen SOLLEN möglichst klein und klar abgegrenzt bleiben.
12. Die Boot Chain MUSS zukünftige Plattformen und Bootmechanismen integrieren können.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-BIOS-0001`
- `NPSPEC-BOOT-UEFI-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `ADR-BOOT-0001`
- `ADR-BOOT-0002`
- `ADR-BOOT-0003`
- `ADR-BOOT-0004`

## Ergebnis

```text
Firmware
   ↓
Validated Boot Stages
   ↓
Normalized Boot State
   ↓
Defined Kernel Handoff
   ↓
Nova Kernel
```

NovaOS erhält damit eine nachvollziehbare und erweiterbare Boot-Kette mit klaren Zuständigkeiten, Übergängen und Fehlerpfaden.