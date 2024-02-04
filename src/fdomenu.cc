/*
 *  FDOmenu - Menu code generator for icewm
 *  Copyright (C) 2015-2022 Eduard Bloch
 *
 *  Inspired by icewm-menu-gnome2 and Freedesktop.org specifications
 *  Using pure glib/gio code and a built-in menu structure instead
 *  the XML based external definition (as suggested by FD.o specs)
 *
 *  Released under terms of the GNU Library General Public License
 *  (version 2.0)
 *
 *  2015/02/05: Eduard Bloch <edi@gmx.de>
 *  - initial version
 *  2018/08:
 *  - overhauled program design and menu construction code, added sub-category handling
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "config.h"
#include "base.h"
//#include "sysdep.h"
#include "intl.h"
#include "appnames.h"
#include "ylocale.h"

#include <stack>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <map>

char const* ApplicationName;

#ifndef LPCSTR // mind the MFC
// easier to read...
typedef const char* LPCSTR;
#endif

#include <glib.h>
#include <gmodule.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <gio/gdesktopappinfo.h>
#include "ycollections.h"

// program options
bool add_sep_before = false;
bool add_sep_after = false;
bool no_sep_others = false;
bool no_sub_cats = false;
bool generic_name = false;
bool right_to_left = false;
bool flat_output = false;
bool match_in_section = false;
bool match_in_section_only = false;

auto substr_filter = "";
auto substr_filter_nocase = "";
auto flat_sep = " / ";
char* terminal_command;
char* terminal_option;

template<typename T, void TFreeFunc(T)>
struct auto_raii {
    T m_p;
    auto_raii(T xp) :
            m_p(xp) {
    }
    ~auto_raii() {
        if (m_p)
            TFreeFunc(m_p);
    }
};
typedef auto_raii<gpointer, g_free> auto_gfree;
typedef auto_raii<gpointer, g_object_unref> auto_gunref;

/*
static int cmpUtf8(const void *p1, const void *p2) {
    return g_utf8_collate((LPCSTR ) p1, (LPCSTR ) p2);
}
*/
struct tLtUtf8 {
    bool operator() (const std::string& a, const std::string& b) {
        return g_utf8_collate(a.c_str(), b.c_str()) < 0;
    }
} lessThanUtf8;

class tDesktopInfo;
typedef YVec<const gchar*> tCharVec;
tCharVec sys_folders, home_folders;
tCharVec* sys_home_folders[] = { &sys_folders, &home_folders };
const char* rtls[] = {
    "ar",   // arabic
    "fa",   // farsi
    "he",   // hebrew
    "ps",   // pashto
    "sd",   // sindhi
    "ur",   // urdu
};

/*
 * Two relevant columns from https://specifications.freedesktop.org/menu-spec/latest/apas02.html
 * exported as CSV with , delimiter and with manual fix of HardwareSettings order.
 *
 * Powered by PERL! See contrib/conv_cat.pl
 */

#include "fdospecgen.h"


char const * const par_sec_dummy[] = { nullptr };
class tDesktopInfo;


void decorate_and_print();


// little adapter class to extract information we get from desktop files using GLib methods
class tDesktopInfo {
    GDesktopAppInfo *pInfo = nullptr;
    LPCSTR d_file = nullptr;
    char* found_name = nullptr;
public:
    tDesktopInfo(LPCSTR szFileName) : d_file(strdup(szFileName))  {
        pInfo = g_desktop_app_info_new_from_filename(szFileName);
        if (!pInfo)
            return;

        // tear down if not permitted
        if (!g_app_info_should_show(*this)) {
            g_object_unref(pInfo);
            pInfo = nullptr;
        }
    }

    inline operator GAppInfo*() {
        return (GAppInfo*) pInfo;
    }

    inline operator GDesktopAppInfo*() {
        return pInfo;
    }

    ~tDesktopInfo() {
        // XXX: call g_free on pInfo? free on d_file?
     }

    LPCSTR get_name() const {
        if (!pInfo)
            return nullptr;
        if (found_name)
            return found_name;
        found_name = g_app_info_get_display_name((GAppInfo*) pInfo);
        if (found_name)
            return found_name;
        return found_name = g_desktop_app_info_get_generic_name(pInfo);
    }
#if 0
    LPCSTR get_generic() const {
        if (!pInfo || !generic_name)
            return nullptr;
        return g_desktop_app_info_get_generic_name(pInfo);
    }
    #endif

