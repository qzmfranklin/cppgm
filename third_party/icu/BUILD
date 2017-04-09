licenses(['notice'])
package(default_visibility = ['//visibility:__pkg__'])

_exclude_pattern = [
        'extra/**',
        'samples/**',
        'test/**',
        'tools/**',
]

icu_includes = [
]

cc_library(
        name = 'icu',
        srcs = glob([
                'common/**/*.c',
                'common/**/*.cpp',
                'io/**/*.c',
                'io/**/*.cpp',
                'i18n/**/*.c',
                'i18n/**/*.cpp',
                'stubdata/**/*.c',
                'stubdata/**/*.cpp',
                #'layoutex/**/*.c',
                #'layoutex/**/*.cpp',
        ]),
        hdrs = glob([
                'common/**/*.h',
                'io/**/*.h',
                'i18n/**/*.h',
                'stubdata/**/*.h',
                #'layoutex/**/*.h',
        ]),
        copts = [
                '-fno-exceptions',
                '-Ithird_party/icu/common',
                '-Ithird_party/icu/i18n',
        ],
        visibility = ["//visibility:public"],
)
