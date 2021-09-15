#include "duk_print_alert.h"

#include <iostream>
#include <string>

#include <duktape.h>

#include "..\PartyFilterCore\utils.h"

/* Faster, less churn, higher footprint option. */
static duk_ret_t duk__print_alert_helper(duk_context* ctx, FILE* fh)
{
    duk_idx_t nargs;
    const duk_uint8_t* buf;
    duk_size_t sz_buf;
    const char nl = '\n';
    duk_uint8_t buf_stack[256];

    nargs = duk_get_top(ctx);

    /* If argument count is 1 and first argument is a buffer, write the buffer
     * as raw data into the file without a newline; this allows exact control
     * over stdout/stderr without an additional entrypoint (useful for now).
     * Otherwise current print/alert semantics are to ToString() coerce
     * arguments, join them with a single space, and append a newline.
     */

    if (nargs == 1 && duk_is_buffer_data(ctx, 0))
    {
        buf = (const duk_uint8_t*)duk_get_buffer_data(ctx, 0, &sz_buf);
    }
    else if (nargs > 0)
    {
        duk_idx_t i;
        duk_size_t sz_str;
        const duk_uint8_t* p_str;
        duk_uint8_t* p;

        sz_buf = (duk_size_t)nargs;  /* spaces (nargs - 1) + newline */
        for (i = 0; i < nargs; i++)
        {
            (void)duk_to_lstring(ctx, i, &sz_str);
            sz_buf += sz_str;
        }

        if (sz_buf <= sizeof(buf_stack))
        {
            p = (duk_uint8_t*)buf_stack;
        }
        else
        {
            p = (duk_uint8_t*)duk_push_fixed_buffer(ctx, sz_buf + 1);
        }

        buf = (const duk_uint8_t*)p;
        for (i = 0; i < nargs; i++)
        {
            p_str = (const duk_uint8_t*)duk_get_lstring(ctx, i, &sz_str);
            memcpy((void*)p, (const void*)p_str, sz_str);
            p += sz_str;
            *p++ = (duk_uint8_t)(i == nargs - 1 ? '\n' : ' ');
        }
        *p++ = 0;
    }
    else
    {
        buf = (const duk_uint8_t*)&nl;
        sz_buf = 1;
    }

    if (sz_buf > 0)
    {
        std::cout << buf << std::endl << std::flush;
    }

    return 0;
}

static duk_ret_t duk__print(duk_context* ctx)
{
    return duk__print_alert_helper(ctx, stdout);
}

static duk_ret_t duk__alert(duk_context* ctx)
{
    return duk__print_alert_helper(ctx, stderr);
}

void duk_print_alert_init(duk_context* ctx, duk_uint_t flags)
{
    (void)flags;  /* unused at the moment */

    /* XXX: use duk_def_prop_list(). */
    duk_push_global_object(ctx);
    duk_push_string(ctx, "print");
    duk_push_c_function(ctx, duk__print, DUK_VARARGS);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    duk_push_string(ctx, "alert");
    duk_push_c_function(ctx, duk__alert, DUK_VARARGS);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    duk_pop(ctx);
}
