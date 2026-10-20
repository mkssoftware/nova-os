/* NovaOS C-Kernel - minimaler Framebuffer-Zugriff (linearer GOP-Framebuffer
   aus dem BIB-Graphics-TLV, 32bpp). */
#ifndef NOVA_FRAMEBUFFER_H
#define NOVA_FRAMEBUFFER_H

#include <stdint.h>

/* 0 = Erfolg (Framebuffer vorhanden und uebernommen), -1 = kein Framebuffer. */
int framebuffer_initialize(uint32_t address, uint32_t pitch, uint32_t width,
                            uint32_t height, uint32_t bits_per_pixel);

int framebuffer_available(void);
uint32_t framebuffer_width(void);
uint32_t framebuffer_height(void);

void framebuffer_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rgb);
void framebuffer_clear(uint32_t rgb);

#endif
