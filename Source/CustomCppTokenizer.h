
#pragma once

class CustomCppTokenizer : public CPlusPlusCodeTokeniser
{
public:
  CustomCppTokenizer() {
  }

  CodeEditorComponent::ColourScheme getDefaultColourScheme() override
  {
    struct Type
    {
      const char* name;
      uint32 colour;
    };

    const Type types[] =
    {
      { "Error",              0xffF8F8F0 },
      { "Comment",            0xff777777 },
      { "Keyword",            0xffee6f6f },
      { "Operator",           0xffcbcbcb },
      { "Identifier",         0xffdddddd },
      { "Integer",            0xff42c8c4 },
      { "Float",              0xff885500 },
      { "String",             0xffbc45dd },
      { "Bracket",            0xffc8c8c8 },
      { "Punctuation",        0xffcfbeff },
      { "Preprocessor Text",  0xffff6666 }
      
    };

    CodeEditorComponent::ColourScheme cs;

   for (unsigned int i = 0; i < sizeof(types) / sizeof(types[0]); ++i)  // (NB: numElementsInArray doesn't work here in GCC4.2)
      cs.set(types[i].name, Colour(types[i].colour));

    return cs;
  }
};