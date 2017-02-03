
#ifndef __gimp_marshal_MARSHAL_H__
#define __gimp_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* BOOLEAN:BOOLEAN (./gimpmarshal.list:25) */
extern void gimp_marshal_BOOLEAN__BOOLEAN (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);

/* BOOLEAN:ENUM,INT (./gimpmarshal.list:26) */
extern void gimp_marshal_BOOLEAN__ENUM_INT (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

/* BOOLEAN:OBJECT,POINTER (./gimpmarshal.list:27) */
extern void gimp_marshal_BOOLEAN__OBJECT_POINTER (GClosure     *closure,
                                                  GValue       *return_value,
                                                  guint         n_param_values,
                                                  const GValue *param_values,
                                                  gpointer      invocation_hint,
                                                  gpointer      marshal_data);

/* BOOLEAN:OBJECT,POINTER,STRING (./gimpmarshal.list:28) */
extern void gimp_marshal_BOOLEAN__OBJECT_POINTER_STRING (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);

/* VOID:BOOLEAN (./gimpmarshal.list:30) */
#define gimp_marshal_VOID__BOOLEAN	g_cclosure_marshal_VOID__BOOLEAN

/* VOID:BOOLEAN,INT,INT,INT,INT (./gimpmarshal.list:31) */
extern void gimp_marshal_VOID__BOOLEAN_INT_INT_INT_INT (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);

/* VOID:BOXED (./gimpmarshal.list:32) */
#define gimp_marshal_VOID__BOXED	g_cclosure_marshal_VOID__BOXED

/* VOID:BOXED,ENUM (./gimpmarshal.list:33) */
extern void gimp_marshal_VOID__BOXED_ENUM (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);

/* VOID:DOUBLE (./gimpmarshal.list:34) */
#define gimp_marshal_VOID__DOUBLE	g_cclosure_marshal_VOID__DOUBLE

/* VOID:DOUBLE,DOUBLE (./gimpmarshal.list:35) */
extern void gimp_marshal_VOID__DOUBLE_DOUBLE (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* VOID:ENUM (./gimpmarshal.list:36) */
#define gimp_marshal_VOID__ENUM	g_cclosure_marshal_VOID__ENUM

/* VOID:ENUM,ENUM,BOXED,INT (./gimpmarshal.list:37) */
extern void gimp_marshal_VOID__ENUM_ENUM_BOXED_INT (GClosure     *closure,
                                                    GValue       *return_value,
                                                    guint         n_param_values,
                                                    const GValue *param_values,
                                                    gpointer      invocation_hint,
                                                    gpointer      marshal_data);

/* VOID:ENUM,OBJECT (./gimpmarshal.list:38) */
extern void gimp_marshal_VOID__ENUM_OBJECT (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

/* VOID:ENUM,POINTER (./gimpmarshal.list:39) */
extern void gimp_marshal_VOID__ENUM_POINTER (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

/* VOID:FLAGS (./gimpmarshal.list:40) */
#define gimp_marshal_VOID__FLAGS	g_cclosure_marshal_VOID__FLAGS

/* VOID:INT (./gimpmarshal.list:41) */
#define gimp_marshal_VOID__INT	g_cclosure_marshal_VOID__INT

/* VOID:INT,INT (./gimpmarshal.list:42) */
extern void gimp_marshal_VOID__INT_INT (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

/* VOID:INT,INT,INT,INT (./gimpmarshal.list:43) */
extern void gimp_marshal_VOID__INT_INT_INT_INT (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);

/* VOID:OBJECT (./gimpmarshal.list:44) */
#define gimp_marshal_VOID__OBJECT	g_cclosure_marshal_VOID__OBJECT

/* VOID:OBJECT,INT (./gimpmarshal.list:45) */
extern void gimp_marshal_VOID__OBJECT_INT (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);

/* VOID:OBJECT,OBJECT (./gimpmarshal.list:46) */
extern void gimp_marshal_VOID__OBJECT_OBJECT (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* VOID:OBJECT,POINTER (./gimpmarshal.list:47) */
extern void gimp_marshal_VOID__OBJECT_POINTER (GClosure     *closure,
                                               GValue       *return_value,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint,
                                               gpointer      marshal_data);

/* VOID:POINTER (./gimpmarshal.list:48) */
#define gimp_marshal_VOID__POINTER	g_cclosure_marshal_VOID__POINTER

/* VOID:POINTER,BOXED (./gimpmarshal.list:49) */
extern void gimp_marshal_VOID__POINTER_BOXED (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* VOID:POINTER,ENUM (./gimpmarshal.list:50) */
extern void gimp_marshal_VOID__POINTER_ENUM (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

/* VOID:STRING (./gimpmarshal.list:51) */
#define gimp_marshal_VOID__STRING	g_cclosure_marshal_VOID__STRING

/* VOID:STRING,BOOLEAN,UINT,FLAGS (./gimpmarshal.list:52) */
extern void gimp_marshal_VOID__STRING_BOOLEAN_UINT_FLAGS (GClosure     *closure,
                                                          GValue       *return_value,
                                                          guint         n_param_values,
                                                          const GValue *param_values,
                                                          gpointer      invocation_hint,
                                                          gpointer      marshal_data);

/* VOID:STRING,FLAGS (./gimpmarshal.list:53) */
extern void gimp_marshal_VOID__STRING_FLAGS (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

/* VOID:STRING,STRING,STRING (./gimpmarshal.list:54) */
extern void gimp_marshal_VOID__STRING_STRING_STRING (GClosure     *closure,
                                                     GValue       *return_value,
                                                     guint         n_param_values,
                                                     const GValue *param_values,
                                                     gpointer      invocation_hint,
                                                     gpointer      marshal_data);

/* VOID:VOID (./gimpmarshal.list:55) */
#define gimp_marshal_VOID__VOID	g_cclosure_marshal_VOID__VOID

G_END_DECLS

#endif /* __gimp_marshal_MARSHAL_H__ */

