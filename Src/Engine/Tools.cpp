#include "Tools.hpp"

void erase_all(std::string& s, const std::string& to_erase) {
    size_t pos;
    while((pos = s.find(to_erase)) != std::string::npos) s.erase(pos, to_erase.length());
}

void erase_all(std::string& s, const std::string& to_erase_begin, const std::string& to_erase_end) {
    size_t pos;
    while((pos = s.find(to_erase_begin)) != std::string::npos && s.find(to_erase_end, pos) != std::string::npos) s.erase(pos, s.find(to_erase_end, pos) + to_erase_end.length() - pos);
}

void replace_all(std::string& s, const std::string& to_replace, const char* replace_with) {
    size_t pos;
    while((pos = s.find(to_replace)) != std::string::npos) s.replace(pos, to_replace.length(), replace_with);
}

bool match_all(std::string& s, const std::string& to_match, std::function<bool(std::string&, size_t)> callback) {
    size_t pos;
    while((pos = s.find(to_match)) != std::string::npos) if(!callback(s, pos)) return false;
    return true;
}

size_t find_not_escaped(std::string& s, const std::string& to_find, size_t start_pos) {
    while((start_pos = s.find(to_find, start_pos + 1)) != std::string::npos) if(start_pos == 0 || s[start_pos - 1] != '\\') return start_pos;
    return std::string::npos;
}

// todo: possibly optimize this?
std::vector<std::string> split(std::string& s, const char& separator, const char& escape_character, bool keep_escape_chars) {
    std::vector<std::string> out;
    bool escaped = false;
    size_t len = s.length();
    size_t pos = 0;
    std::string cur = "";

    // Skip any leading separators
    while(pos != len && s[pos] == separator) pos++;

    while(pos != len) {
        char& c = s[pos];
        if(c == escape_character && (pos == 0 || s[pos - 1] != '\\')) {
            escaped = !escaped;
            if(keep_escape_chars) cur += c;
        } else if(c == separator && !escaped) {
            out.push_back(cur);
            cur = "";
            // Skip any subsequent separators
            while(s[pos] == separator) pos++;
            continue;
        } else cur += c;
        pos++;
    }

    if(cur != "") out.push_back(cur);
    return out;
}
