/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * 2023 Beaver Shadow for Gimp Layers
 */


#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

enum_start (gegl_gimplayershadow_grow_shape)
  enum_value (GEGL_DROPSHADOW_GROW_SHAPE_SQUARE,  "square",  N_("Square"))
  enum_value (GEGL_DROPSHADOW_GROW_SHAPE_CIRCLE,  "circle",  N_("Circle"))
  enum_value (GEGL_DROPSHADOW_GROW_SHAPE_DIAMOND, "diamond", N_("Diamond"))
enum_end (GeglgimplayershadowGrowShape)



property_double (x, _("X Horizontal Movability"), 0.0)
  description   (_("Horizontal shadow offset"))
  ui_range      (-40.0, 40.0)
  ui_steps      (1, 10)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "x")

property_double (y, _("Y Veritical Movability"), 0.0)
  description   (_("Vertical shadow offset"))
  ui_range      (-40.0, 40.0)
  ui_steps      (1, 10)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "y")

property_double (radius, _("Shadow Blur Radius"), 23)
   description (_("Blur the shadow. At very low settings the shadow will be solid and have outline properties."))
   value_range (0.0, 1500.0)
   ui_range    (0.24, 100.0)
   ui_gamma    (3.0)
   ui_meta     ("unit", "pixel-distance")

property_enum   (growshape, _("Grow shape"),
                 GeglgimplayershadowGrowShape, gegl_gimplayershadow_grow_shape,
                 GEGL_DROPSHADOW_GROW_SHAPE_CIRCLE)
  description   (_("The base shape to expand the shadow in"))


property_double (growradius, _("Shadow Grow radius"), 0.0)
  value_range   (0.0, 100.0)
  ui_range      (0.0, 50.0)
  ui_digits     (0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
  description (_("The distance to expand the shadow."))

property_color (color, _("Color"), "#000000")
    description (_("The color of the shadow."))


property_double (opacity, _("Shadow Opacity"), 1)
   description (_("HyperOpacity of the shadow"))
   value_range (0.0, 2.0)
   ui_range    (0.0, 2.0)
   ui_gamma    (1.0)
   ui_meta     ("unit", "pixel-distance")



#else

#define GEGL_OP_META
#define GEGL_OP_NAME     layershadow
#define GEGL_OP_C_SOURCE layershadow.c

#include "gegl-op.h"



static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *color, *move, *repair, *opacity, *blur, *median, *output;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");


 move   = gegl_node_new_child (gegl,
                                  "operation", "gegl:translate",  NULL);

 opacity   = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",  NULL);



 color   = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",  NULL);

  blur      = gegl_node_new_child (gegl, "operation", "gegl:gaussian-blur",
                                         "clip-extent", FALSE,
                                         "abyss-policy", 0, NULL);

  median     = gegl_node_new_child (gegl, "operation", "gegl:median-blur",
                                         "percentile",       100.0,
                                         "alpha-percentile", 100.0,
                                         "abyss-policy",     GEGL_ABYSS_NONE,
                                         NULL);

 repair   = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur", "radius", 0, "abyss-policy", 0,  NULL);


  gegl_node_link_many (input, median, blur, opacity, move, color, repair,  output, NULL);




  gegl_operation_meta_redirect (operation, "color", color, "value");
  gegl_operation_meta_redirect (operation, "radius", blur, "std-dev-x");
  gegl_operation_meta_redirect (operation, "radius", blur, "std-dev-y");
  gegl_operation_meta_redirect (operation, "growradius", median, "radius");
  gegl_operation_meta_redirect (operation, "growshape", median, "neighborhood");
  gegl_operation_meta_redirect (operation, "x", move, "x");
  gegl_operation_meta_redirect (operation, "y", move, "y");
  gegl_operation_meta_redirect (operation, "opacity", opacity, "value");


}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:shadow",
    "title",       _("Gimp Layer Shadow"),
    "categories",  "plugins",
    "reference-hash", "shea2aao1wbgxxft27730vaefe2g4f1b2ac",
    "description", _("Makes a shadow/glow meant to be used on individual Gimp layers. Duplicate your alpha channel present layer. Now apply this filter. Now the shadow is a layer of its own and can be moved using Gimp's move tool."
                     ""),
    NULL);
}

#endif
