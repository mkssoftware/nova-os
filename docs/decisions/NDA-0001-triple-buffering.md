# NDA-0001 - Triple Buffering als Standard

## Status

Angenommen

## Entscheidung

Nova OS verwendet Triple Buffering als Standardmodell für grafische Ausgabe.

## Begründung

- reduziert Flackern
- verbessert Animationen
- entkoppelt Rendering von Bildausgabe
- bereitet den Compositor vor
- erleichtert spätere GPU-Beschleunigung

## Betroffene Bereiche

- Nova Graphics Engine
- Compositor
- Desktop
- Recovery UI
- Boot UI