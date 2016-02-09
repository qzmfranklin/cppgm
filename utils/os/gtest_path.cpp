#include "path.h"
#include <gtest/gtest.h>

TEST(path, basename) {
    const std::vector<std::string> w0 = {
        "/usr/local/bin/python3",
        "~/bin/zmake",
        "/Users/qzmfrank/.ssh",
        "foo.bar"
    };
    const std::vector<std::string> w1 = {
        "python3",
        "zmake",
        ".ssh",
        "foo.bar"
    };
    EXPECT_EQ(w0.size(), w1.size());
    const size_t num = w0.size();
    for (size_t i = 0; i < num; i++)
        EXPECT_EQ(os::path::basename(w0[i]), w1[i]);
}

TEST(path, dirname) {
    const std::vector<std::string> w0 = {
        "/usr/local/bin/python3",
        "~/bin/zmake",
        "/Users/qzmfrank/.ssh",
        "foo.bar"
    };
    const std::vector<std::string> w1 = {
        "/usr/local/bin",
        "~/bin",
        "/Users/qzmfrank",
        "."
    };
    EXPECT_EQ(w0.size(), w1.size());
    const size_t num = w0.size();
    for (size_t i = 0; i < num; i++)
        EXPECT_EQ(os::path::dirname(w0[i]), w1[i]);
}

TEST(path, join) {
    const std::vector<std::vector<std::string>> w0 = {
        { "/usr", "local", "bin", "python3" },
        { "~", "bin", "zmake" },
        { "/Users", "qzmfrank", ".ssh" },
        { "/Users////", "qzmfrank///", ".ssh" },
        { "foo.bar" }
    };
    const std::vector<std::string> w1 = {
        "/usr/local/bin/python3",
        "~/bin/zmake",
        "/Users/qzmfrank/.ssh",
        "/Users/////qzmfrank////.ssh",
        "foo.bar"
    };
    EXPECT_EQ(w0.size(), w1.size());
    const size_t num = w0.size();
    for (size_t i = 0; i < num; i++)
        EXPECT_EQ(os::path::join(w0[i]), w1[i]);
}

TEST(path, normpath) {
    const std::vector<std::string> w0 = {
        "/usr/local/.././X11/bin/Xephyr"
        "~/bin///zmake",
        "/Users/././/qzmfrank/.ssh",
        ".//foo.bar"
    };
    const std::vector<std::string> w1 = {
        "/usr/X11/bin/Xephyr"
        "~/bin/zmake",
        "/Users/qzmfrank/.ssh",
        "foo.bar"
    };
    EXPECT_EQ(w0.size(), w1.size());
    const size_t num = w0.size();
    for (size_t i = 0; i < num; i++)
        EXPECT_EQ(os::path::normpath(w0[i]), w1[i]);
}

TEST(path, realpath) {
    printf("This test will probably fail if you are not qzmfrank.\n");
    const std::vector<std::string> w0 = {
        "/usr/local/bin/python3",
        "/Users/././/qzmfrank/.ssh",
        "/Users/qzmfrank/git/rsdic/"
    };
    const std::vector<std::string> w1 = {
        "/usr/local/Cellar/python3/3.4.3_2/Frameworks/Python.framework/Versions/3.4/bin/python3.4",
        "/Users/qzmfrank/.ssh",
        "/Volumes/SourceCode/git/rsdic"
    };
    EXPECT_EQ(w0.size(), w1.size());
    const size_t num = w0.size();
    for (size_t i = 0; i < num; i++)
        EXPECT_EQ(os::path::realpath(w0[i]), w1[i]);
}

TEST(path, expanduser) {
    printf("This test will probably fail if you are not qzmfrank.\n");
    const std::vector<std::string> w0 = {
        "/usr/local/bin/python3",
        "~/bin/zmake",
        "~/git/rsdic/"
    };
    const std::vector<std::string> w1 = {
        "/usr/local/bin/python3",
        "/Users/qzmfrank/bin/zmake",
        "/Users/qzmfrank/git/rsdic/"
    };
    EXPECT_EQ(w0.size(), w1.size());
    const size_t num = w0.size();
    for (size_t i = 0; i < num; i++)
        EXPECT_EQ(os::path::expanduser(w0[i]), w1[i]);
}

TEST(path, exists) {
    printf("This test may fail if you are not on Darwin system.\n");
    const std::vector<std::string> w0 = {
        "/usr/bin/python3",
        "/bin/sh",
        "/etc/",
    };
    for (const auto w: w0)
        EXPECT_TRUE(os::path::exists(w));

    const std::vector<std::string> w1 = {
        "/usr/bin/python3333",
        "/bin/shhhhh",
        "/etccccc/",
    };
    for (const auto w: w1)
        EXPECT_FALSE(os::path::exists(w));
}

TEST(path, warning) {
    // You may have failed some unit tests here. Do not worry about it. Some of
    // the tests rely on Zhongming's own directories and files to work. They
    // served to initially test the issues. These functions, as exposed to you,
    // have a high probability of functioning.
}
