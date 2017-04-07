licenses(['notice'])
package(default_visibility = ['//visibility:public'])


cc_library(
        name = 'libtools',
        srcs = glob([
                'tools/**/*.c',
                'tools/**/*.cpp',
                'tools/**/*.h',
        ], exclude = [
                'tools/icu*/*',
                'tools/gen*/*',
                'tools/pkgdata/*',
                'tools/tzcode/*',
        ]),
        deps = [
                ':libicu',
        ],
        copts = [
                '-iquote third_party/icu/common',
                '-iquote third_party/icu/i18n',
                '-iquote third_party/icu/tools/toolutil',
                '-iquote third_party/icu/tools/ctestfw',
                '-DU_TOOLUTIL_IMPLEMENTATION',
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = 'lib',
        deps = [
                'io',
                'i18n',
                'le',
                'lx',
                'uc',
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = 'lx',
        srcs = glob([
                'lx/**/*.c',
                'lx/**/*.cpp',
        ]),
        hdrs = glob([
                'lx/**/*.h',
        ]),
        copts = [
                '-fno-exceptions',
        ],
)

cc_library(
        name = 'uc',
        srcs = glob([
                'uc/**/*.c',
                'uc/**/*.cpp',
        ]),
        hdrs = glob([
                'uc/**/*.h',
        ]),
        copts = [
                '-fno-exceptions',
        ],
)

