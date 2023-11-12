// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module util.unicode;

import std.core;
import util.binary.stream.writer;
import util.binary.stream.reader;
import util.stream;
import util.text.util;

export namespace util {

std::string SpringPPVersionStr();
std::string SpringPPRoot();
std::string SpringPPUcdFilePath();

void SetEx();

class UnicodeException : public std::runtime_error
{
public:
    UnicodeException(const std::string& message_);
};

void ThrowInvalidUtf8Sequence();
void ThrowUnicodeException(const std::string& message_);

std::u32string ToUtf32(const std::string& utf8Str);
std::u32string ToUtf32(const std::u16string& utf16Str);
std::u32string ToUtf32(const std::u32string& utf32Str) { return utf32Str; }
std::u16string ToUtf16(const std::u32string& utf32Str);
std::u16string ToUtf16(const std::u16string& utf16Str) { return utf16Str; }
std::u16string ToUtf16(const std::string& u8str);
std::string ToUtf8(const std::u32string& utf32Str);
std::string ToUtf8(const std::u16string& utf16Str);
std::string ToUtf8(const std::string& utf8Str) { return utf8Str; }

class Utf8ToUtf32Engine
{
public:
    Utf8ToUtf32Engine();
    void Put(uint8_t x);
    bool ResultReady() const { return resultReady; }
    char32_t Result() const { return result; }
private:
    int state;
    bool resultReady;
    char32_t result;
    uint8_t bytes[4];
};

std::u32string ToUpper(const std::u32string& s);
std::u16string ToUpper(const std::u16string& s);
std::string ToUpper(const std::string& s);
std::u32string ToLower(const std::u32string& s);
std::u16string ToLower(const std::u16string& s);
std::string ToLower(const std::string& s);
std::string MakeCanonicalPropertyName(const std::string& s);

enum class BinaryPropertyId : uint8_t
{
    asciiHexDigit,
    alphabetic,
    bidiControl,
    bidiMirrored,
    cased,
    compositionExclusion,
    caseIgnorable,
    fullCompositionExclusion,
    changesWhenCasefolded,
    changesWhenCaseMapped,
    changesWhenNFKCCasefolded,
    changesWhenLowercased,
    changesWhenTitlecased,
    changesWhenUppercased,
    dash,
    deprecated,
    defaultIgnorableCodePoint,
    diacritic,
    extender,
    graphemeBase,
    graphemeExtend,
    graphemeLink,
    hexDigit,
    hyphen,
    idContinue,
    ideographic,
    idStart,
    idsBinaryOperator,
    idsTrinaryOperator,
    joinControl,
    logicalOrderException,
    lowercase,
    math,
    noncharacterCodePoint,
    otherAlphabetic,
    otherDefaultIgnorableCodePoint,
    otherGraphemeExtend,
    otherIdContinue,
    otherIdStart,
    otherLowercase,
    otherMath,
    otherUppercase,
    patternSyntax,
    patternWhiteSpace,
    prependedConcatenationMark,
    quotationMark,
    radical,
    softDotted,
    sentenceterminal,
    terminalPunctuation,
    unifiedIdeograph,
    uppercase,
    variationSelector,
    whiteSpace,
    xidContinue,
    xidStart,
    expandsOnNFC,
    expandsOnNFD,
    expandsOnNFKC,
    expandsOnNFKD
};

class BinaryProperty
{
public:
    BinaryProperty(BinaryPropertyId id_, const std::string& shortName_, const std::string& longName_);
    BinaryPropertyId Id() const { return id; }
    const std::string& ShortName() const { return shortName; }
    const std::string& LongName() const { return longName; }
private:
    BinaryPropertyId id;
    std::string shortName;
    std::string longName;
};

class BinaryPropertyTable
{
public:
    static BinaryPropertyTable& Instance();
    const BinaryProperty& GetBinaryProperty(BinaryPropertyId binaryPropertyId) const;
    bool IsBinaryProperty(const std::string& shortName) const;
    const BinaryProperty& GetBinaryPropertyByShortName(const std::string& shortName) const;
    const BinaryProperty& GetBinaryPropertyByLongName(const std::string& longName) const;
    const std::vector<BinaryProperty>& BinaryProperties() const { return binaryProperties; }
private:
    BinaryPropertyTable();
    std::map<BinaryPropertyId, const BinaryProperty*> binaryPropertyIdMap;
    std::map<std::string, const BinaryProperty*> shortNameMap;
    std::map<std::string, const BinaryProperty*> longNameMap;
    std::vector<BinaryProperty> binaryProperties;
};

inline const BinaryProperty& GetBinaryProperty(BinaryPropertyId id)
{
    return BinaryPropertyTable::Instance().GetBinaryProperty(id);
}

inline const BinaryProperty& GetBinaryPropertyByShortName(const std::string& shortName)
{
    return BinaryPropertyTable::Instance().GetBinaryPropertyByShortName(shortName);
}

inline const BinaryProperty& GetBinaryPropertyByLongName(const std::string& longName)
{
    return BinaryPropertyTable::Instance().GetBinaryPropertyByLongName(longName);
}

enum class BlockId : uint16_t
{
    none = 0,
    adlam, aegeanNumbers, ahom, alchemical, alphabeticPf, anatolianHieroglyphs, ancientGreekMusic, ancientGreekNumbers, ancientSymbols,
    arabic, arabicExtA, arabicMath, arabicPfA, arabicPfB, arabicSup, armenian, arrows, ascii, avestan,
    balinese, bamum, bamumSup, bassaVah, batak, bengali, bhaisuki, blockElements, bopomofo, bopomofoExt, boxDrawing,
    brahmi, braille, buginese, buhid, byzantineMusic,
    carian, caucasianAlbanian, chakma, cham, cherokee, cherokeeSup, chessSymbols,
    cjk, cjkCompat, cjkCompatForms, cjkCompatIdeographs, cjkCompatIdeographsSup, cjkExtA, cjkExtB, cjkExtC, cjkExtD, cjkExtE, cjkExtF,
    cjkRadicalsSup, cjkStrokes, cjkSymbols, compatJamo, controlPictures, coptic, copticEpactNumbers, countingRod, cuneiform,
    cuneiformNumbers, currencySymbols, cypriotSyllabary, cyrillic, cyrillicExtA, cyrillicExtB, cyrillicExtC, cyrillicSup,
    deseret, devanagari, devanagariExt, diacriticals, diariticalsForSymbols, diacriticalsSup, diacriticalsExt, dingbats, dogra, domino, duployan,
    earlyDynasticCuneiform, egyptianHieroglyphs, egyptianHieroglyphFormatControls, elbasan, elymaic, emoticons, enclosedAlphanum, enclosedAlphanumSup, enclosedCjk, enclosedIdeographicSup,
    ethiopic, ethiopicExt, ethiopicExtA, ethiopicSup,
    geometricShapes, geometricShapesExt, georgian, georgianExt, georgianSup, glagolitic, glagoliticSup, gothic, grantha, greek, greekExt, gujarati, gunjalaGondi, gurmukhi,
    halfAndFullForms, halfMarks, hangul, hanifiRohingya, hanunoo, hatran, hebrew, highPuSurrogates, highSurrogates, hiragana,
    idc, ideographicSymbols, imperialAramaic, indicNumberForms, inscriptionalPahlavi, inscriptionalParthian, ipaExt, indicSiyaqNumbers,
    jamo, jamoExtA, jamoExtB, javanese, kaithi, kanaExtA, smallKanaExt, kanaSup, kanbun, kangxi, kannada, katakana, katakanaExt, kayahLi, kharoshthi, khmer, khmerSymbols, khojki, khudawadi,
    lao, latin1Sup, latinExtA, latinExtAdditional, latinExtB, latinExtC, latinExtD, latinExtE, lepcha, letterlikeSymbols, limbu,
    linearA, linearBIdeograms, linearBSyllabary, lisu, lowSurrogates, lycian, lydian,
    mahajani, mahjong, makasar, malayalam, mandaic, manichean, marchen, masaramGondi, mathAlphanum, mathOperators, mayanNumerals, medefaidrin, meeteiMayek, meeteiMayekExt, mendeKikakui, meroiticCursive, meroiticHieroglyphs,
    miao, miscArrows, miscMathSymbolsA, miscMathSymbolsB, miscPictographs, miscSymbols, miscTechnical, modi, modifierLetters, modifierToneLetters, mongolian, mongolianSup,
    mro, music, multani, myanmar, myanmarExtA, myanmarExtB,
    nabataean, nb, nandinagari, newTaiLue, newa, nko, numberForms, nushu, nyiakengPuachueHmong,
    ocr, ogham, olChiki, oldHungarian, oldItalic, oldNorthArabian, oldPermic, oldPersian, oldSogdian, oldSouthArabian, oldTurkic, oriya, ornamentalDingbats, osage, osmanya, ottomanSiyaqNumbers,
    pahawhHmong, palmyrene, pauCinHau, phagsPa, phaistos, phoenician, phoneticExt, phoneticExtSup, playingCards, psalterPahlavi, pua, punctuation,
    rejang, rumi, runic,
    samaritan, saurashtra, sharada, shavian, shorthandFormatControls, siddham, sinhala, sinhalaArchaicNumbers, smallForms, sogdian, soraSompeng, soyombo, specials, sundanese, sundaneseSup,
    supArrowsA, supArrowsB, supArrowsC, supMathOperators, supPuaA, supPuaB, supPunctuation, supSymbolsAndPictographs, superAndSub, suttonSignWriting, sylotiNagri, symbolsAndPictographsExtA, syriac,
    syriacSup,
    tagalog, tagbanwa, tags, taiLe, taiTham, taiViet, taiXuanJing, takri, tamil, tamilSup, tangut, tangutComponents, telugu, thaana, thai, tibetan, tifinagh, tirhuta, transportAndMap,
    ucas, ucasExt, ugaritic, vai, vedicExt, verticalForms, vs, vsSup, wancho, warangCiti, yiRadicals, yiSyllables, yijing, zanabazarSquare
};

class Block
{
public:
    Block(BlockId id_, const std::string& shortName_, const std::string& longName_, char32_t start, char32_t end_);
    BlockId Id() const { return id; }
    const std::string& ShortName() const { return shortName; }
    const std::string& LongName() const { return longName; }
    char32_t Start() const { return start; }
    char32_t End() const { return end; }
private:
    BlockId id;
    std::string shortName;
    std::string longName;
    char32_t start;
    char32_t end;
};

class BlockTable
{
public:
    static BlockTable& Instance();
    const Block& GetBlock(BlockId blockId) const;
    const Block& GetBlockByShortName(const std::string& shortName) const;
    const Block& GetBlockByLongName(const std::string& longName) const;
    const std::vector<Block>& Blocks() const { return blocks; }
private:
    BlockTable();
    std::map<BlockId, const Block*> blockIdMap;
    std::map<std::string, const Block*> shortNameMap;
    std::map<std::string, const Block*> longNameMap;
    std::vector<Block> blocks;
};

inline const Block& GetBlock(BlockId blockId)
{
    return BlockTable::Instance().GetBlock(blockId);
}

inline const Block& GetBlockByShortName(const std::string& shortName)
{
    return BlockTable::Instance().GetBlockByShortName(shortName);
}

inline const Block& GetBlockByLongName(const std::string& longName)
{
    return BlockTable::Instance().GetBlockByLongName(longName);
}

enum class GeneralCategoryId : int32_t
{
    none = 0,
    Lu = 1 << 0, Ll = 1 << 1, Lt = 1 << 2, Lm = 1 << 3, Lo = 1 << 4,
    LC = Lu | Ll | Lt,
    L = Lu | Ll | Lt | Lm | Lo,
    Mn = 1 << 5, Mc = 1 << 6, Me = 1 << 7,
    M = Mn | Mc | Me,
    Nd = 1 << 8, Nl = 1 << 9, No = 1 << 10,
    N = Nd | Nl | No,
    Pc = 1 << 11, Pd = 1 << 12, Ps = 1 << 13, Pe = 1 << 14, Pi = 1 << 15, Pf = 1 << 16, Po = 1 << 17,
    P = Pc | Pd | Ps | Pe | Pi | Pf | Po,
    Sm = 1 << 18, Sc = 1 << 19, Sk = 1 << 20, So = 1 << 21,
    S = Sm | Sc | Sk | So,
    Zs = 1 << 22, Zl = 1 << 23, Zp = 1 << 24,
    Z = Zs | Zl | Zp,
    Cc = 1 << 25, Cf = 1 << 26, Cs = 1 << 27, Co = 1 << 28, Cn = 1 << 29,
    C = Cc | Cf | Cs | Co | Cn,
    G = L | M | N | P | S | Zs,
    B = L | N | P | S | Zs
};

inline GeneralCategoryId operator&(GeneralCategoryId left, GeneralCategoryId right)
{
    return GeneralCategoryId(int32_t(left) & int32_t(right));
}

inline GeneralCategoryId operator|(GeneralCategoryId left, GeneralCategoryId right)
{
    return GeneralCategoryId(int32_t(left) | int32_t(right));
}

class GeneralCategory
{
public:
    GeneralCategory(GeneralCategoryId id_, const std::string& shortName_, const std::string& longName_);
    GeneralCategoryId Id() const { return id; }
    const std::string& ShortName() const { return shortName; }
    const std::string& LongName() const { return longName; }
private:
    GeneralCategoryId id;
    std::string shortName;
    std::string longName;
};

class GeneralCategoryTable
{
public:
    static GeneralCategoryTable& Instance();
    const GeneralCategory& GetGeneralCategory(GeneralCategoryId generalCategoryId) const;
    const GeneralCategory& GetGeneralCategoryByShortName(const std::string& shortName) const;
    const GeneralCategory& GetGeneralCategoryByLongName(const std::string& shortName) const;
    const std::vector<GeneralCategory>& GeneralCategories() const { return generalCategories; }
private:
    GeneralCategoryTable();
    std::map<GeneralCategoryId, const GeneralCategory*> generalCategoryIdMap;
    std::map<std::string, const GeneralCategory*> shortNameMap;
    std::map<std::string, const GeneralCategory*> longNameMap;
    std::vector<GeneralCategory> generalCategories;
};

inline const GeneralCategory& GetGeneralCategory(GeneralCategoryId id)
{
    return GeneralCategoryTable::Instance().GetGeneralCategory(id);
}

inline const GeneralCategory& GetGeneralCategoryByShortName(const std::string& shortName)
{
    return GeneralCategoryTable::Instance().GetGeneralCategoryByShortName(shortName);
}

inline const GeneralCategory& GetGeneralCategoryByLongName(const std::string& longName)
{
    return GeneralCategoryTable::Instance().GetGeneralCategoryByLongName(longName);
}

enum class AgeId : uint8_t
{
    age_unassigned = 0,
    age_1_1,
    age_2_0, age_2_1,
    age_3_0, age_3_1, age_3_2,
    age_4_0, age_4_1,
    age_5_0, age_5_1, age_5_2,
    age_6_0, age_6_1, age_6_2, age_6_3,
    age_7_0,
    age_8_0,
    age_9_0,
    age_10_0,
    age_11_0,
    age_12_0, age_12_1
};

class Age
{
public:
    Age(AgeId id_, const std::string& version_);
    AgeId Id() const { return id; }
    const std::string& Version() const { return version; }
private:
    AgeId id;
    std::string version;
};

class AgeTable
{
public:
    static AgeTable& Instance();
    const Age& GetAge(AgeId id) const;
    const Age& GetAge(const std::string& version) const;
    const std::vector<Age>& Ages() const { return ages; }
private:
    AgeTable();
    std::map<AgeId, const Age*> ageIdMap;
    std::map<std::string, const Age*> versionMap;
    std::vector<Age> ages;
};

inline const Age& GetAge(AgeId id)
{
    return AgeTable::Instance().GetAge(id);
}

inline const Age& GetAge(const std::string& version)
{
    return AgeTable::Instance().GetAge(version);
}

enum class ScriptId : uint8_t
{
    none = 0,
    adlm, aghb, ahom, arab, armi, armn, avst,
    bali, bamu, bass, batk, beng, bhks, bopo, brah, brai, bugi, buhd,
    cakm, cans, cari, cham, cher, copt, cprt, cyrl,
    deva, dogr, dsrt, dupl,
    elba, elym, egyp, ethi,
    geor, glag, gong, gonm, goth, gran, grek, gujr, guru,
    hang, hani, hano, hatr, hebr, hira, hluw, hmng, hmnp, hrkt, hung,
    ital,
    java,
    kali, kana, khar, khmr, khoj, knda, kthi,
    lana, laoo, latn, lepc, limb, lina, linb, lisu, lyci, lydi,
    mahj, maka, mand, mani, marc, medf, mend, merc, mero, mlym, modi, mong, mroo, mtei, mult, mymr,
    nand, narb, nbat, newa, nkoo, nshu,
    ogam, olck, orkh, orya, osge, osma,
    palm, pauc, perm, phag, phli, phlp, phnx, plrd, prti,
    qaai,
    rjng, rohg, runr,
    samr, sarb, saur, sgnw, shaw, shrd, sidd, sind, sinh, sogd, sogo, sora, soyo, sund, sylo, syrc,
    tagb, takr, tale, talu, taml, tang, tavt, telu, tfng, tglg, thaa, thai, tibt, tirh,
    ugar,
    vaii,
    wara,
    wcho,
    xpeo, xsux,
    yiii, zanb,
    zinh, zyyy, zzzz
};

class Script
{
public:
    Script(ScriptId id_, const std::string& shortName_, const std::string& longName_);
    ScriptId Id() const { return id; }
    const std::string& ShortName() const { return shortName; }
    const std::string& LongName() const { return longName; }
private:
    ScriptId id;
    std::string shortName;
    std::string longName;
};

class ScriptTable
{
public:
    static ScriptTable& Instance();
    const Script& GetScript(ScriptId id) const;
    const Script& GetScriptByShortName(const std::string& shortName) const;
    const Script& GetScriptByLongName(const std::string& longName) const;
    const std::vector<Script>& Scripts() const { return scripts; }
private:
    ScriptTable();
    std::vector<Script> scripts;
    std::map<ScriptId, const Script*> scriptIdMap;
    std::map<std::string, const Script*> shortNameMap;
    std::map<std::string, const Script*> longNameMap;
};

inline const Script& GetScript(ScriptId id)
{
    return ScriptTable::Instance().GetScript(id);
}

inline const Script& GetScriptByShortName(const std::string& shortName)
{
    return ScriptTable::Instance().GetScriptByShortName(shortName);
}

inline const Script& GetScriptByLongName(const std::string& longName)
{
    return ScriptTable::Instance().GetScriptByLongName(longName);
}

class CharacterInfo
{
public:
    CharacterInfo();
    bool GetBinaryProperty(BinaryPropertyId binaryPropertyId) const
    {
        uint64_t mask = static_cast<uint64_t>(1) << static_cast<uint64_t>(static_cast<uint8_t>(binaryPropertyId));
        return (binaryProperties & mask) != 0;
    }
    void SetBinaryPropery(BinaryPropertyId binaryPropertyId, bool value)
    {
        uint64_t bit = static_cast<uint64_t>(1) << static_cast<uint64_t>(static_cast<uint8_t>(binaryPropertyId));
        if (value)
        {
            binaryProperties = binaryProperties | bit;
        }
        else
        {
            binaryProperties = binaryProperties & ~bit;
        }
    }
    GeneralCategoryId GetGeneralCategory() const
    {
        return generalCategory;
    }
    bool HasGeneralCategory(GeneralCategoryId generalCategory_) const
    {
        return (generalCategory & generalCategory_) != GeneralCategoryId::none;
    }
    void SetGeneralCategory(GeneralCategoryId generalCategory_)
    {
        generalCategory = generalCategory_;
    }
    char32_t Upper() const
    {
        return upper;
    }
    void SetUpper(char32_t upper_)
    {
        upper = upper_;
    }
    char32_t Lower() const
    {
        return lower;
    }
    void SetLower(char32_t lower_)
    {
        lower = lower_;
    }
    char32_t Title() const
    {
        return title;
    }
    void SetTitle(char32_t title_)
    {
        title = title_;
    }
    char32_t Folding() const
    {
        return folding;
    }
    void SetFolding(char32_t folding_)
    {
        folding = folding_;
    }
    BlockId GetBlock() const
    {
        return block;
    }
    void SetBlock(BlockId block_)
    {
        block = block_;
    }
    AgeId GetAge() const
    {
        return age;
    }
    void SetAge(AgeId age_)
    {
        age = age_;
    }
    ScriptId GetScript() const
    {
        return script;
    }
    void SetScript(ScriptId script_)
    {
        script = script_;
    }
    void Write(BinaryStreamWriter& writer);
    void Read(BinaryStreamReader& reader);
private:
    uint64_t binaryProperties;
    GeneralCategoryId generalCategory;
    char32_t upper;
    char32_t lower;
    char32_t title;
    char32_t folding;
    BlockId block;
    AgeId age;
    ScriptId script;
};

constexpr int numInfosInPage = 128;
constexpr size_t characterInfoSize = sizeof(uint64_t) + sizeof(BlockId) + sizeof(GeneralCategoryId) + sizeof(AgeId) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) +
sizeof(uint32_t) + sizeof(ScriptId);
constexpr size_t characterInfoPageSize = numInfosInPage * characterInfoSize;

