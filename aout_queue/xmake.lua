add_rules("mode.debug", "mode.release")

target("aout_queue")
    set_kind("binary")
    set_targetdir("./build")
    add_files("src/*.c")

