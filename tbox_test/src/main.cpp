/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox/tbox.h"

void test_tb_static_string()
{
    tb_static_string_t s;
    tb_char_t b[4096];
    
    // 初始化
    tb_static_string_init(&s, b, 4096);
    tb_static_string_cstrcpy(&s, "hello hu");
    tb_static_string_chrcat(&s, ' ');
    tb_static_string_cstrfcat(&s, "%s", "world");

    tb_trace_i("%s", tb_static_string_cstr(&s));

    // 反初始化
    tb_static_string_exit(&s);

    return;
}

void test_tb_string()
{
    tb_string_t s;
    tb_string_init(&s);
    tb_string_cstrcpy(&s, "hello");
    tb_string_chrcat(&s, ' ');
    tb_string_cstrfcat(&s, "%s", "world");
    tb_string_chrcat(&s, ' ');
    tb_string_chrncat(&s, 'x', 5);
    tb_trace_i("%d: %s", tb_string_size(&s), tb_string_cstr(&s));
    tb_string_exit(&s);
    return;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    // init tbox
    if (!tb_init(tb_null, tb_null)) return -1;

    // test static string
    test_tb_static_string();

    // test string
    test_tb_string();

    // vector
    tb_vector_ref_t vector = tb_vector_init(0, tb_element_str(tb_true));
    if (vector)
    {
        tb_vector_insert_head(vector, "hi!");

        tb_vector_insert_tail(vector, "how");
        tb_vector_insert_tail(vector, "are");
        tb_vector_insert_tail(vector, "you");

        // dump element
        tb_for_all(tb_char_t const*, cstr, vector)
        {
            tb_trace_i("%s", cstr);
        }

        tb_vector_exit(vector);
    }

    // exit tbox
    tb_exit();
    return 0;
}
