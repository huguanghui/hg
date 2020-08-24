-- add target
target("prtlog")

    -- set kind
    set_kind("binary")

    add_links("hv")

    -- add files
    add_files("*.cpp")