enum class NumericTypeId : uint8_t
{
    none = 0,
    de, di, nu
};

class NumericType
{
public:
    NumericType(NumericTypeId id_, const std::string& shortName_, const std::string& longName_);
    NumericTypeId Id() const { return id; }
    const std::string& ShortName() const { return shortName; }
    const std::string& LongName() const { return longName; }
private:
    NumericTypeId id;
    std::string shortName;
    std::string longName;
};

class NumericTypeTable
{
public:
    static NumericTypeTable& Instance();
    const NumericType& GetNumericType(NumericTypeId id) const;
    const NumericType& GetNumericTypeByShortName(const std::string& shortName) const;
    const NumericType& GetNumericTypeByLongName(const std::string& longName) const;
    const std::vector<NumericType>& NumericTypes() const { return numericTypes; }
private:
    NumericTypeTable();
    std::vector<NumericType> numericTypes;
    std::map<NumericTypeId, const NumericType*> numericTypeMap;
    std::map<std::string, const NumericType*> shortNameMap;
    std::map<std::string, const NumericType*> longNameMap;
};

inline const NumericType& GetNumericType(NumericTypeId id)
{
    return NumericTypeTable::Instance().GetNumericType(id);
}

inline const NumericType& GetNumericTypeByShortName(const std::string& shortName)
{
    return NumericTypeTable::Instance().GetNumericTypeByShortName(shortName);
}

