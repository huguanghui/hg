set_kind("binary")
set_warnings("all")
set_targetdir("../build")
add_includedirs("..")
add_linkdirs("../lib")
add_links("base")

after_build(function ()
    os.rm("build")
end)

target("def_test")
    add_files("def_test.cc")
