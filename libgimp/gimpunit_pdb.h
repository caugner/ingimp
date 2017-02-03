/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimpunit_pdb.h
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

#ifndef __GIMP_UNIT_PDB_H__
#define __GIMP_UNIT_PDB_H__

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


G_GNUC_INTERNAL gint     _gimp_unit_get_number_of_units          (void);
G_GNUC_INTERNAL gint     _gimp_unit_get_number_of_built_in_units (void);
G_GNUC_INTERNAL GimpUnit _gimp_unit_new                          (const gchar *identifier,
                                                                  gdouble      factor,
                                                                  gint         digits,
                                                                  const gchar *symbol,
                                                                  const gchar *abbreviation,
                                                                  const gchar *singular,
                                                                  const gchar *plural);
G_GNUC_INTERNAL gboolean _gimp_unit_get_deletion_flag            (GimpUnit     unit_id);
G_GNUC_INTERNAL gboolean _gimp_unit_set_deletion_flag            (GimpUnit     unit_id,
                                                                  gboolean     deletion_flag);
G_GNUC_INTERNAL gchar*   _gimp_unit_get_identifier               (GimpUnit     unit_id);
G_GNUC_INTERNAL gdouble  _gimp_unit_get_factor                   (GimpUnit     unit_id);
G_GNUC_INTERNAL gint     _gimp_unit_get_digits                   (GimpUnit     unit_id);
G_GNUC_INTERNAL gchar*   _gimp_unit_get_symbol                   (GimpUnit     unit_id);
G_GNUC_INTERNAL gchar*   _gimp_unit_get_abbreviation             (GimpUnit     unit_id);
G_GNUC_INTERNAL gchar*   _gimp_unit_get_singular                 (GimpUnit     unit_id);
G_GNUC_INTERNAL gchar*   _gimp_unit_get_plural                   (GimpUnit     unit_id);


G_END_DECLS

#endif /* __GIMP_UNIT_PDB_H__ */