add_rules("mode.debug", "mode.release")

set_kind("binary")
add_cxxflags("-std=c++11")
set_warnings("all")
set_targetdir("../build")
add_includedirs("..")
add_linkdirs("../lib")
add_links("base")

add_syslinks("pthread", "dl")

after_build(function ()
    os.rm("build")
end)

target("def_test")
    add_files("def_test.cc")
    
target("random_test")
    add_files("random_test.cc")

target("lrumap_test")
    add_files("lrumap_test.cc")

target("fast_test")
    add_files("fast_test.cc")

target("str_test")
    add_files("str_test.cc")
    
target("os_test")
    add_files("os_test.cc")
    
target("flag_test")
    add_files("flag_test.cc")

target("fastring_test")
    add_files("fastring_test.cc")
