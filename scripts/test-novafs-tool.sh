#!/usr/bin/env bash
# Host-Selbsttest des NovaFS-Werkzeugs (NPSPEC-NOVAFS-ONDISK-0001).
# Verwendung: test-novafs-tool.sh <novafs-Werkzeug> <Arbeitsverzeichnis>
set -euo pipefail
TOOL=$1
WORK=$2
rm -rf "$WORK"
mkdir -p "$WORK"
IMG="$WORK/volume.img"

fail() { echo "novafs-check: FEHLER: $*" >&2; exit 1; }

"$TOOL" mkfs "$IMG" --size-mib 32 --uuid 4e6f76614f5300000000000000000c01 >/dev/null
"$TOOL" fsck "$IMG" >/dev/null || fail "fsck nach mkfs"
for dir in System Benutzer Apps Volumes Boot Solutions; do
    "$TOOL" ls "$IMG" / | grep -q " $dir\$" || fail "Grundlayout ohne /$dir"
done

# 120 Dateien mit wachsender Groesse: teilt Object-, Directory- und Extent-Tree.
"$TOOL" mkdir "$IMG" /Benutzer/Test
for i in $(seq 1 120); do
    head -c $((i * 211)) /dev/urandom > "$WORK/f$i"
    "$TOOL" put "$IMG" "$WORK/f$i" "/Benutzer/Test/datei-$i.bin"
done
head -c 600000 /dev/urandom > "$WORK/gross"
"$TOOL" put "$IMG" "$WORK/gross" /Apps/gross.bin
"$TOOL" fsck "$IMG" >/dev/null || fail "fsck nach Befuellung"
"$TOOL" tree "$IMG" | grep -q 'Object    Hoehe 2' || fail "Object Tree wurde nicht geteilt"
"$TOOL" tree "$IMG" | grep -q 'Directory Hoehe 2' || fail "Directory Tree wurde nicht geteilt"
"$TOOL" tree "$IMG" | grep -q 'Extent    Hoehe 2' || fail "Extent Tree wurde nicht geteilt"
for i in 1 13 14 27 55 56 99 120; do
    "$TOOL" cat "$IMG" "/Benutzer/Test/datei-$i.bin" | cmp -s - "$WORK/f$i" || fail "Inhalt datei-$i.bin"
done
"$TOOL" cat "$IMG" /Apps/gross.bin | cmp -s - "$WORK/gross" || fail "Inhalt gross.bin"
if "$TOOL" put "$IMG" "$WORK/f1" /Benutzer/Test/datei-1.bin 2>/dev/null; then fail "doppelter Name akzeptiert"; fi

# Umbenennen, Verschieben und Loeschen (leere Blaetter bleiben bestehen).
"$TOOL" mv "$IMG" /Benutzer/Test/datei-7.bin /Benutzer/Test/umbenannt.bin
"$TOOL" cat "$IMG" /Benutzer/Test/umbenannt.bin | cmp -s - "$WORK/f7" || fail "Inhalt nach mv"
if "$TOOL" cat "$IMG" /Benutzer/Test/datei-7.bin >/dev/null 2>&1; then fail "alter Name nach mv sichtbar"; fi
"$TOOL" mkdir "$IMG" /Benutzer/Ziel
"$TOOL" mv "$IMG" /Benutzer/Test/umbenannt.bin /Benutzer/Ziel/verschoben.bin
"$TOOL" cat "$IMG" /Benutzer/Ziel/verschoben.bin | cmp -s - "$WORK/f7" || fail "Inhalt nach Verschieben"
if "$TOOL" mv "$IMG" /Benutzer/Test/datei-8.bin /Benutzer/Ziel/verschoben.bin 2>/dev/null; then fail "mv ueberschreibt Ziel"; fi
if "$TOOL" mv "$IMG" /Benutzer/Ziel /Benutzer/Ziel/innen 2>/dev/null; then fail "Verzeichnis in sich selbst verschoben"; fi
if "$TOOL" rm "$IMG" /Benutzer 2>/dev/null; then fail "Namespace geloescht"; fi
if "$TOOL" rm "$IMG" /Benutzer/Ziel 2>/dev/null; then fail "nicht leeres Verzeichnis geloescht"; fi
"$TOOL" fsck "$IMG" >/dev/null || fail "fsck nach mv"
FREE_BEFORE=$("$TOOL" info "$IMG" | sed -n 's/.*frei \([0-9]*\).*/\1/p')
"$TOOL" rm "$IMG" /Apps/gross.bin
FREE_AFTER=$("$TOOL" info "$IMG" | sed -n 's/.*frei \([0-9]*\).*/\1/p')
[ $((FREE_AFTER - FREE_BEFORE)) -eq 147 ] || fail "rm gab $((FREE_AFTER - FREE_BEFORE)) statt 147 Bloecke frei"
for i in $(seq 1 120); do
    [ "$i" -eq 7 ] && continue
    "$TOOL" rm "$IMG" "/Benutzer/Test/datei-$i.bin"
done
"$TOOL" rm "$IMG" /Benutzer/Test
"$TOOL" fsck "$IMG" >/dev/null || fail "fsck nach rm"
"$TOOL" tree "$IMG" | grep -Eq 'Extent    Hoehe 2:.* 0( |$)' || fail "erwartete leere Extent-Blaetter"
for i in $(seq 1 30); do
    "$TOOL" put "$IMG" "$WORK/f$i" "/Benutzer/Ziel/neu-$i.bin"
done
"$TOOL" cat "$IMG" /Benutzer/Ziel/neu-30.bin | cmp -s - "$WORK/f30" || fail "Inhalt nach Wiederbefuellung"
"$TOOL" fsck "$IMG" >/dev/null || fail "fsck nach Wiederbefuellung"

# Beschaedigter primaerer Superblock: Backup muss uebernehmen.
cp "$IMG" "$WORK/corrupt.img"
printf '\377' | dd of="$WORK/corrupt.img" bs=1 seek=$((4096 + 200)) conv=notrunc status=none
"$TOOL" info "$WORK/corrupt.img" 2>&1 | grep -q 'Backup verwendet' || fail "Backup-Superblock nicht verwendet"
"$TOOL" fsck "$WORK/corrupt.img" >/dev/null 2>&1 || fail "fsck mit Backup-Superblock"

# Beschaedigter Baumknoten: fsck muss abbrechen.
ROOT_BLOCK=$(od -An -t u8 -j $((4096 + 56)) -N 8 "$IMG" | tr -d ' ')
cp "$IMG" "$WORK/node.img"
printf '\377' | dd of="$WORK/node.img" bs=1 seek=$((ROOT_BLOCK * 4096 + 100)) conv=notrunc status=none
if "$TOOL" fsck "$WORK/node.img" >/dev/null 2>&1; then fail "beschaedigter Knoten nicht erkannt"; fi

# DIRTY-Volume darf nicht beschrieben werden.
cp "$IMG" "$WORK/dirty.img"
"$TOOL" mark-dirty "$WORK/dirty.img"
"$TOOL" info "$WORK/dirty.img" | grep -q 'DIRTY' || fail "DIRTY-Zustand nicht sichtbar"
if "$TOOL" put "$WORK/dirty.img" "$WORK/f1" /Apps/neu.bin 2>/dev/null; then fail "Schreiben auf DIRTY-Volume erlaubt"; fi

"$TOOL" fsck "$IMG"
echo "novafs-check: Host-Werkzeug, Baumteilung, Loeschen/Umbenennen, Backup-Superblock und Fehlererkennung erfolgreich"