inline const NumericType& GetNumericTypeByLongName(const std::string& longName)
{
    return NumericTypeTable::Instance().GetNumericTypeByLongName(longName);
}

enum class BidiClassId : uint8_t
{
    none = 0,
    al, an, b, bn, cs, en, es, et, fsi, l, lre, lri, lro, nsm, on, pdf, pdi, r, rle, rli, rlo, s, ws
};

class BidiClass
{
public:
    BidiClass(BidiClassId id_, const std::string& shortName_, const std::string& longName_);
    BidiClassId Id() const { return id; }
    const std::string& ShortName() const { return shortName; }
    const std::string& LongName() const { return longName; }
private:
    BidiClassId id;
    std::string shortName;
    std::string longName;
};

class BidiClassTable
{
public:
    static BidiClassTable& Instance();
    const BidiClass& GetBidiClass(BidiClassId id) const;
    const BidiClass& GetBidiClassByShortName(const std::string& shortName) const;
    const BidiClass& GetBidiClassByLongName(const std::string& longName) const;
    const std::vector<BidiClass>& BidiClasses() const { return bidiClasses; }
private:
    BidiClassTable();
    std::vector<BidiClass> bidiClasses;
    std::map<BidiClassId, const BidiClass*> bidiClassMap;
    std::map<std::string, const BidiClass*> shortNameMap;
    std::map<std::string, const BidiClass*> longNameMap;
};