    char * get_icon_path() const {
        auto pIcon = g_app_info_get_icon((GAppInfo*) pInfo);
        auto_gunref free_icon((GObject*) pIcon);
        return pIcon ? g_icon_to_string(pIcon) : nullptr;
    }

    // split tokens into a list, to be cleared by g_strfreev
    gchar** get_cat_tokens() {

        LPCSTR pCats = g_desktop_app_info_get_categories(pInfo);
        if (!pCats)
            pCats = "Other";
        if (0 == strncmp(pCats, "X-", 2))
            return (gchar**) malloc(0);

        return g_strsplit(pCats, ";", -1);
    }

#error FIXME, memory cleanup
    std::unique_ptr<LPCSTR, free_functor> get_launch_cmd() {

        LPCSTR cmdraw = g_app_info_get_commandline((GAppInfo*) pInfo);
        if (!cmdraw || !*cmdraw)
            return g_strdup("-");

        // if the strings contains the exe and then only file/url tags that we wouldn't
        // set anyway, THEN create a simplified version and use it later (if bSimpleCmd is true)
        // OR use the original command through a wrapper (if bSimpleCmd is false)
        bool bUseSimplifiedCmd = true;
        gchar * cmdMod = g_strdup(cmdraw);
        gchar *pcut = strpbrk(cmdMod, " \f\n\r\t\v");

        if (pcut) {
            bool bExpectXchar = false;
            for (gchar *p = pcut; *p && bUseSimplifiedCmd; ++p) {
                int c = (unsigned) *p;
                if (bExpectXchar) {
                    if (strchr("FfuU", c))
                        bExpectXchar = false;
                    else
                        bUseSimplifiedCmd = false;
                    continue;
                } else if (c == '%') {
                    bExpectXchar = true;
                    continue;
                } else {
                    if (isspace(unsigned(c)))
                        continue;
                    else {
                        if (!strchr(p, '%'))
                            goto cmdMod_is_good_as_is;
                        else
                            bUseSimplifiedCmd = false;
                    }
                }
            }

            if (bExpectXchar)
                bUseSimplifiedCmd = false;
            if (bUseSimplifiedCmd)
                *pcut = '\0';
            cmdMod_is_good_as_is: ;
        }

        bool bForTerminal = false;
    #if GLIB_VERSION_CUR_STABLE >= G_ENCODE_VERSION(2, 36)
        bForTerminal = g_desktop_app_info_get_boolean(pInfo, "Terminal");
    #else
        // cannot check terminal property, callback is as safe bet
        bUseSimplifiedCmd = false;
    #endif

        char *alt = nullptr;
        if (bUseSimplifiedCmd && !bForTerminal) // best case
            return cmdMod;
        else if (bForTerminal && nonempty(terminal_command))
            progCmd = g_strjoin(" ", terminal_command, "-e", cmdMod, NULL);
        else
            // not simple command or needs a terminal started via launcher callback, or both
            progCmd = g_strdup_printf("%s \"%s\"", ApplicationName, dinfo.d_file);
        if (cmdMod && cmdMod != progCmd)
            g_free(cmdMod), cmdMod = nullptr;
    }
};

struct tMenuAnchor {
    // list of application elements, ordered by the translated names
    std::map<LPCSTR, tDesktopInfo*, lessThanUtf8> translated_apps;
    bool is_localized_builtin = false;
    bool is_localized_system = false;
    std::string icon_name = "folder";
};


struct tStaticMenuDescription {
    LPCSTR key, icon;
    const char ** parent_sec;
};

// shim class which describes the Menu related attributes, which can be fetched from static data or adjusted with localized or additional things from filesystem
class tListMeta {
    LPCSTR title = nullptr, icon = nullptr, key = nullptr;
    tStaticMenuDescription *builtin_data = nullptr;
    tListMeta() {}
    /*
    enum eLoadFlags :char {
        NONE = 0,
        DEFAULT_TRANSLATED = 1,
        SYSTEM_TRANSLATED = 2,
        DEFAULT_ICON = 4,
        FALLBACK_ICON = 8,
        SYSTEM_ICON = 16,
    };
    short load_state_icon, load_state_title;
*/

