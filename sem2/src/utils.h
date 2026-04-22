/**
 * Kostra druhe semestralni prace z predmetu KIV/UPP
 * Soubory a hlavicku upravujte dle sveho uvazeni a nutnosti
 */

#pragma once

#include <string>

namespace utils {

// return {N, M} from the args
// may THROW on invalid args
std::tuple<int, int> parse_args(int argc, char **argv);

// precte cely soubor do retezce
// path - cesta k souboru
// vraci obsah souboru nebo prazdny retezec v pripade chyby
std::string readWholeFile(const std::string &path);

// stahne HTML kod stranky z dane URL
// url - adresa stranky
// vraci obsah stranky nebo prazdny retezec v pripade chyby
std::string downloadHTML(const std::string &url);
} // namespace utils