inline const BidiClass& GetBidiClass(BidiClassId id)
{
    return BidiClassTable::Instance().GetBidiClass(id);
}

inline const BidiClass& GetBidiClassByShortName(const std::string& shortName)
{
    return BidiClassTable::Instance().GetBidiClassByShortName(shortName);
}

inline const BidiClass& GetBidiClassByLongName(const std::string& longtName)
{
    return BidiClassTable::Instance().GetBidiClassByLongName(longtName);
}

enum class BidiPairedBracketTypeId : uint8_t
{
    none = 0,
    o, c
};

class BidiPairedBracketType
{
public:
    BidiPairedBracketType(BidiPairedBracketTypeId id_, const std::string& shortName_, const std::string& longName_);
    BidiPairedBracketTypeId Id() const { return id; }
    const std::string& ShortName() const { return shortName; }
    const std::string& LongName() const { return longName; }
private:
    BidiPairedBracketTypeId id;
    std::string shortName;
    std::string longName;
};

class BidiPairedBracketTypeTable
{
public:
    static BidiPairedBracketTypeTable& Instance();
    const BidiPairedBracketType& GetBidiPairedBracketType(BidiPairedBracketTypeId id) const;
    const BidiPairedBracketType& GetBidiPairedBracketTypeByShortName(const std::string& shortName) const;
    const BidiPairedBracketType& GetBidiPairedBracketTypeByLongName(const std::string& longName) const;
    const std::vector<BidiPairedBracketType>& BidiPairedBracketTypes() const { return bidiPairedBracketTypes; }
private:
    BidiPairedBracketTypeTable();
    std::vector<BidiPairedBracketType> bidiPairedBracketTypes;
    std::map<BidiPairedBracketTypeId, const BidiPairedBracketType*> typeMap;
    std::map<std::string, const BidiPairedBracketType*> shortNameMap;
    std::map<std::string, const BidiPairedBracketType*> longNameMap;
};