    std::unordered_map<std::string, const tListMeta*> meta_lookup_data;
    std::unordered_map<std::string, std::string> known_directory_files;

public:
    tListMeta(const tDesktopInfo&);
    static tListMeta make_dummy() { return tListMeta();}
    LPCSTR get_key() const {
        if (key)
            return key;
        if (builtin_data && builtin_data->key)
            return builtin_data->key;
        return "<UNKNOWN>";
    }
    LPCSTR get_icon() const {
        if (icon)
            return icon;
        if (builtin_data && builtin_data->icon)
            return builtin_data->icon;
        return "-";
    }
    LPCSTR get_title() const {
        if (title)
            return title;
        if (builtin_data && builtin_data->key)
            return builtin_data->key;
        return "<UNKNOWN>";
    }
    decltype(tStaticMenuDescription::parent_sec) get_parent_secs() const {
        if (builtin_data)
            return builtin_data->parent_sec;
        return (const char**) &par_sec_dummy;
    }
};


tListMeta* lookup_category(const std::string& key)
{
            #ifdef OLD_IMP

    auto it = meta_lookup_data.find(key);
    if (it == meta_lookup_data.end())
        return nullptr;
    auto* ret = (tListMeta*) it->second;
#ifdef CONFIG_I18N
        // auto-translate the default title for the user's language
    //if (ret && !ret->title)
    //    ret->title = gettext(ret->key);
    //printf("Got title? %s -> %s\n", ret->key, ret->title);
#endif
    return ret;
    #else
return nullptr;
    #endif
}

std::stack<std::string> flat_pfxes;

const char* flat_pfx()
{
    if (!flat_output || flat_pfxes.empty())
        return "";
    return flat_pfxes.top().c_str();
}

void flat_add_level(const char *name)
{
    if (!flat_output)
        return;
    if (flat_pfxes.empty())
        flat_pfxes.push(std::string(name) + flat_sep);
    else
        flat_pfxes.push(flat_pfxes.top() + name + flat_sep);
}

void flat_drop_level()
{
    flat_pfxes.pop();
}

bool filter_matched(const char* title, const char* section_prefix)
{
    if (!match_in_section)
        section_prefix = nullptr;
    if (match_in_section_only)
        title = nullptr;

    bool hasFilter(substr_filter && *substr_filter),
            hasIfilter (substr_filter_nocase && *substr_filter_nocase);

    if (hasFilter) {
         if(title && strstr(title, substr_filter))
             return true;
         if(section_prefix && strstr(section_prefix, substr_filter))
             return true;
    }
#ifdef __GNU_LIBRARY__
    if (hasIfilter) {
        if(title && strcasestr(title, substr_filter_nocase))
            return true;
        if(section_prefix && strcasestr(section_prefix, substr_filter_nocase))
            return true;
    }
#endif
    return ! (hasFilter || hasIfilter);
}

struct t_menu_node;
extern t_menu_node root;

// very basic, avoid vtables!
struct t_menu_node {
protected:
    // for leafs -> NULL, otherwise sub-menu contents
    std::map<std::string, const t_menu_node*, tLtUtf8> store;
    char* progCmd;
    const char* generic;
public:
    const tListMeta *meta;
    t_menu_node(const tListMeta* desc):
        progCmd(nullptr), generic(nullptr), meta(desc) {}

    struct t_print_visitor {
        int count, level;
        t_menu_node* print_separated;
    };

    void print(t_print_visitor *ctx) {
        if (!meta)
            return;
        auto title = meta->get_title();

        if (store.empty())
        {
            if (title && progCmd &&
                    filter_matched(title, flat_pfxes.empty()
                                   ? nullptr : flat_pfxes.top().c_str()))
            {
                if (ctx->count == 0 && add_sep_before)
                    puts("separator");
                if (nonempty(generic) && !strcasestr(title, generic)) {
                    if (right_to_left) {
                        printf("prog \"(%s) %s%s\" %s %s\n",
                                generic, flat_pfx(), title, meta->get_icon(), progCmd);
                    } else {
                        printf("prog \"%s%s (%s)\" %s %s\n",
                                flat_pfx(), title, generic, meta->get_icon(), progCmd);
                    }
                }
                else
                    printf("prog \"%s%s\" %s %s\n", flat_pfx(), title, meta->get_icon(), progCmd);
            }
            ctx->count++;
            return;
        }

        // else: render as menu

        if (ctx->level == 1 && !no_sep_others
                && 0 == strcmp(meta->get_key(), "Other")) {

            ctx->print_separated = this;
            return;
        }

        // root level does not have a name, for others open category menu
        if (ctx->level > 0) {
            if (ctx->count == 0 && add_sep_before)
                puts("separator");
            ctx->count++;
            if (flat_output)
                flat_add_level(title);
            else
                printf("menu \"%s\" %s {\n", title, meta->get_icon());
        }
        ctx->level++;

        for(auto& it: this->store) {
            ((t_menu_node*) it.second)->print(ctx);
        }

        if (ctx->level == 1 && ctx->print_separated)
        {
            fflush(stdout);
            puts("separator");
            no_sep_others = true;
            ctx->print_separated->print(ctx);
        }
        ctx->level--;
        if (ctx->level > 0) {
            if (flat_output)
                flat_drop_level();
            else {
#ifndef DEBUG
                puts("}");
#else
                printf("# end of menu \"%s\"\n}\n", title);
#endif
            }
        }
        if (add_sep_after && ctx->level == 0 && ctx->count > 0)
            puts("separator");

    }

