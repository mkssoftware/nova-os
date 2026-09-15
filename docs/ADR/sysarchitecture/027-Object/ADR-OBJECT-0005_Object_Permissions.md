# ADR-OBJECT-0005 – Object Permissions

## Status

Angenommen

## Kategorie

Object Architecture / Security / Permissions

## Kontext

NovaOS verwaltet Dateien, Geräte, Prozesse, Dienste und andere Ressourcen über das Unified System Object Model.

Der Zugriff auf diese Objekte muss einheitlich kontrolliert werden können, ohne dass jeder Objekttyp ein vollständig eigenes Berechtigungsmodell benötigt.

## Entscheidung

NovaOS bindet **Permissions direkt an Systemobjekte**.

```text
Subject
   ↓
Permission Check
   ↓
Object
```

Eine Object ID allein gewährt keinen Zugriff.

## Grundmodell

Permissions beschreiben, welche Operationen ein Subject auf einem Objekt ausführen darf.

```text
Subject
Object
Operation
Context
    ↓
Permission Decision
```

## Operationen

Objekttypen können unterschiedliche Operationen definieren.

Beispiele:

```text
read
write
execute
modify
delete
enumerate
control
delegate
```

Permissions beziehen sich auf konkrete Operationen und nicht nur auf einen globalen Zugriffszustand.

## Subjects

Subjects können beispielsweise sein:

```text
User
Process
Service
Application
Capability
System Component
```

Die konkrete Subject Identity muss eindeutig bestimmbar sein.

## Capabilities

Object Permissions und Capability Security ergänzen sich.

```text
Capability available
        +
Object access permitted
        ↓
Operation allowed
```

Der Besitz einer Capability bedeutet nicht automatisch Zugriff auf jedes passende Objekt.

## Versionen

Permissions gehören grundsätzlich zur stabilen Object Identity.

Objektversionen können zusätzliche Einschränkungen besitzen, wenn dies erforderlich ist.

```text
Object ID
   ↓
Permissions
   ↓
Versions
```

## Beziehungen

Permissions dürfen nicht automatisch über Semantic Relationships übertragen werden.

```text
Access Object A
      ≠
Access Object B
```

Eine Beziehung zwischen zwei Objekten erzeugt keine implizite Zugriffsberechtigung.

## Vererbung

Objekttypen wie Verzeichnisse oder Container dürfen Permission-Vererbung unterstützen.

Vererbung muss jedoch explizit definiert und nachvollziehbar sein.

## Default

NovaOS verwendet **Fail-Safe Defaults**.

```text
No Permission
     ↓
Deny
```

Nicht eindeutig erlaubte Zugriffe werden abgelehnt.

## Normative Anforderungen

1. NovaOS MUSS Permissions auf Systemobjekte anwenden können.
2. Object Identity und Zugriffsberechtigung MÜSSEN getrennte Konzepte sein.
3. Permissions MÜSSEN operationsbezogen definierbar sein.
4. Subjects MÜSSEN eindeutig identifizierbar sein.
5. Nicht explizit erlaubte Zugriffe MÜSSEN standardmäßig abgelehnt werden.
6. Capabilities DÜRFEN Object Permissions nicht automatisch umgehen.
7. Semantic Relationships DÜRFEN keine impliziten Zugriffsrechte erzeugen.
8. Permission-Vererbung MUSS explizit definiert sein.
9. Permissions MÜSSEN unabhängig von Name und Pfad eines Objekts funktionieren.

## Konsequenzen

### Positive Konsequenzen

- einheitliches Berechtigungsmodell für Systemobjekte,
- Permissions bleiben bei Verschieben und Umbenennen erhalten,
- Capability Security und Object Security können kombiniert werden,
- Zugriffe können feingranular kontrolliert werden.

### Negative Konsequenzen

- Permission Checks liegen auf vielen Objektzugriffspfaden,
- Vererbung und Delegation benötigen klare Regeln.

## Abhängigkeiten

- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SEMANTIC-0006_Typed_Capabilities`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-PERMISSION-0001`
- `NPSPEC-OBJECT-PERMISSION-CHECK-0001`
- `NPSPEC-OBJECT-PERMISSION-INHERITANCE-0001`

## Ergebnis

NovaOS kontrolliert den Zugriff direkt am adressierten Systemobjekt:

```text
Subject
   +
Operation
   +
Object
   ↓
Permission Check
   ↓
Allow / Deny
```

Der zentrale Grundsatz lautet:

```text
Ein Objekt kann bekannt
und adressierbar sein,

ohne deshalb
zugänglich zu sein.
```