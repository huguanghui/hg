target("libhv_test")
    -- set kind
    set_kind("binary")
    add_links("hv")
    add_syslinks("pthread")
    -- add files
    add_files("main.cpp")

target("nlog_test")
    -- set kind
    set_kind("binary")
    add_links("hv")
    add_syslinks("pthread")
    -- add files
    add_files("nlog_test.cpp")