    /**
     * Usual print method for the root node
     */
    void print() {
        t_print_visitor ctx = {0,0,nullptr};
        print(&ctx);
    }

    t_menu_node* add(t_menu_node* node) {
        auto nam = node->meta->get_title();
        if (nam)
            store[nam] = node;
        return node;
    }

    /**
     * Returns a sub-menu which is named by the title (or key) of the provided information.
     * Creates one as needed or finds existing one.
     */
    t_menu_node* get_subtree(const tListMeta* info) {
        auto title = info->get_title();
        auto it = store.find(title);
        return (it != store.end()) ? (t_menu_node*) it->second
                                   : add(new t_menu_node(info));
    }

    /**
     * Find and examine the possible subcategory, try to assign it to the particular main category with the proper structure.
     * When succeeded, blank out the main category pointer in matched_main_cats.
     */
    void try_add_to_subcat(t_menu_node* pNode, const tListMeta* subCatCandidate, YVec<tListMeta*> &matched_main_cats)
    {
        t_menu_node *pTree = &root;
        // skip the rest of the further nesting (cannot fit into any)
        bool skipping = false;

        tListMeta* pNewCatInfo = nullptr;
        tListMeta** ppLastMainCat = nullptr;

        for (auto pSubCatName = subCatCandidate->get_parent_secs();
                *pSubCatName; ++pSubCatName) {
            // stop nesting, add to the last visited/created submenu
            bool store_here = **pSubCatName == '|';
            if (skipping && store_here) {
                skipping = false;
                pTree = &root;
                continue;
            }
            if (store_here) {
                pTree->get_subtree(subCatCandidate)->add(pNode);
                // main menu was served, don't come here again
                if (ppLastMainCat)
                    *ppLastMainCat = nullptr;
            }

            skipping = true;
            // check main category filter, enter from root for main categories

            for (tListMeta** ppMainCat = matched_main_cats.data;
                    ppMainCat < matched_main_cats.data + matched_main_cats.size;
                    ++ppMainCat) {

                if (!*ppMainCat)
                    continue;

                // empty filter means ANY
                if (**pSubCatName == '\0'
                    || 0 == strcmp(*pSubCatName, (**ppMainCat).get_key())) {
                    // the category is enabled!
                    skipping = false;
                    pTree = &root;
                    pNewCatInfo = *ppMainCat;
                    ppLastMainCat = ppMainCat;
                    break;
                }
            }
            if (skipping)
                continue;
            // if not on first node, make or find submenues for the comming tokens
            if (!pNewCatInfo)
                pNewCatInfo = lookup_category(*pSubCatName);
            if (!pNewCatInfo)
                return; // heh? fantasy category? Let caller handle it
            pTree = pTree->get_subtree(pNewCatInfo);
        }
    }
    void add_by_categories(t_menu_node* pNode, gchar **ppCats) {
        static YVec<tListMeta*> matched_main_cats, matched_sub_cats;
        matched_main_cats.size = matched_sub_cats.size = 0;

        for (gchar **pCatKey = ppCats; pCatKey && *pCatKey; ++pCatKey) {
            if (!**pCatKey)
                continue; // empty?
            tListMeta *pResolved = lookup_category(*pCatKey);
            if (!pResolved)
                continue;
            if (!pResolved->get_parent_secs())
                matched_main_cats.add(pResolved);
            else
                matched_sub_cats.add(pResolved);
        }
        if (matched_main_cats.size == 0)
            matched_main_cats.add(lookup_category("Other"));
        if (!no_sub_cats) {
            for (tListMeta** p = matched_sub_cats.data;
                    p < matched_sub_cats.data + matched_sub_cats.size; ++p) {
                try_add_to_subcat(pNode, *p, matched_main_cats);
            }
        }
        for (tListMeta** p = matched_main_cats.data;
                p < matched_main_cats.data + matched_main_cats.size; ++p) {

            if (*p == nullptr)
                continue;
            get_subtree(*p)->add(pNode);
        }
    }
};