inline const BidiPairedBracketType& GetBidiPairedBracketType(BidiPairedBracketTypeId id)
{
    return BidiPairedBracketTypeTable::Instance().GetBidiPairedBracketType(id);
}

inline const BidiPairedBracketType& GetBidiPairedBracketTypeByShortName(const std::string& shortName)
{
    return BidiPairedBracketTypeTable::Instance().GetBidiPairedBracketTypeByShortName(shortName);
}

inline const BidiPairedBracketType& GetBidiPairedBracketTypeByLongName(const std::string& longName)
{
    return BidiPairedBracketTypeTable::Instance().GetBidiPairedBracketTypeByLongName(longName);
}

enum class AliasTypeId : uint8_t
{
    none = 0,
    correction, control, alternate, figment, abbreviation
};

class AliasType
{
public:
    AliasType(AliasTypeId id_, const std::string& name_);
    AliasTypeId Id() const { return id; }
    const std::string& Name() const { return name; }
private:
    AliasTypeId id;
    std::string name;
};

class AliasTypeTable
{
public:
    static AliasTypeTable& Instance();
    const AliasType& GetAliasType(AliasTypeId id) const;
    const AliasType& GetAliasType(const std::string& typeName) const;
    const std::vector<AliasType>& AliasTypes() const { return aliasTypes; }
private:
    AliasTypeTable();
    std::vector<AliasType> aliasTypes;
    std::map<AliasTypeId, const AliasType*> aliasTypeMap;
    std::map<std::string, const AliasType*> typeNameMap;
};

