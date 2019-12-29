set_kind("binary")
add_cxxflags("-std=c++11")
set_warnings("all")
set_targetdir("../build")
add_includedirs("..")
add_linkdirs("../lib")
add_links("base")

after_build(function ()
    os.rm("build")
end)

if is_plat("macosx", "linux") then
    add_syslinks("pthread", "dl")
end

target("time_unitest")
    add_files("time_unitest.cc")

