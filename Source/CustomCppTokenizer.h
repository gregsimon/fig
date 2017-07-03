
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
      { "Error",              0xffcc0000 },
      { "Comment",            0xff75715e },
      { "Keyword",            0xffF92672 },
      { "Operator",           0xff225500 },
      { "Identifier",         0xffA6E22E },
      { "Integer",            0xffAE81FF },
      { "Float",              0xffAE81FF },
      { "String",             0xffE6DB74 },
      { "Bracket",            0xff550055 },
      { "Punctuation",        0xff004400 },
      { "Preprocessor Text",  0xff660000 }
    };

    CodeEditorComponent::ColourScheme cs;

    for (unsigned int i = 0; i < sizeof(types) / sizeof(types[0]); ++i)  // (NB: numElementsInArray doesn't work here in GCC4.2)
      cs.set(types[i].name, Colour(types[i].colour));

    return cs;
  }
};