// test_vocab.cpp
//
// URL:      https://github.com/d99kris/spacy-cpp
//
// Copyright (C) 2017 Kristofer Berggren
// All rights reserved.
//
// spacy-cpp is distributed under the MIT license, see LICENSE for details.

#include <iostream>
#include <spacy/spacy_if.h>
#include <utility/string_builder.h>
#include "unittest.h"

int main()
{
  int rv = 0;

  try
  {
      void* data = parse("what is the W.H.O. to you?");
      freePointer(data);
  }
  catch(const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
