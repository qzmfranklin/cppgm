licenses(['notice'])
package(default_visibility = ['//visibility:__pkg__'])

cc_library(
    name = 'CodeUnit',
    srcs = [
        'PPCodeUnit.cpp',
    ],
    hdrs = [
        'PPCodeUnit.h',
    ],
    deps = [
        '//utils:utils',
        ':CodePointCheck',
    ],
)

cc_library(
    name = 'CodeUnitCheck',
    srcs = [
        'PPCodeUnitCheck.cpp',
    ],
    hdrs = [
        'PPCodeUnitCheck.h',
    ],
    deps = [
        ':CodeUnit',
    ],
)

cc_library(
    name = 'UTF32Stream',
    srcs = [
        'PPUTF32Stream.cpp',
    ],
    hdrs = [
        'PPUTF32Stream.h',
        'UTF32StreamIfc.h',
    ],
    deps = [
        ':CodeUnitCheck',
    ],
)

cc_library(
    name = 'CodeUnitStream',
    srcs = [
        'PPCodeUnitStream.cpp',
    ],
    hdrs = [
        'PPCodeUnitStream.h',
        'PPCodeUnitStreamIfc.h',
    ],
    deps = [
        ':CodeUnitCheck',
        ':UTF32Stream',
    ],
)

cc_library(
    name = 'Token',
    srcs = [
        'PPToken.cpp',
    ],
    hdrs = [
        'PPToken.h',
    ],
)

cc_library(
    name = 'CodePointCheck',
    srcs = [
        'PPCodePointCheck.cpp',
    ],
    hdrs = [
        'PPCodePointCheck.h',
    ],
)

cc_library(
    name = 'TokenizerDFA',
    srcs = [
        'PPTokenizerDFA.cpp',
    ],
    hdrs = [
        'PPTokenizerDFA.h',
    ],
    deps = [
        ':Token',
        ':CodePointCheck',
        ':CodeUnitStream',
    ],
)

cc_binary(
    name = 'pptok',
    visibility = [
        '//visibility:public',
    ],
    srcs = [
        'pptok.cpp',
    ],
    deps = [
        ':TokenizerDFA',
    ],
    linkstatic = 1,
)

cc_test(
    name = 'gtest_PPToken',
    srcs = [
        'gtest_PPToken.cpp',
        'PPToken.cpp',
        'PPToken.h',
    ],
    deps = [
        '//third_party/gtest:gtest_main',
    ],
)

cc_test(
    name = 'gtest_PPCodePointCheck',
    srcs = [
        'gtest_PPCodePointCheck.cpp',
    ],
    deps = [
        ':CodePointCheck',
        '//third_party/gtest:gtest_main',
    ],
)

cc_test(
    name = 'gtest_PPCodeUnit',
    srcs = [
        'gtest_PPCodeUnit.cpp',
    ],
    deps = [
        ':CodeUnit',
        '//third_party/gtest:gtest_main',
    ],
    linkstatic = 1,
)

cc_test(
    name = 'gtest_PPUTF32Stream',
    srcs = [
        'gtest_PPUTF32Stream.cpp',
    ],
    deps = [
        ':UTF32Stream',
        '//third_party/gtest:gtest_main',
    ],
    linkstatic = 1,
)

cc_test(
    name = 'gtest_PPCodeUnitStream',
    srcs = [
        'gtest_PPCodeUnitStream.cpp',
    ],
    deps = [
        ':CodeUnitStream',
        '//third_party/gtest:gtest_main',
    ],
    linkstatic = 1,
)

cc_test(
    name = 'gtest_PPTokenizerDFA',
    srcs = [
        'gtest_PPTokenizerDFA.cpp',
    ],
    deps = [
        ':TokenizerDFA',
        '//third_party/gtest:gtest_main',
    ],
    linkstatic = 1,
)