bool checkSuffix(const char* szFilename, const char* szFileSfx)
{
    const char* pSfx = strrchr(szFilename, '.');
    return pSfx && 0 == strcmp(pSfx+1, szFileSfx);
}


tListMeta::tListMeta(const tDesktopInfo& dinfo) {
    icon = Elvis((const char*) dinfo.get_icon_path(), "-");
    title = key = Elvis(dinfo.get_name(), "<UNKNOWN>");
}

        #ifdef OLD_IMP

t_menu_node root(tListMeta::make_dummy());
#endif

// variant with local description data
struct t_menu_node_app : t_menu_node
{
    tListMeta description;

    t_menu_node_app(const tDesktopInfo& dinfo) : t_menu_node(&description),
            description(dinfo) {

        LPCSTR cmdraw = g_app_info_get_commandline((GAppInfo*) dinfo.pInfo);
        if (!cmdraw || !*cmdraw)
            return;

        // if the strings contains the exe and then only file/url tags that we wouldn't
        // set anyway, THEN create a simplified version and use it later (if bSimpleCmd is true)
        // OR use the original command through a wrapper (if bSimpleCmd is false)
        bool bUseSimplifiedCmd = true;
        gchar * cmdMod = g_strdup(cmdraw);
        gchar *pcut = strpbrk(cmdMod, " \f\n\r\t\v");

        if (pcut) {
            bool bExpectXchar = false;
            for (gchar *p = pcut; *p && bUseSimplifiedCmd; ++p) {
                int c = (unsigned) *p;
                if (bExpectXchar) {
                    if (strchr("FfuU", c))
                        bExpectXchar = false;
                    else
                        bUseSimplifiedCmd = false;
                    continue;
                } else if (c == '%') {
                    bExpectXchar = true;
                    continue;
                } else {
                    if (isspace(unsigned(c)))
                        continue;
                    else {
                        if (!strchr(p, '%'))
                            goto cmdMod_is_good_as_is;
                        else
                            bUseSimplifiedCmd = false;
                    }
                }
            }

            if (bExpectXchar)
                bUseSimplifiedCmd = false;
            if (bUseSimplifiedCmd)
                *pcut = '\0';
            cmdMod_is_good_as_is: ;
        }

        bool bForTerminal = false;
    #if GLIB_VERSION_CUR_STABLE >= G_ENCODE_VERSION(2, 36)
        bForTerminal = g_desktop_app_info_get_boolean(dinfo.pInfo, "Terminal");
    #else
        // cannot check terminal property, callback is as safe bet
        bUseSimplifiedCmd = false;
    #endif

        if (bUseSimplifiedCmd && !bForTerminal) // best case
            progCmd = cmdMod;
        else if (bForTerminal && nonempty(terminal_command))
            progCmd = g_strjoin(" ", terminal_command, "-e", cmdMod, NULL);
        else
            // not simple command or needs a terminal started via launcher callback, or both
            progCmd = g_strdup_printf("%s \"%s\"", ApplicationName, dinfo.d_file);
        if (cmdMod && cmdMod != progCmd)
            g_free(cmdMod), cmdMod = nullptr;
        generic = dinfo.get_generic();
    }
    ~t_menu_node_app() {
        g_free(progCmd);
    }
};

std::string get_stem(const std::string& full_path) {
    LPCSTR dot(nullptr), start(full_path.c_str());
    for(auto p = start; ; ++p)
    {
        if ('/' == *p)
            start = p + 1;
        else if ('.' == *p) {
            dot = p;
        }
        else if (!*p) {
            if (!dot)
                dot = p;
            else if (dot < start)
                dot = p;

            break;
        }
    }
    return std::string(start, dot-start);
}

