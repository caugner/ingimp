/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GIMP_DISPLAY_FOREACH_H__
#define __GIMP_DISPLAY_FOREACH_H__


gboolean        gimp_displays_dirty            (Gimp      *gimp);
GimpContainer * gimp_displays_get_dirty_images (Gimp      *gimp);
void            gimp_displays_delete           (Gimp      *gimp);
void            gimp_displays_reconnect        (Gimp      *gimp,
                                                GimpImage *old,
                                                GimpImage *new);

void            gimp_displays_set_busy         (Gimp      *gimp);
void            gimp_displays_unset_busy       (Gimp      *gimp);


#endif /*  __GIMP_DISPLAY_FOREACH_H__  */