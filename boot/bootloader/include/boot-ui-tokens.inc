; NovaOS Boot UI – Referenz-Design-Tokens in Device Layout Units (DLU).
; Bei der 800x600-Referenzausgabe entspricht 1 DLU genau einem Pixel. Andere
; Backends rechnen DLU erst an der Präsentationsgrenze in Pixel um.

%ifndef NOVA_BOOT_UI_TOKENS_INC
%define NOVA_BOOT_UI_TOKENS_INC

NOVA_UI_REFERENCE_WIDTH       equ 800
NOVA_UI_REFERENCE_HEIGHT      equ 600
NOVA_UI_SAFE_LEFT             equ 32
NOVA_UI_SAFE_TOP              equ 24
NOVA_UI_SAFE_RIGHT            equ 768
NOVA_UI_SAFE_BOTTOM           equ 576
NOVA_UI_PANEL_X               equ 184
NOVA_UI_PANEL_Y               equ 55
NOVA_UI_PANEL_WIDTH           equ 567
NOVA_UI_PANEL_HEIGHT          equ 473
NOVA_UI_CONTENT_X             equ 204
NOVA_UI_CONTENT_RIGHT         equ 727
NOVA_UI_MENU_Y                equ 77
NOVA_UI_MENU_ITEM_HEIGHT      equ 62
NOVA_UI_MENU_ITEM_STRIDE      equ 68
NOVA_UI_MENU_ICON_X           equ 228
NOVA_UI_MENU_TEXT_X           equ 278
NOVA_UI_SPACE_XS              equ 4
NOVA_UI_SPACE_S               equ 8
NOVA_UI_SPACE_M               equ 12
NOVA_UI_SPACE_L               equ 16
NOVA_UI_SPACE_XL              equ 24
NOVA_UI_RADIUS_CONTROL        equ 8
NOVA_UI_RADIUS_PANEL          equ 12

%endif
