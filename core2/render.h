#ifndef __RENDER_H
#define __RENDER_H

void horz_render_normal		(unsigned char *src, unsigned char *dest);
void horz_render_hstretch	(unsigned char *src, unsigned char *dest);
void horz_render_hvstretch	(unsigned char *src, unsigned char *dest);

void vert_render_normal		(unsigned char *src, unsigned char *dest);

#endif
