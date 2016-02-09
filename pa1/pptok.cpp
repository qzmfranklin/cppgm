#include "PPCodeUnitStream.h"
#include "PPTokenizerDFA.h"
#include "PPUTF32Stream.h"
#include "utils/os/path.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static int _pptokenize(const std::string &filestring)
{
  auto u32s = std::make_shared<PPUTF32Stream>(filestring);
  auto cus  = std::make_shared<PPCodeUnitStream>(u32s);
  auto dfa  = std::make_shared<PPTokenizerDFA>(cus);

  while (!dfa->isEmpty()) {
    if (!dfa->getErrorMessage().empty()) {
      fprintf(stderr,"ERROR: %s\n", dfa->getErrorMessage().c_str());
      return 1;
    }
    const std::shared_ptr<PPToken> tok = dfa->getPPToken();
    dfa->toNext();
    if (tok->getType() == PPTokenType::NewLine)
      printf("new-line\n");
    else if (tok->getType() == PPTokenType::WhitespaceSequence)
      continue;
    else
      printf("%s %zu %s\n", PPToken::getTokenTypeUTF8String(tok->getType()).c_str(),
          tok->getRawText().length(), tok->getRawText().c_str());
  }

  printf("eof\n");

  return 0;
}

int main(int argc, char const* argv[])
{
  FILE *fin = argc == 1 ? stdin: fopen(argv[1], "r");

  // Output to stderr to not polute stdout
  if (argc > 2)
    fprintf(stderr, "Only the first argument is meaningful. Other arguments are ignored.\n");

  if (!fin) {
    fprintf(stderr, "The file does not exist: %s\n", argv[1]);
    exit(1);
  }

  std::string filestring;

  {
    size_t bufsize = BUFSIZ;
    char *buf = (char*) malloc(bufsize);
    ssize_t bytesread;
    while ( (bytesread = getline(&buf, &bufsize, fin)) != -1 ) {
      buf[bytesread] = 0;
      filestring += buf;
    }
    if (fin != stdin)
      fclose(fin);
  }

  return _pptokenize(filestring);
}
