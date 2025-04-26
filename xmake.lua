add_rules("mode.debug", "mode.release")
set_languages("c++17")

add_requires("glad", "glfw", "assimp", "glm", "stb")

set_warnings("all")

target("learnopengl")
    set_kind("binary")
    set_default(true)

    add_files("src/*.cpp")
    add_includedirs("src/include")

    add_packages("glad", "glfw", "assimp", "glm", "stb")

    after_build(function (target)
        os.cp("resources", target:targetdir())
    end)

    add_rules("clean_resources")

rule("clean_resources")
    before_clean(function (target)
        os.rm(path.join(target:targetdir(), "resources"))
    end)