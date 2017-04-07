#!/bin/bash

# Modify the variable 'src_url' to update the release of icu in use.

src_url=http://download.icu-project.org/files/icu4c/58.2/icu4c-58_2-src.zip
src_zip=icu4c-src.zip

rm -rf icu/

wget "${src_url}" --output-document "${src_zip}"
unzip "${src_zip}" && rm "${src_zip}"
