# NPSPEC-SYSTEM-MODULES-0001 – Nova System Modules

## Status

Angenommen

## Kategorie

System / Modules

## Zweck

NovaOS definiert System Modules als austauschbare, versionierte Komponenten zur Erweiterung systemweiter Funktionen.

Module besitzen klar definierte Schnittstellen und Lebenszyklen und dürfen nicht von undokumentierten internen Zuständen anderer Systemkomponenten abhängig sein.

## Grundprinzipien

```text
Module ≠ Program
Module ≠ Solution
Module ≠ Driver
Module ≠ Kernel
Module Presence ≠ Authority
Module Interface ≠ Implementation
```

## Modulmodell

Ein System Module besitzt mindestens:

```text
SystemModule
├── ModuleID
├── Version
├── Interface
├── Dependencies
├── State
└── Compatibility
```

Optional:

```text
Capabilities
Provider Interfaces
Runtime Requirements
Trust Information
Resource Requirements
```

Die `ModuleID` bleibt unabhängig von Dateiname, Pfad oder Ladeadresse.

## Architektur

```text
System Framework
      ↓
Module Interface
      ↓
System Module
      ↓
Foundation / Runtime / Services
```

Andere Komponenten verwenden definierte Schnittstellen statt direkter Zugriffe auf die interne Implementierung.

## Modultypen

System Modules können beispielsweise Funktionen bereitstellen für:

```text
Framework Provider
Protocol Support
Data Format Support
System Integration
Runtime Extension
Security Function
Media Component
System Capability
```

Gerätespezifische Hardwareanbindung verbleibt im Driver Framework.

## Abhängigkeiten

Module deklarieren ihre benötigten Abhängigkeiten und Versionen.

```text
Module
  ↓
Dependency Resolution
  ↓
Compatibility Validation
  ↓
Load
```

Zirkuläre oder nicht erfüllbare Abhängigkeiten müssen erkannt werden.

## Lifecycle

```text
Discover
  ↓
Validate
  ↓
Resolve Dependencies
  ↓
Load
  ↓
Initialize
  ↓
Active
  ↓
Suspend / Replace
  ↓
Unload
```

Module sollen kontrolliert geladen und entfernt werden können.

## Live Replacement

Wenn Modul und abhängige Komponenten dies unterstützen, darf NovaOS ein Modul ohne vollständigen Systemneustart ersetzen.

```text
Old Module
    ↓
Prepare Replacement
    ↓
Transfer State
    ↓
Switch
    ↓
Verify
    ↓
Retire Old Module
```

Ein fehlgeschlagener Wechsel darf keinen undefinierten Mischzustand erzeugen.

## Sicherheit

Module erhalten ausschließlich die Capabilities, die sie für ihre Funktion benötigen.

Ein geladenes System Module erhält nicht automatisch globale System-Authority.

Integrität, Herkunft und Vertrauensstatus müssen vor privilegierter Aktivierung prüfbar sein.

## Isolation

Fehler eines Moduls sollen möglichst auf das betroffene Modul und seine Funktion begrenzt bleiben.

Module dürfen abhängig von ihren Anforderungen innerhalb oder außerhalb privilegierter Systembereiche ausgeführt werden.

## Normative Anforderungen

1. System Modules MÜSSEN stabile `ModuleID`s besitzen können.
2. Module MÜSSEN klar definierte Schnittstellen verwenden.
3. Module DÜRFEN nicht von undokumentierten internen Zuständen anderer Komponenten abhängen.
4. Modulabhängigkeiten MÜSSEN deklarierbar und versionierbar sein.
5. Fehlende oder inkompatible Abhängigkeiten MÜSSEN erkannt werden.
6. Module MÜSSEN kontrolliert geladen und entladen werden können.
7. Module SOLLEN Live Replacement unterstützen können.
8. Modulwechsel DÜRFEN keinen undefinierten Mischzustand erzeugen.
9. Module DÜRFEN keine implizite globale Authority erhalten.
10. Integrität und Trust MÜSSEN vor privilegierter Aktivierung prüfbar sein.
11. Modulfehler SOLLEN möglichst isoliert werden.
12. Modulzustand, Version, Abhängigkeiten und Schnittstellen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-FRAMEWORK-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Modell für modulare Systemkomponenten. Systemfunktionen können versioniert, ausgetauscht und erweitert werden, ohne Kernel, Programme oder andere Komponenten unnötig miteinander zu koppeln oder zusätzliche Authority zu erzeugen.