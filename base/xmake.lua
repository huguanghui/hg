target("base")
    set_kind("static")
    set_warnings("all")
    set_targetdir("../lib")
    add_files("**.cc")

    after_build(function ()
        os.rm("build")
    end)