inline const AliasType& GetAliasType(AliasTypeId id)
{
    return AliasTypeTable::Instance().GetAliasType(id);
}

inline const AliasType& GetAliasType(const std::string& typeName)
{
    return AliasTypeTable::Instance().GetAliasType(typeName);
}

class Alias
{
public:
    Alias();
    Alias(AliasTypeId typeId_, const std::string& name_);
    AliasTypeId TypeId() const { return typeId; }
    const std::string& Name() const { return name; }
    void Write(BinaryStreamWriter& writer);
    void Read(BinaryStreamReader& reader);
private:
    AliasTypeId typeId;
    std::string name;
};

class ExtendedCharacterInfo
{
public:
    ExtendedCharacterInfo();
    const std::string& CharacterName() const
    {
        return characterName;
    }
    void SetCharacterName(const std::string& characterName_);
    const std::string& Unicode1Name() const
    {
        return unicode1Name;
    }
    void SetUnicode1Name(const std::string& unicode1Name_);
    uint8_t GetCanonicalCombiningClass() const
    {
        return canonicalCombiningClass;
    }
    void SetCanonicalCombiningClass(uint8_t canonicalCombiningClass_)
    {
        canonicalCombiningClass = canonicalCombiningClass_;
    }
    const std::u32string& FullUpper() const
    {
        return fullUpper;
    }
    std::u32string& FullUpper()
    {
        return fullUpper;
    }
    const std::u32string& FullLower() const
    {
        return fullLower;
    }
    std::u32string& FullLower()
    {
        return fullLower;
    }
    const std::u32string& FullTitle() const
    {
        return fullTitle;
    }
    std::u32string& FullTitle()
    {
        return fullTitle;
    }
    const std::u32string& FullFolding() const
    {
        return fullFolding;
    }
    std::u32string& FullFolding()
    {
        return fullFolding;
    }
    BidiClassId GetBidiClass() const
    {
        return bidiClass;
    }
    void SetBidiClass(BidiClassId bidiClass_)
    {
        bidiClass = bidiClass_;
    }
    NumericTypeId GetNumericType() const
    {
        return numericType;
    }
    void SetNumericType(NumericTypeId numericType_)
    {
        numericType = numericType_;
    }
    const std::string& GetNumericValue() const
    {
        return numericValue;
    }
    void SetNumericValue(const std::string& numericValue_)
    {
        numericValue = numericValue_;
    }
    const std::vector<Alias>& Aliases() const
    {
        return aliases;
    }
    std::vector<Alias>& Aliases()
    {
        return aliases;
    }
    char32_t GetBidiMirroringGlyph() const
    {
        return bidiMirroringGlyph;
    }
    void SetBidiMirroringGlyph(char32_t bidiMirroringGlyph_)
    {
        bidiMirroringGlyph = bidiMirroringGlyph_;
    }
    BidiPairedBracketTypeId GetBidiPairedBracketType() const
    {
        return bidiPairedBracketType;
    }
    void SetBidiPairedBracketType(BidiPairedBracketTypeId bidiPairedBracketType_)
    {
        bidiPairedBracketType = bidiPairedBracketType_;
    }
    char32_t GetBidiPairedBracket() const
    {
        return bidiPairedBracket;
    }
    void SetBidiPairedBracket(char32_t bidiPairedBracket_)
    {
        bidiPairedBracket = bidiPairedBracket_;
    }
    void Write(BinaryStreamWriter& writer);
    void Read(BinaryStreamReader& reader);
private:
    std::string characterName;
    std::string unicode1Name;
    uint8_t canonicalCombiningClass;
    std::u32string fullUpper;
    std::u32string fullLower;
    std::u32string fullTitle;
    std::u32string fullFolding;
    BidiClassId bidiClass;
    NumericTypeId numericType;
    std::string numericValue;
    std::vector<Alias> aliases;
    char32_t bidiMirroringGlyph;
    BidiPairedBracketTypeId bidiPairedBracketType;
    char32_t bidiPairedBracket;
};

