// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module wing.text.view;

namespace wing {

Color DefaultSelectionBackgroundColor()
{
    return Color(153, 201, 239);
}

std::u32string TrimEnd(const std::u32string& line)
{
    int i = line.length();
    while (i > 0 && std::isspace(line[i - 1]))
    {
        --i;
    }
    return line.substr(0, i);
}

std::vector<std::u32string> SplitTextIntoLines(const std::u32string& text)
{
    std::vector<std::u32string> lines;
    std::u32string line;
    int state = 0;
    for (char32_t c : text)
    {
        switch (state)
        {
        case 0:
        {
            switch (c)
            {
            case '\n':
            {
                lines.push_back(TrimEnd(line));
                line.clear();
                break;
            }
            case '\r':
            {
                state = 1;
                break;
            }
            default:
            {
                line.append(1, c);
                break;
            }
            }
            break;
        }
        case 1:
        {
            if (c == '\n')
            {
                lines.push_back(TrimEnd(line));
                line.clear();
                state = 0;
            }
            break;
        }
        }
    }
    if (!line.empty())
    {
        lines.push_back(TrimEnd(line));
    }
    return lines;
}

} // wing
