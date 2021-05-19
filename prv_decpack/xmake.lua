target("prv_depack")
    set_kind("binary")
    add_cxxflags("-std=c++11")
    set_optimize("faster")
    set_warnings("all")
    set_targetdir("../build")
    add_includedirs("..")
    add_includedirs("../include")
    add_linkdirs("../lib")
    add_links("base")
    add_files("*.cc")

    add_cxflags("-g3")
    add_syslinks("pthread", "dl")

    after_build(function ()
        os.rm("build")
    end)
    
