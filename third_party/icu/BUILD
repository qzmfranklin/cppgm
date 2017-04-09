licenses(['notice'])
package(default_visibility = ['//visibility:public'])

icu_copts = [
    '-fno-exceptions',
    '-I' + PACKAGE_NAME + '/common',
    '-I' + PACKAGE_NAME + '/i18n',
    '-I' + PACKAGE_NAME + '/tools/ctestfw',
    '-I' + PACKAGE_NAME + '/tools/toolutil',
]

icu_linkopts = [
    '-ldl',
]

cc_library(
    visibility = ['//visibility:__pkg__'],
    name = 'icu-base',
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
        # Officially recommended settings.
        'U_USING_ICU_NAMESPACE=0',
        'U_CHARSET_IS_UTF8=1',
        'U_NO_DEFAULT_INCLUDE_UTF_HEADERS=1',

        # Without -DU_HAVE_ELF_H=1, the tools/toolutil/pkg_genc.c will not
        # export the symbol 'writeObjectCode'.  Discovered via `VERBOSE make`.
        'U_HAVE_ELF_H=1',
        '_REENRAN=1',
        'U_HAVE_STRTOD_L=1',

        # TODO: The following macros are recommended by the official
        # install guide.  Unfortunately, they break the tests shipped
        # with the source code.  Commented out for now.
        ## add `explicit` to ctors
        #'UNISTR_FROM_CHAR_EXPLICIT=explicit',
        ## add `explicit` to ctors
        #'UNISTR_FROM_STRING_EXPLICIT=explicit',
    ],
    copts = icu_copts + [
        # required for building the files under common/
        '-DU_COMMON_IMPLEMENTATION',
        # required for building the files under io/
        '-DU_IO_IMPLEMENTATION',
        # required for building the files under i18n/
        '-DU_I18N_IMPLEMENTATION',

    ],
)

cc_library(
    visibility = ["//visibility:__pkg__"],
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
        'icu-base',
    ],
    copts = icu_copts + [
        '-DU_TOOLUTIL_IMPLEMENTATION',
    ]
)


# The complete list of tools is:
#
#     derb     genccode   gencmn     gendict    genrb      icupkg     pkgdata
#     genbrk   gencfu     gencnval   gennorm2   gensprep   makeconv   uconv
#
# Only build icupkg and pkgdata here as they are needed for building libicudata.

cc_binary(
    name = 'icupkg',
    srcs = [
        'tools/icupkg/icupkg.cpp',
    ],
    deps = [
        'icu-tools',
    ],
    copts = icu_copts,
    linkopts = icu_linkopts,
)

cc_binary(
    name = 'pkgdata',
    srcs = [
        'tools/pkgdata/pkgdata.cpp',
        'tools/pkgdata/pkgtypes.c',
        'tools/pkgdata/pkgtypes.h',
    ],
    deps = [
        'icu-tools',
    ],
    copts = icu_copts,
    linkopts = icu_linkopts,
)

genrule(
    name = 'libicudata_a',
    srcs = [
        'data/in/icudt58l.dat',
        'data/in/icupkg.inc',
    ],
    tools = [
        ':icupkg',
        ':pkgdata',
    ],
    outs = [
        'libicudata.a',
    ],
    cmd = 'true' + \
        ' && mkdir -p @D/out' + \
        ' && mkdir -p @D/out/build' + \
        ' && mkdir -p @D/out/build/icudt58l' + \
        ' && mkdir -p @D/out/build/icudt58l/curr' + \
        ' && mkdir -p @D/out/build/icudt58l/lang' + \
        ' && mkdir -p @D/out/build/icudt58l/region' + \
        ' && mkdir -p @D/out/build/icudt58l/zone' + \
        ' && mkdir -p @D/out/build/icudt58l/unit' + \
        ' && mkdir -p @D/out/build/icudt58l/brkitr' + \
        ' && mkdir -p @D/out/build/icudt58l/coll' + \
        ' && mkdir -p @D/out/build/icudt58l/rbnf' + \
        ' && mkdir -p @D/out/build/icudt58l/translit' + \
        ' && mkdir -p @D/out/tmp' + \
        ' && mkdir -p @D/out/tmp/curr' + \
        ' && mkdir -p @D/out/tmp/lang' + \
        ' && mkdir -p @D/out/tmp/region' + \
        ' && mkdir -p @D/out/tmp/zone' + \
        ' && mkdir -p @D/out/tmp/unit' + \
        ' && mkdir -p @D/out/tmp/coll' + \
        ' && mkdir -p @D/out/tmp/rbnf' + \
        ' && mkdir -p @D/out/tmp/translit' + \
        ' && mkdir -p @D/out/tmp/brkitr' + \
        #../bin/icupkg -d ./out/build/icudt58l --list -x \* ../../data/in/icudt58l.dat -o out/tmp/icudata.lst
        ' && $(location :icupkg) -d @D/out/build/icudt58l --list -x \* $(location data/in/icudt58l.dat) -o @D/out/tmp/icudata.lst' + \
        #../bin/pkgdata -O ../data/icupkg.inc -q -c -s /home/zhongming/git/cppgm/third_party/icu/build/data/out/build/icudt58l -d ../lib -e icudt58  -T ./out/tmp -p icudt58l -m static -r 58.2 -L icudata ./out/tmp/icudata.lst
        ' && $(location :pkgdata) -O $(location data/in/icupkg.inc) -q -c -s @D/out/build/icudt58l -d @D/out -e icudt58  -T @D/out/tmp -p icudt58l -m static -r 58.2 -L icudata @D/out/tmp/icudata.lst' + \
        ' && mv @D/out/libicudata.a $@' + \
        '',
)

#cc_library(
    #name = 'icu-data',
#)

cc_library(
    name = 'icu',
    deps = [
        'icu-base',
        #'icu-data',
    ],
    copts = icu_copts,
)

cc_binary(
    name = 'iotest',
    srcs = glob([
        'test/iotest/*.c',
        'test/iotest/*.cpp',
        'test/iotest/*.h',
    ]),
    deps = [
        'icu',
    ],
    copts = icu_copts,
    linkopts = icu_linkopts,
)