struct tFromTo { LPCSTR from; LPCSTR to;};
// match transformations applied by some DEs
const tFromTo SameCatMap[] = {
        {"Utilities", "Utility"},
        {"Sound & Video", "AudioVideo"},
        {"Sound", "Audio"},
        {"File", "FileTools"},
        {"Terminal Applications", "TerminalEmulator"},
        {"Mathematics", "Math"},
        {"Arcade", "ArcadeGame"},
        {"Card Games", "CardGame"}
};
const tFromTo SameIconMap[] = {
        { "AudioVideo", "Audio" },
        { "AudioVideo", "Video" }
};

typedef void (*tFuncInsertInfo)(LPCSTR szDesktopFile);
void pickup_folder_info(LPCSTR szDesktopFile) {
    GKeyFile *kf = g_key_file_new();
    auto_raii<GKeyFile*, g_key_file_free> free_kf(kf);

    if (!g_key_file_load_from_file(kf, szDesktopFile, G_KEY_FILE_NONE, nullptr))
        return;
    LPCSTR cat_name = g_key_file_get_string(kf, "Desktop Entry", "Name", nullptr);
    // looks like bad data
    if (!cat_name || !*cat_name)
        return;
    // try a perfect match by path or file name
    auto pCat = lookup_category(cat_name);
    if (!pCat)
    {
        auto bn = get_stem(szDesktopFile);
        if (!bn.empty())
            pCat = lookup_category(bn);
    }
    for (const tFromTo* p = SameCatMap;
            !pCat && p < SameCatMap+ACOUNT(SameCatMap);
            ++p) {

        if (0 == strcmp(cat_name, p->from))
            pCat = lookup_category(p->to);
    }

    if (!pCat)
        return;

        #ifdef OLD_IMP
    if (pCat->load_state_icon < tListMeta::SYSTEM_ICON) {
        LPCSTR icon_name = g_key_file_get_string (kf, "Desktop Entry",
                                                       "Icon", nullptr);
        if (icon_name && *icon_name) {
            pCat->icon = icon_name;
            pCat->load_state_icon = tListMeta::SYSTEM_ICON;
        }
    }
    if (pCat->load_state_title < tListMeta::SYSTEM_TRANSLATED) {
        char* cat_title = g_key_file_get_locale_string(kf, "Desktop Entry",
                                                       "Name", nullptr, nullptr);
        if (!cat_title)
            return;
        pCat->title = cat_title;
        char* cat_title_c = g_key_file_get_string(kf, "Desktop Entry",
                                                  "Name", nullptr);
        bool same_trans = 0 == strcmp (cat_title_c, cat_title);
        if (!same_trans)
            pCat->load_state_title = tListMeta::SYSTEM_TRANSLATED;

        // otherwise: not sure, keep searching for a better translation
    }
    // something special, donate the icon to similar items unless they have a better one

    for (auto p = SameIconMap; p < SameIconMap + ACOUNT(SameIconMap); ++p) {

        if (strcmp (pCat->key, p->from))
            continue;

        tListMeta *t = lookup_category (p->to);
        // give them at least some icon for now
        if (t && t->load_state_icon < tListMeta::FALLBACK_ICON) {
            t->icon = pCat->icon;
            t->load_state_icon = tListMeta::FALLBACK_ICON;
        }
    }
                #endif

}

void insert_app_info(const char* szDesktopFile) {
    tDesktopInfo dinfo(szDesktopFile);
    if (!dinfo.pInfo)
        return;

    LPCSTR pCats = g_desktop_app_info_get_categories(dinfo.pInfo);
    if (!pCats)
        pCats = "Other";
    if (0 == strncmp(pCats, "X-", 2))
        return;

    t_menu_node* pNode = new t_menu_node_app(dinfo);
    // Pigeonholing roughly by guessed menu structure

    gchar **ppCats = g_strsplit(pCats, ";", -1);
    root.add_by_categories(pNode, ppCats);
    g_strfreev(ppCats);

}

