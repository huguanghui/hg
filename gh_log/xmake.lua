target("gh_log")
    set_kind("static")
    add_includedirs("./inc", {public = true})
    add_files("src/*.c")
