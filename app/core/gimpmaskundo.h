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

#ifndef __GIMP_MASK_UNDO_H__
#define __GIMP_MASK_UNDO_H__


#include "gimpitemundo.h"


#define GIMP_TYPE_MASK_UNDO            (gimp_mask_undo_get_type ())
#define GIMP_MASK_UNDO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_MASK_UNDO, GimpMaskUndo))
#define GIMP_MASK_UNDO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_MASK_UNDO, GimpMaskUndoClass))
#define GIMP_IS_MASK_UNDO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_MASK_UNDO))
#define GIMP_IS_MASK_UNDO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_MASK_UNDO))
#define GIMP_MASK_UNDO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_MASK_UNDO, GimpMaskUndoClass))


typedef struct _GimpMaskUndoClass GimpMaskUndoClass;

struct _GimpMaskUndo
{
  GimpItemUndo  parent_instance;

  TileManager  *tiles;
  gint          x;
  gint          y;
};

struct _GimpMaskUndoClass
{
  GimpItemUndoClass  parent_class;
};


GType   gimp_mask_undo_get_type (void) G_GNUC_CONST;


#endif /* __GIMP_MASK_UNDO_H__ */