void proc_dir_rec(LPCSTR syspath, unsigned depth,
        tFuncInsertInfo cb, LPCSTR szSubfolder,
        LPCSTR szFileSfx) {
    gchar *path = g_strjoin("/", syspath, szSubfolder, NULL);
    auto_gfree relmem_path(path);
    GDir *pdir = g_dir_open(path, 0, nullptr);
    if (!pdir)
        return;
    auto_raii<GDir*, g_dir_close> dircloser(pdir);

    const gchar *szFilename(nullptr);
    while (nullptr != (szFilename = g_dir_read_name(pdir))) {
        if (!szFilename || !checkSuffix(szFilename, szFileSfx))
            continue;

        gchar *szFullName = g_strjoin("/", path, szFilename, NULL);
        auto_gfree xxfree(szFullName);
        static GStatBuf buf;
        if (0 != g_stat(szFullName, &buf))
            continue;
        if (S_ISDIR(buf.st_mode)) {
            static ino_t reclog[6];
            for (unsigned i = 0; i < depth; ++i) {
                if (reclog[i] == buf.st_ino)
                    goto dir_visited_before;
            }
            if (depth < ACOUNT(reclog)) {
                reclog[++depth] = buf.st_ino;
                proc_dir_rec(szFullName, depth, cb, szSubfolder,
                        szFileSfx);
                --depth;
            }
            dir_visited_before: ;
        }

        if (!S_ISREG(buf.st_mode))
            continue;

        cb(szFullName);
    }
}

bool launch(LPCSTR dfile, char** argv, int argc) {
    GDesktopAppInfo *pInfo = g_desktop_app_info_new_from_filename(dfile);
    if (!pInfo)
        return false;
#if 0 // g_file_get_uri crashes, no idea why, even enforcing file prefix doesn't help
    if (argc > 0)
    {
        GList* parms = NULL;
        for (int i = 0; i < argc; ++i)
        parms = g_list_append(parms,
                g_strdup_printf("%s%s", strstr(argv[i], "://") ? "" : "file://",
                        argv[i]));
        return g_app_info_launch ((GAppInfo *)pInfo,
                parms, NULL, NULL);
    }
    else
#else
    (void) argv;
    (void) argc;
#endif
    return g_app_info_launch((GAppInfo *) pInfo, nullptr, nullptr, nullptr);
}

static void init() {
#ifdef CONFIG_I18N
    setlocale(LC_ALL, "");

    auto loc = YLocale::getCheckedExplicitLocale(false);
    right_to_left = loc && std::any_of(rtls, rtls + ACOUNT(rtls),
        [&](const char* rtl) {
            return rtl[0] == loc[0] && rtl[1] == loc[1];
        }
    );

    bindtextdomain(PACKAGE, LOCDIR);
    textdomain(PACKAGE);

#endif

    //meta_lookup_data = g_hash_table_new(g_str_hash, g_str_equal);
    //gh_directory_files = g_hash_table_new(g_str_hash, g_str_equal);

#ifdef OLD_IMP

    for (auto& what: spec::menuinfo) {
        if (no_sub_cats && what.parent_sec)
            continue;
        meta_lookup_data[what.key] = &what;
    }
#endif
    const char* terminals[] = { terminal_option, getenv("TERMINAL"), TERM,
                                "urxvt", "alacritty", "roxterm", "xterm",
                                "x-terminal-emulator", "terminator" };
    for (auto term : terminals)
        if (term && (terminal_command = path_lookup(term)) != nullptr)
            break;
}

static void help(LPCSTR home, LPCSTR dirs, FILE* out, int xit) {
    g_fprintf(out,
            "USAGE: icewm-menu-fdo [OPTIONS] [FILENAME]\n"
            "OPTIONS:\n"
            "-g, --generic\tInclude GenericName in parentheses of progs\n"
            "-o, --output=FILE\tWrite the output to FILE\n"
            "-t, --terminal=NAME\tUse NAME for a terminal that has '-e'\n"
            "--seps  \tPrint separators before and after contents\n"
            "--sep-before\tPrint separator before the contents\n"
            "--sep-after\tPrint separator only after contents\n"
            "--no-sep-others\tNo separation of the 'Others' menu point\n"
            "--no-sub-cats\tNo additional subcategories, just one level of menues\n"
            "--flat\tDisplay all apps in one layer with category hints\n"
            "--flat-sep STR\tCategory separator string used in flat mode (default: ' / ')\n"
            "--match PAT\tDisplay only apps with title containing PAT\n"
            "--imatch PAT\tLike --match but ignores the letter case\n"
            "--match-sec\tApply --match or --imatch to apps AND sections\n"
            "--match-osec\tApply --match or --imatch only to sections\n"
            "FILENAME\tAny .desktop file to launch its application Exec command\n"
            "This program also listens to environment variables defined by\n"
            "the XDG Base Directory Specification:\n"
            "XDG_DATA_HOME=%s\n"
            "XDG_DATA_DIRS=%s\n", home, dirs);
    exit(xit);
}

#ifdef DEBUG_xxx
void dbgPrint(const gchar *msg)
{
    tlog("%s", msg);
}
#endif

