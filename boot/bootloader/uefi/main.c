/**
 * Nova OS Bootloader (UEFI)
 * Datei: main.c
 *
 * Aufgabe:
 * - Einstiegspunkt des UEFI-Bootloaders
 * - Initialisiert UEFI-Dienste
 * - Zeigt Boot-Status an
 * - Wartet kurz (2 Sekunden)
 * - Prüft Tasteneingaben:
 *      F5  -> Boot-Menü (Recovery / Tools)
 *      F8  -> Erweiterte Optionen
 *      ESC -> Abbruch / Diagnose
 *      sonst -> Kernel starten
 *
 * WICHTIG:
 * Dieser Code läuft VOR dem Betriebssystem.
 * Daher sind nur UEFI-APIs verfügbar.
 */

#include <efi.h>
#include <efilib.h>

#define WARTEZEIT_MS 2000

// -----------------------------
// Hilfsfunktion: deutsche Ausgabe
// -----------------------------
static void schreibeText(EFI_SYSTEM_TABLE *SystemTable, CHAR16 *text)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, text);
}

// -----------------------------
// einfache Verzögerung
// (keine OS-Funktionen verfügbar)
// -----------------------------
static void warte(EFI_BOOT_SERVICES *BootServices, UINTN millisekunden)
{
    // UEFI arbeitet mit 100ns Intervallen
    BootServices->Stall(millisekunden * 1000);
}

// -----------------------------
// Tastatur prüfen (nicht-blockierend)
// -----------------------------
static EFI_INPUT_KEY leseTaste(EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_INPUT_KEY key;
    EFI_STATUS status;

    // Standardwert (keine Taste)
    key.ScanCode = 0;
    key.UnicodeChar = 0;

    // Prüfen ob Taste vorhanden
    status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);

    if (EFI_ERROR(status))
    {
        key.ScanCode = 0;
        key.UnicodeChar = 0;
    }

    return key;
}

// -----------------------------
// Kernel Start (Platzhalter)
// -----------------------------
static void starteKernel()
{
    // Später: ELF laden + Übergabe an Kernel
    Print(L"\n[NOVA] Kernel wird gestartet...\n");
}

// -----------------------------
// Bootmenü starten (Platzhalter)
// -----------------------------
static void starteBootMenue()
{
    Print(L"\n[NOVA] Boot-Menü wird gestartet (Recovery / Tools)...\n");
}

// -----------------------------
// Hauptfunktion (UEFI Entry Point)
// -----------------------------
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    schreibeText(SystemTable, L"\n====================================\n");
    schreibeText(SystemTable, L"        NOVA OS BOOTLOADER         \n");
    schreibeText(SystemTable, L"====================================\n\n");

    schreibeText(SystemTable, L"[NOVA] UEFI erfolgreich initialisiert...\n");
    schreibeText(SystemTable, L"[NOVA] Starte Hardware-Pruefung...\n");

    // Wartezeit für Benutzer (2 Sekunden)
    schreibeText(SystemTable, L"[NOVA] F5 => Boot-Menue\n");
    schreibeText(SystemTable, L"[NOVA] F8 => erweiterte Optionen\n");
    schreibeText(SystemTable, L"[NOVA] ESC => Diagnose\n");
    schreibeText(SystemTable, L"[NOVA] Automatischer Start in 2 Sekunden...\n");

    warte(SystemTable->BootServices, WARTEZEIT_MS);

    // Tastatur abfragen
    EFI_INPUT_KEY key = leseTaste(SystemTable);

    // Entscheidung
    if (key.ScanCode == 0x3F) // F5 (UEFI ScanCode)
    {
        starteBootMenue();
        return EFI_SUCCESS;
    }
    else if (key.ScanCode == 0x42) // F8
    {
        Print(L"\n[NOVA] Erweiterte Optionen gestartet...\n");
        starteBootMenue();
        return EFI_SUCCESS;
    }
    else if (key.ScanCode == 0x17) // ESC
    {
        Print(L"\n[NOVA] Diagnosemodus gestartet...\n");
        starteBootMenue();
        return EFI_SUCCESS;
    }

    // Standard: Kernel starten
    starteKernel();

    return EFI_SUCCESS;
}