-- add target
target("tbox_test")

    -- set kind
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"tbox_test\"")

    -- add packages
    add_packages("tbox")

    -- add files
    add_files("*.cpp")