int main(int argc, char** argv) {
    ApplicationName = my_basename(argv[0]);

#if !GLIB_CHECK_VERSION(2,36,0)
    g_type_init();
#endif

#ifdef DEBUG_xxx
    // XXX: if enabled, one probably also need to enable debug facilities in its code, like
    // what --debug option does
    g_set_printerr_handler(dbgPrint);
    g_set_print_handler(dbgPrint);
#endif

    char* usershare = getenv("XDG_DATA_HOME");
    if (!usershare || !*usershare)
        usershare = g_strjoin(nullptr, getenv("HOME"), "/.local/share", NULL);

    // system dirs, either from environment or from static locations
    LPCSTR sysshare = getenv("XDG_DATA_DIRS");
    if (!sysshare || !*sysshare)
        sysshare = "/usr/local/share:/usr/share";

    if (argc == 2 && checkSuffix(argv[1], "desktop")
            && launch(argv[1], argv + 2, argc - 2)) {
        return EXIT_SUCCESS;
    }

    for (auto pArg = argv + 1; pArg < argv + argc; ++pArg) {
        if (is_version_switch(*pArg)) {
            g_fprintf(stdout,
                    "icewm-menu-fdo "
                    VERSION
                    ", Copyright 2015-2024 Eduard Bloch, 2017-2023 Bert Gijsbers.\n");
            exit(0);
        }
        else if (is_copying_switch(*pArg))
            print_copying_exit();
        else if (is_help_switch(*pArg))
            help(usershare, sysshare, stdout, EXIT_SUCCESS);
        else if (is_long_switch(*pArg, "seps"))
            add_sep_before = add_sep_after = true;
        else if (is_long_switch(*pArg, "sep-before"))
            add_sep_before = true;
        else if (is_long_switch(*pArg, "sep-after"))
            add_sep_after = true;
        else if (is_long_switch(*pArg, "no-sep-others"))
            no_sep_others = true;
        else if (is_long_switch(*pArg, "no-sub-cats"))
            no_sub_cats = true;
        else if (is_long_switch(*pArg, "flat"))
            flat_output = no_sep_others = true;
        else if (is_long_switch(*pArg, "match-sec"))
            match_in_section = true;
        else if (is_long_switch(*pArg, "match-osec"))
            match_in_section = match_in_section_only = true;
        else if (is_switch(*pArg, "g", "generic-name"))
            generic_name = true;
        else {
            char *value = nullptr, *expand = nullptr;
            if (GetArgument(value, "o", "output", pArg, argv + argc)) {
                if (*value == '~')
                    value = expand = tilde_expansion(value);
                else if (*value == '$')
                    value = expand = dollar_expansion(value);
                if (nonempty(value)) {
                    if (freopen(value, "w", stdout) == nullptr) {
                        fflush(stdout);
                    }
                }
                if (expand)
                    delete[] expand;
            }
            else if (GetArgument(value, "m", "match", pArg, argv + argc))
                substr_filter = value;
            else if (GetArgument(value, "M", "imatch", pArg, argv + argc))
                substr_filter_nocase = value;
            else if (GetArgument(value, "F", "flat-sep", pArg, argv + argc))
                flat_sep = value;
            else if (GetArgument(value, "t", "terminal", pArg, argv + argc))
                terminal_option = value;
            else // unknown option
                help(usershare, sysshare, stderr, EXIT_FAILURE);
        }
    }

    init();

    auto split_folders = [](const char* path_string, tCharVec& where) {
        for (auto p = g_strsplit(path_string, ":", -1); *p; ++p)
            where.add(*p);
    };
    split_folders(sysshare, sys_folders);
    split_folders(usershare, home_folders);

    for(auto& where: sys_home_folders) {
        for (auto p = where->data; p < where->data + where->size; ++p)
            proc_dir_rec(*p, 0, insert_app_info, "applications", "desktop");
    }


    for(auto& where: sys_home_folders) {
        for (auto p = where->data; p < where->data + where->size; ++p) {
            proc_dir_rec(*p, 0, pickup_folder_info, "desktop-directories",
                    "directory");
        }
    }

    //root.print();
    decorate_and_print();

    //printf("What? %s\n", gettext("Building"));

    if (nonempty(usershare) && usershare != getenv("XDG_DATA_HOME"))
        g_free(usershare);
    delete[] terminal_command;

    return EXIT_SUCCESS;
}

// vim: set sw=4 ts=4 et:
