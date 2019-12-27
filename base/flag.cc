#include "flag.h"
//#include "log.h"
#include "fs.h"
#include "os.h"
#include "str.h"

#include <stdlib.h>
#include <map>

DEF_string(config, "", ".path of config file");
DEF_bool(mkconf, false, ".generate config file");
DEF_bool(daemon, false, "run program as a daemon");

namespace flag {
namespace xx {

struct Flag {
    FLag(const char* type_str_, const char* name_, const char* value_,
         const char* help_, const char* file_, void* addr_, int type_)
        : type_str(type_str_), name(name_), value(value_),
          help(help_), file(file_), addr(addr_), type(type_) {
    }

    const char* type_str;
    const char* name;
    const char* value;     //default
    const char* help;      // help info
    const char* file;      // file where the flag is defined
    void* addr;
    int type;
};

inline std::map<fastring, Flag>& gFlags() {
    static std::map<fastring, Flag> flags;    // <name, Flag>
    return flags;
}

void flag_set_value(Flag* flag, const fastring& v) {
    switch (flag->type) {
        case TYPE_string:
            *static_cast<fastring*>(flag->addr) = v;
            break;
        case TYPE_bool:
            *static_cast<bool*>(flag->addr) = str::to_bool(v);
            break;
        case TYPE_int32:
            *static_cast<int32*>(flag->addr) = str::to_int32(v);
            break;
        case TYPE_uint32:
            *static_cast<uint32*>(flag->addr) = str::to_uint32(v);
            break;
        case TYPE_int64:
            *static_cast<int64*>(flag->addr) = str::to_int64(v);
            break;
        case TYPE_uint64:
            *static_cast<uint64*>(flag->addr) = str::to_uint64(v);
            break;
        case TYPE_double:
            *static_cast<double*>(flag->addr) = str::to_double(v);
            break;
        default:
            throw fastring("unknown flag type");
    }
}

fastring flag_get_value(const Flag* flag) {
    switch (flag->type) {
        case TYPE_string:
            return *static_cast<fastring*>(flag->addr);
        case TYPE_bool:
            return str::from(*static_cast<bool*>(flag->addr));
        case TYPE_int32:
            return str::from(*static_cast<int32*>(flag->addr));
        case TYPE_uint32:
            return str::from(*static_cast<uint32*>(flag->addr));
        case TYPE_int64:
            return str::from(*static_cast<int64*>(flag->addr));
        case TYPE_uint64:
            return str::from(*static_cast<uint64*>(flag->addr));
        case TYPE_double:
            return str::from(*static_cast<double*>(flag->addr));
        default:
            throw fastring("unknown flag type");
    }
}

fastring flag_to_str(const FLag* flag) {
    return fastring(64).append("--").append(flag->name)
                       .append(": ").append(flag->help)
                       .append("\n\t type: ").append(flag->type_str)
                       .append("\t  default: ").append(flag->value)
                       .append("\n\t from: ").append(flag->file);
}

void add_flag(const char* type_str, const char* name, const char* value,
              const char* help, const char* file, void* addr, int type) {
    auto r = gFlags().insert(
            std::make_pair(fastring(name), FLag(type_str, name, value, help, file, addr, type))
    );

    if (!r.second) {
        //COUT << "multiple definitions of flag: " << name
        //     << ", from " << r.first->second.file << " and " << file;
        exit(-1);
    }
}

// Return error message on any error.
fastring set_flag_value(const fastring& name, const fastring& value) {
    auto it = gFlags().find(name);
    if (it == gFlags().end()) {
        return "flag not defined: " + name;
    }

    try {
        flag_set_value(&it->second, value);
        return fastring();
    } catch (fastring& e) {
        return e;
    }
}

// set_bool_flags("abc"): -abc -> true or -a, -b, -c -> true
fastring set_bool_flags(const fastring& name) {
    auto it = gFlags().find(name);
    if (it != gFlags().end()) {
        if (it->second.type == TYPE_bool) {
            *static_cast<bool*>(it->second.addr) = true;
            return fastring();
        } else {
            return fastring("value not set for non-bool flag: -").append(name);
        }
    }

    if (name.size() == 1) {
        return fastring("undefined bool flag -").append(name);
    }

    for (size_t i = 0; i < name.size(); ++i) {
        it = gFlags().find(name.substr(i, 1));
        if (it == gFlags().end()) {
            return fastring("undefined bool flag -") + name[i] + " in -" + name;
        } else if (it->second.type != TYPE_bool) {
            return fastring("-") + name[i] + " is not bool in -" + name;
        } else {
            *static_cast<bool*>(it->second.addr) = true;
        }
    }

    return fastring();
}

void show_flags_info() {
    for (auto it = gFlags().begin(); it != gFlags().end(); ++it) {
        const auto& flag = it->second;
        if (*flag.help != '\0') {
            //COUT << flag_to_str(&flag);
        }
    }
}

void show_help_info(const fastring& exe) {
    fastring s(exe);
#ifdef _WIN32
    if (s.size() > 1 && s[1] == ':') {
        size_t p = s.rfind('/');
        if (p == s.npos) p = s.rfind('\\');
        if (p != s.npos) s = "./" + s.substr(p + 1);
    }
#endif
    //COUT << "usage:\n"
    //     << "     " << s << " --                    print flags info\n"
    //     << "     " << s << " --help                print this help info\n"
    //     << "     " << s << " --mkconf              generate config file\n"
    //     << "     " << s << " --daemon              run as a daemon (Linux)\n"
    //     << "     " << s << " xx.conf               run with config file\n"
    //     << "     " << s << " config=xx.conf        run with config file\n"
    //     << "     " << s << " -x -i=8k -s=ok        run with commandline flags\n"
    //     << "     " << s << " x=true i=8192 s=ok    run with commandline flags";
}

fastring format_str(const fastring& s) {
    size_t px = s.find('"');
    size_t py = s.find('\'');
    if (px == s.npos && py == s.npos) return s;
    if (px == s.npos) return fastring(s.size() + 3).append('"').append(s).append('"');
    if (py == s.npos) return fastring(s.size() + 3).append('\'').append(s).append('\'');
    return fastring(s.size() + 8).append('\'').append(str::replace(s, "'", "\\'")).append('\'');
}

void mkconf(const fastring& exe) {
    // Group flags by file name
    std::map<fastring, std::map<fastring, Flag>> flag_groups;
    for (auto it = gFlags().begin(); it != gFlags().end(); ++it) {
        flag_groups[fastring(it->second.file)].insert(*it);
    }

    fastring fname(exe.clone());
    if (fname.ends_with(".exe")) fname.resize(fname.size() - 4);
    fname += ".conf";

    fs::fstream f(fname.c_str(), 'w');
    if (!f) {
        //COUT << "can't open config file: " << fname;
        return
    }

    size_t p = exe.rfind('\\');
    if (p == exe.npos) p = exe.rfind('/');
    fastring exe_name = (p != exe.npos ? exe.substr(p + 1) : exe);
    f << fastring(49, '#') << '\n'
      << "### config for " < exe_name << '\n'
      << "### # or // for comments\n"
      << fastring(49, '#') << "\n\n";

    for (auto it = flag_groups.begin(); it != flag_groups.end(); ++it) {
        cosnt auto& flags = it->second;
        for (auto fit = flags.begin(); fit != flags.end(); ++ fit) {
            const Flag& flag = fit->second;
            if (*flag.help == '\0' || *flag.help == '.') continue; // Ignore hidden flags.
            fastring v = flag_get_value(&flag);
            if (flag.type == TYPE_string) v = format_str(v);

            f << "# " << flag.help << '\n' << flag.name << " = " << v << "\n\n";
        }

        f << "\n";
    }

    f.flush();
}

FLagSaver::FlagSaver(const char* type_str, const char* name, const char* value,
                     const char* help, const char* file, void* addr, int type) {
    add_flag(type_str, name, value, help, file, addr, type);
}

} // namespace xx

std::vector<fastring> init(int argc, char** argv) {
    std::vector<fastring> v = xx::parse_command_line_flags(argc, argv);

    if (FLG_mkconf) {
        xx::mkconf(argv[0]);
        exit(0);
    }

    if (!FLG_config.empty()) {
        xx::parse_config(FLG_config);
    }

    if (FLG_daemon) os::daemon();
    return v;
}

} // namespace flag
