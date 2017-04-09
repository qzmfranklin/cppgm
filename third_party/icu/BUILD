licenses(['notice'])
package(default_visibility = ['//visibility:__pkg__'])

filegroup(
        name = 'icu-data',
        srcs = [
                'data/in/icudt58l.dat',
        ],
)

# Cannot use the `includes` attribute of the cc_library target because
# `includes` does not apply to header files included via double quotes.
icu_copts = [
        '-Ithird_party/icu/common',
        '-Ithird_party/icu/i18n',
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
                'layoutex/**/*.c',
                'layoutex/**/*.cpp',
        ]),
        hdrs = glob([
                'common/**/*.h',
                'io/**/*.h',
                'i18n/**/*.h',
                'stubdata/**/*.h',
                'layoutex/**/*.h',
        ]),
        defines = [
                # disable `using namespace icu`
                'U_USING_ICU_NAMESPACE=0',
                # default to utf8 to speed up queries at runtime
                'U_CHARSET_IS_UTF8=1',
                # do not include unused header files
                'U_NO_DEFAULT_INCLUDE_UTF_HEADERS=1',

                # TODO: The following macros are recommended by the official
                # install guide.  Unfortunately, they break the tests shipped
                # with the source code.  Commented out for now.
                ## add `explicit` to ctors
                #'UNISTR_FROM_CHAR_EXPLICIT=explicit',
                ## add `explicit` to ctors
                #'UNISTR_FROM_STRING_EXPLICIT=explicit',
        ],
        copts = [
                # icu library does not raise or handle exceptions
                '-fno-exceptions',
                # required for building the files under common/
                '-DU_COMMON_IMPLEMENTATION',
                # required for building the files under io/
                '-DU_IO_IMPLEMENTATION',
                # required for building the files under i18n/
                '-DU_I18N_IMPLEMENTATION',

        ] + icu_copts,
        visibility = ["//visibility:public"],
)

icu_tools_copts = [
        '-Ithird_party/icu/tools/ctestfw',
        '-Ithird_party/icu/tools/toolutil',
]

cc_library(
        name = 'icu-tools',
        srcs = glob([
                'tools/ctestfw/**/*.c',
                'tools/ctestfw/**/*.cpp',
                'tools/toolutil/**/*.c',
                'tools/toolutil/**/*.cpp',
        ]),
        hdrs = glob([
                'tools/ctestfw/**/*.h',
                'tools/toolutil/**/*.h',
        ]),
        deps = [
                'icu',
        ],
        copts = [
                '-fno-exceptions',
                '-DU_TOOLUTIL_IMPLEMENTATION',
        ] + icu_copts + icu_tools_copts,
        visibility = ["//visibility:public"],
)

cc_binary(
        name = 'iotest',
        srcs = glob([
                'test/iotest/*.c',
                'test/iotest/*.cpp',
                'test/iotest/*.h',
        ]),
        deps = [
                'icu-tools',
        ],
        copts = [
                '-fno-exceptions',
                ## required for statically linked binaries
                #'-DU_STATIC_IMPLEMENTATION',
        ] + icu_copts + icu_tools_copts,
        linkopts = [
                '-ldl',
        ],
        data = [
                'icu-data',
        ],
)
