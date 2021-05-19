add_rules("mode.debug", "mode.release")

add_requires("ffmpeg")

target("Frame_Parse")
    set_kind("binary")
    set_targetdir("./build")
    add_packages("ffmpeg")
    add_files("src/*.c")