class CharacterInfoPage
{
public:
    CharacterInfoPage();
    CharacterInfoPage(const CharacterInfoPage&) = delete;
    CharacterInfoPage& operator=(const CharacterInfoPage&) = delete;
    const CharacterInfo& GetCharacterInfo(int index) const;
    CharacterInfo& GetCharacterInfo(int index);
    void Write(BinaryStreamWriter& writer);
    void Read(BinaryStreamReader& reader);
private:
    std::vector<CharacterInfo> characterInfos;
};

class ExtendedCharacterInfoPage
{
public:
    ExtendedCharacterInfoPage();
    ExtendedCharacterInfoPage(const ExtendedCharacterInfoPage&) = delete;
    ExtendedCharacterInfoPage& operator=(const ExtendedCharacterInfoPage&) = delete;
    const ExtendedCharacterInfo& GetExtendedCharacterInfo(int index) const;
    ExtendedCharacterInfo& GetExtendedCharacterInfo(int index);
    void Write(BinaryStreamWriter& writer);
    void Read(BinaryStreamReader& reader);
private:
    std::vector<ExtendedCharacterInfo> extendedCharacterInfos;
};

class ExtendedCharacterInfoHeader
{
public:
    void AllocatePages(int numExtendedPages);
    void Write(BinaryStreamWriter& writer);
    void Read(BinaryStreamReader& reader);
    uint32_t GetPageStart(int pageIndex) const;
    void SetPageStart(int pageIndex, uint32_t extendedPageStart);
private:
    std::vector<uint32_t> extendedPageStarts;
};

const int soul_ucd_version_1 = 1;
const int soul_ucd_version_2 = 2;
const int soul_ucd_version_3 = 3;
const int soul_ucd_version_4 = 4;
const int current_soul_ucd_version = soul_ucd_version_4;

enum class CharacterTableDataSource
{
    file, memory
};

class CharacterTable
{
public:
    CharacterTable(const CharacterTable&) = delete;
    CharacterTable& operator=(const CharacterTable&) = delete;
    static CharacterTable& Instance();
    std::string FilePath() const;
    std::string DeflateFilePath() const;
    int64_t GetUncompressedFileSize() const;
    const CharacterInfo& GetCharacterInfo(char32_t codePoint);
    CharacterInfo& CreateCharacterInfo(char32_t codePoint);
    const ExtendedCharacterInfo& GetExtendedCharacterInfo(char32_t codePoint);
    ExtendedCharacterInfo& CreateExtendedCharacterInfo(char32_t codePoint);
    void Write();
#ifdef _WIN32	
    void WriteDeflate();
#endif	
    void SetData(uint8_t* data_, int64_t size_);
#ifdef _WIN32
    void SetDeflateData(uint8_t* deflateData, int64_t deflateSize, int64_t uncompressedSize);
#endif	
private:
    CharacterTable();
    Streams GetReadStreams();
    void WriteHeader(BinaryStreamWriter& writer);
    void ReadHeader(BinaryStreamReader& reader);
    void ReadExtendedHeader(BinaryStreamReader& reader);
    CharacterTableDataSource dataSource;
    uint8_t* data;
    int64_t size;
    std::vector<uint8_t> memory;
    bool headerRead;
    std::vector<std::unique_ptr<CharacterInfoPage>> pages;
    uint32_t extendedHeaderStart;
    uint32_t extendedHeaderEnd;
    bool extendedHeaderRead;
    ExtendedCharacterInfoHeader extendedHeader;
    std::vector<std::unique_ptr<ExtendedCharacterInfoPage>> extendedPages;
    size_t headerSize;
};

inline const CharacterInfo& GetCharacterInfo(char32_t codePoint) { return CharacterTable::Instance().GetCharacterInfo(codePoint); }
inline CharacterInfo& CreateCharacterInfo(char32_t codePoint) { return CharacterTable::Instance().CreateCharacterInfo(codePoint); }
inline const ExtendedCharacterInfo& GetExtendedCharacterInfo(char32_t codePoint) { return CharacterTable::Instance().GetExtendedCharacterInfo(codePoint); }
inline ExtendedCharacterInfo& CreateExtendedCharacterInfo(char32_t codePoint) { return CharacterTable::Instance().CreateExtendedCharacterInfo(codePoint); }

inline GeneralCategoryId GetGeneralCategory(char32_t c) { return GetCharacterInfo(c).GetGeneralCategory(); }
inline bool HasGeneralCategory(char32_t c, GeneralCategoryId generalCategory) { return GetCharacterInfo(c).HasGeneralCategory(generalCategory); }

inline bool IsUpperLetter(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Lu;
}

