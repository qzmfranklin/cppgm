licenses(['notice'])
package(default_visibility = ['//visibility:public'])

icu_copts = [
    #'-fno-exceptions',
    '-I' + 'third_party/icu' + '/common',
    '-I' + 'third_party/icu' + '/i18n',
    '-I' + 'third_party/icu' + '/io',
    '-I' + 'third_party/icu' + '/tools/ctestfw',
    '-I' + 'third_party/icu' + '/tools/toolutil',
    '-DU_USING_ICU_NAMESPACE=0',
]

icu_linkopts = [
    '-ldl',
]

cc_test(
    name = 'test_icu',
    srcs = [
        'icu/test_icu.cpp',
    ],
    deps = [
        '//third_party/icu:icu',
    ],
    linkstatic = 1,
    copts = icu_copts,
    linkopts = icu_linkopts,
)

cc_test(
    name = 'test_UnicodeString',
    srcs = [
        'icu/gtest_UnicodeString.cpp',
    ],
    deps = [
        '//external:gflags',
        '//third_party/icu:icu',
        '//third_party/gtest:gtest_main',
    ],
    linkstatic = 1,
    copts = icu_copts,
    linkopts = icu_linkopts,
)
