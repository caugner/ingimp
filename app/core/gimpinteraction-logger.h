/* The GIMP -- an image manipulation program
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

#ifndef __GIMP_INTERACTION_LOGGER_H__
#define __GIMP_INTERACTION_LOGGER_H__

int guilog_start(Gimp* gimp);
int guilog_stop(void);

void guilog_register_display_shell   ( void *shell);
void guilog_template_create_image    ( GimpImage     *gimage);

void guilog_create_image             ( GimpImage     *gimage,
                                       const gchar   *comment);

void guilog_revert_image             ( GimpImage     *old_gimage,
                                       GimpImage     *new_gimage);

void guilog_open_image               (const gchar     *uri,
                                      const gchar     *entered_filename,
                                      GimpImage       *gimage);

void guilog_last_opened              ( GimpImage     *gimage,
                                       gint           index,
                                       const gchar   *filename);

void guilog_save_image_requested     ( GimpImage     *gimage,
                                       gboolean       is_save_as);

void guilog_save_image               ( GimpImage     *gimage,
                                       const gchar   *uri,
                                       const gchar   *filename,
                                       gboolean       save_a_copy,
                                       GimpPDBStatusType status);

void guilog_image_duplicate          ( GimpImage     *old_gimage,
                                       GimpImage     *new_gimage);

void guilog_image_dispose            ( GimpImage     *gimage);

void guilog_undo_event               ( GimpImage     *gimage,
                                       GimpUndoEvent  event,
                                       GimpUndo      *undo);
void guilog_plugin_invoked           ( GimpProcedure  *proc,
                                       gboolean       reshow_last);
void guilog_plugin_returned          ( GimpProcedure *proc);
void guilog_plugin_closed            ( GimpProcedure *proc);
void guilog_register_action          ( void          *vp_action);
void guilog_register_window          ( void          *vp_window);

#endif /* __GIMP_INTERACTION_LOGGER_H__ */