inline bool IsLowerLetter(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Ll;
}

inline bool IsTitleLetter(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Lt;
}

inline bool IsModifierLetter(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Lm;
}

inline bool IsOtherLetter(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Lo;
}

inline bool IsCasedLetter(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::LC);
}

inline bool IsLetter(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::L);
}

inline bool IsNonspacingMark(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Mn;
}

inline bool IsSpacingMark(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Mc;
}

inline bool IsEnclosingMark(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Me;
}

inline bool IsMark(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::M);
}

inline bool IsDecimalNumber(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Nd;
}

inline bool IsLetterNumber(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Nl;
}

inline bool IsOtherNumber(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::No;
}

inline bool IsNumber(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::N);
}

inline bool IsConnectorPunctuation(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Pc;
}

inline bool IsDashPunctuation(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Pd;
}

inline bool IsOpenPunctuation(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Ps;
}

inline bool IsClosePunctuation(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Pe;
}

inline bool IsInitialPunctuation(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Pi;
}

inline bool IsFinalPunctuation(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Pf;
}

inline bool IsOtherPunctuation(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Po;
}

inline bool IsPunctuation(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::P);
}

inline bool IsMathSymbol(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Sm;
}

inline bool IsCurrencySymbol(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Sc;
}

inline bool IsModifierSymbol(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Sk;
}

inline bool IsOtherSymbol(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::So;
}

inline bool IsSymbol(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::S);
}

inline bool IsSpaceSeparator(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Zs;
}

inline bool IsLineSeparator(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Zl;
}

inline bool IsParagraphSeparator(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Zp;
}

inline bool IsSeparator(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::Z);
}

inline bool IsControl(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Cc;
}

inline bool IsFormat(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Cf;
}

inline bool IsSurrogate(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Cs;
}

inline bool IsPrivateUse(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Co;
}

inline bool IsUnassigned(char32_t c)
{
    return GetGeneralCategory(c) == GeneralCategoryId::Cn;
}

inline bool IsOther(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::C);
}

inline bool IsGraphic(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::G);
}

inline bool IsBase(char32_t c)
{
    return HasGeneralCategory(c, GeneralCategoryId::B);
}

inline bool IsCombining(char32_t c)
{
    return IsMark(c);
}

inline char32_t ToUpper(char32_t c)
{
    return GetCharacterInfo(c).Upper();
}

inline char32_t ToLower(char32_t c)
{
    return GetCharacterInfo(c).Lower();
}

inline char32_t ToTitle(char32_t c)
{
    return GetCharacterInfo(c).Title();
}

inline char32_t ToFolding(char32_t c)
{
    return GetCharacterInfo(c).Folding();
}

inline const std::u32string& FullUpper(char32_t c)
{
    return GetExtendedCharacterInfo(c).FullUpper();
}

inline const std::u32string& FullLower(char32_t c)
{
    return GetExtendedCharacterInfo(c).FullLower();
}

inline const std::u32string& FullTitle(char32_t c)
{
    return GetExtendedCharacterInfo(c).FullTitle();
}

inline const std::u32string& FullFolding(char32_t c)
{
    return GetExtendedCharacterInfo(c).FullFolding();
}

inline bool IsWhiteSpace(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::whiteSpace);
}

inline bool IsAlphabetic(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::alphabetic);
}

inline bool IsAsciiHexDigit(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::asciiHexDigit);
}

bool IsAsciiDigit(char32_t c);

inline bool IsUppercase(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::uppercase);
}

inline bool IsLowercase(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::lowercase);
}

inline bool IsIdStart(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::idStart);
}

inline bool IsIdCont(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::idContinue);
}

inline bool IsGraphemeBase(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::graphemeBase);
}

inline bool IsGraphemeExtender(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::graphemeExtend);
}

inline bool IsOtherLower(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::otherLowercase);
}

inline bool IsOtherUpper(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::otherUppercase);
}

inline const std::string& GetCharacterName(char32_t c)
{
    return GetExtendedCharacterInfo(c).CharacterName();
}

inline const std::string& GetUnicode1Name(char32_t c)
{
    return GetExtendedCharacterInfo(c).Unicode1Name();
}

inline NumericTypeId GetNumericType(char32_t c)
{
    return GetExtendedCharacterInfo(c).GetNumericType();
}

inline const std::string& GetNumericValue(char32_t c)
{
    return GetExtendedCharacterInfo(c).GetNumericValue();
}

inline bool IsBidiMirrored(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::bidiMirrored);
}

inline bool IsBidiControl(char32_t c)
{
    return GetCharacterInfo(c).GetBinaryProperty(BinaryPropertyId::bidiControl);
}

inline char32_t GetBidiMirroringGlyph(char32_t c)
{
    return GetExtendedCharacterInfo(c).GetBidiMirroringGlyph();
}

inline BidiPairedBracketTypeId GetBidiPairedBracketType(char32_t c)
{
    return GetExtendedCharacterInfo(c).GetBidiPairedBracketType();
}

inline char32_t GetBidiPairedBracket(char32_t c)
{
    return GetExtendedCharacterInfo(c).GetBidiPairedBracket();
}

inline const std::vector<Alias>& Aliases(char32_t c)
{
    return GetExtendedCharacterInfo(c).Aliases();
}

} // util
