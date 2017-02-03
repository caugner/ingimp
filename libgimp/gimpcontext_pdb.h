/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimpcontext_pdb.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* NOTE: This file is auto-generated by pdbgen.pl */

#ifndef __GIMP_CONTEXT_PDB_H__
#define __GIMP_CONTEXT_PDB_H__

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


gboolean             gimp_context_push               (void);
gboolean             gimp_context_pop                (void);
gchar*               gimp_context_get_paint_method   (void);
gboolean             gimp_context_set_paint_method   (const gchar            *name);
gboolean             gimp_context_list_paint_methods (gint                   *num_paint_methods,
                                                      gchar                ***paint_methods);
gboolean             gimp_context_get_foreground     (GimpRGB                *foreground);
gboolean             gimp_context_set_foreground     (const GimpRGB          *foreground);
gboolean             gimp_context_get_background     (GimpRGB                *background);
gboolean             gimp_context_set_background     (const GimpRGB          *background);
gboolean             gimp_context_set_default_colors (void);
gboolean             gimp_context_swap_colors        (void);
gdouble              gimp_context_get_opacity        (void);
gboolean             gimp_context_set_opacity        (gdouble                 opacity);
GimpLayerModeEffects gimp_context_get_paint_mode     (void);
gboolean             gimp_context_set_paint_mode     (GimpLayerModeEffects    paint_mode);
gchar*               gimp_context_get_brush          (void);
gboolean             gimp_context_set_brush          (const gchar            *name);
gchar*               gimp_context_get_pattern        (void);
gboolean             gimp_context_set_pattern        (const gchar            *name);
gchar*               gimp_context_get_gradient       (void);
gboolean             gimp_context_set_gradient       (const gchar            *name);
gchar*               gimp_context_get_palette        (void);
gboolean             gimp_context_set_palette        (const gchar            *name);
gchar*               gimp_context_get_font           (void);
gboolean             gimp_context_set_font           (const gchar            *name);


G_END_DECLS

#endif /* __GIMP_CONTEXT_PDB_H__ */
