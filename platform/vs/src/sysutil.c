#include "game.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#include "lua.h"
#include "lauxlib.h"

static char * s_persist_data = NULL;

static int
ltest_crash(lua_State* L) {
    printf("TODO: ltest_crash\n");
    return 0;
}

static int
llist_dir(lua_State* L) {
    printf("TODO: llist_dir\n");
    return 0;
}

static int
lis_dir(lua_State* L) {
    printf("TODO: lis_dir\n");
    return 0;
}

static int
ldelete_file(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    printf("delete: %s\n", path);
    lua_pushboolean(L, DeleteFile(path));
    return 1;
}

static int
lwrite_file(lua_State* L) {
    int i;
    char c;
    char buf[512];
    DWORD n;
    FILE* fp;
    const char* path = luaL_checkstring(L, 1);
    size_t data_len;
    const char* data = luaL_checklstring(L, 2, &data_len);

    printf("write_file: %s\n", path);
    n = GetFullPathNameA(path, 512, &buf[0], NULL);
    if(n <= 0) {
        return luaL_error(L, "GetFullPathName failed.");
    }
    if(n > 512) {
        return luaL_error(L, "GetFullPathName failed: buf size not enough. %d needed", n);
    }
    for(i=0; i<n; ++i) {
        if(buf[i] == '\\') {
            c = buf[i+1];
            buf[i+1] = 0;
            CreateDirectoryA(buf, NULL);
            buf[i+1] = c;
        }
    }

    fp = fopen(buf, "wb");
    if(!fp) {
        return luaL_error(L, "open file(%s) failed", buf);
    }
    fwrite(data, data_len, 1, fp);
    fclose(fp);
    return 0;
}

static int
lread_file(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	FILE * fp = fopen(path, "rb");
	if(!fp) {
		return luaL_error(L, "open file failed!!!!!!!!!! %s", path);
	}
	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char * buffer = malloc(length);
	if(!buffer) {
		fclose(fp);
		return luaL_error(L, "read_file: malloc failed!!!!!!!!!! %s", path);
	}
	fread(buffer, 1, length, fp);
	fclose(fp);
	lua_pushlstring(L, buffer, length);
	return 1;
}

static int
lexists(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    printf("exists: %s\n", path);
    struct _stat st;
    if(0 == _stat(path, &st)) {
        lua_pushboolean(L, 1);
        printf("    -> true\n");
        return 1;
    }
    if(errno == ENOENT) {
        lua_pushboolean(L, 0);
        printf("    -> false\n");
        return 1;
    };
    return luaL_error(L, "_stat errno: %d", errno);
}

static int
ljoin_path(lua_State* L) {
    printf("TODO: ljoin_path\n");
    return 0;
}

static int
lget_default_language(lua_State* L) {
    printf("TODO: lget_default_language\n");
    return 0;
}

static int
lallow_screen_off(lua_State* L) {
    printf("TODO: lallow_screen_off\n");
    return 0;
}

static int
lkeep_screen_on(lua_State* L) {
    printf("TODO: lkeep_screen_on\n");
    return 0;
}

static int
lopen_url(lua_State* L) {
    printf("TODO: lopen_url\n");
    return 0;
}

static int
linput(lua_State* L) {
    printf("TODO: linput\n");
    return 0;
}

static int
lalert_with_callback(lua_State* L) {
    printf("TODO: lalert_with_callback\n");

	const char* strTitle = luaL_checkstring(L,1);
	const char* strMsg   = luaL_checkstring(L,2);
	int   iid            = luaL_checkinteger(L,4);

	printf("\n\n %s \n\n", strMsg);

    return 0;
}

static int
lalert(lua_State* L) {
    printf("TODO: lalert\n");
    return 0;
}

static int
lget_IDFA(lua_State* L) {
    printf("TODO: lget_IDFA\n");
    return 0;
}

static int
lterminate(lua_State* L) {
    printf("TODO: lterminate\n");
    return 0;
}

static int
lreload(lua_State* L) {
	game_reload();
	return 0;
}

//static int
//dummy(lua_State* L) {
//    printf("dummy\n");
//    return 0;
//}

static int
llog(lua_State* L) {
    int i;
    printf("<log>");
    for(i=1; i<=lua_gettop(L); ++i) {
        printf("%s", lua_tostring(L, i));
    }
    printf("\n");
    return 0;
}

static int
try_create_dir(const char* dir) {
	DWORD dwAttrib = GetFileAttributesA(dir);

	if(dwAttrib != INVALID_FILE_ATTRIBUTES && 
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
			return 1;
	}

	return CreateDirectoryA(dir, NULL);
}

static int
lmkdir(lua_State* L) {
	int i;
	char buf[FILENAME_MAX];
	size_t length;
	const char* dir_path = luaL_checklstring(L, 1, &length);
	if(length >= FILENAME_MAX) {
		return luaL_error(L, "path too long");
	}
	memcpy(buf, dir_path, length);
	buf[length] = 0;

	printf("mkdir: %s\n", dir_path);

	for(i=0; i<length; ++i) {
		if(buf[i] == '\\' || buf[i] == '/') {
			char c = buf[i];
			buf[i] = 0;
			if(i>0 && buf[i-1] != ':') {
				if(!try_create_dir(buf)) {
					lua_pushboolean(L, 0);
					return 1;
				}
			}

			buf[i] = c;
		}
	}
	lua_pushboolean(L, try_create_dir(dir_path));
	return 1;
}

// modes:
//   "d" for Documents
//   "l" for Library
//   "c" for Caches
static int
lget_path(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    const char* mode = luaL_optstring(L, 2, "d");
    char buf[512];
    DWORD n = GetCurrentDirectoryA(512, &buf[0]);
    if(n == 0) {
        return 0;
    }
    if(n > 512) {
        return luaL_error(L, "buf size not enough. %d needed", n);
    }

    if(mode == NULL || mode[0] == 'd') {
        strcat(buf, "\\document\\");
    } else if(mode[0] == 'l') {
        strcat(buf, "\\library\\");
    } else if(mode[0] == 'c') {
        strcat(buf, "\\cache\\");
    } else {
        return luaL_error(L, "unknown mode '%s' for 'get_path'", mode);
    }
    strcat(buf, filename);
    lua_pushstring(L, buf);
    return 1;
}

static int
set_persist_data(lua_State *L) {
	const char * data = lua_tostring(L, 1);
	if(s_persist_data) {
		free(s_persist_data);
	}
	s_persist_data = data ? strdup(data) : NULL;
	return 0;
}

static int
get_persist_data(lua_State *L) {
	lua_pushstring(L, s_persist_data);
	return 1;
}

static int
_device_usedmemory(lua_State* L) {
	PROCESS_MEMORY_COUNTERS info;
	GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
	double mem = (double)info.WorkingSetSize / 1024 / 1024;
	lua_pushnumber(L, mem);
	return 1;
}

int
luaopen_sysutil_c(lua_State* L) {
    luaL_checkversion(L);

    luaL_Reg l[] = {
        {"terminate", lterminate},
		{"reload", lreload},

        {"get_IDFA", lget_IDFA},

        // alert & input
        {"alert", lalert},
        {"alert_with_callback", lalert_with_callback},
        {"input", linput},

        // util
        {"log", llog},

        {"open_url", lopen_url},
        {"keep_screen_on", lkeep_screen_on},
        {"allow_screen_off", lallow_screen_off},
        {"get_default_language", lget_default_language},

        // filesystem
        {"mkdir", lmkdir},
        {"get_path", lget_path},
        {"join_path", ljoin_path},
        {"exists", lexists},
        {"read_file", lread_file},
        {"write_file", lwrite_file},
        {"delete_file", ldelete_file},
        {"list_dir", llist_dir},
        {"is_dir", lis_dir},

		// device
		{"device_usedmemory", _device_usedmemory},

        // crash for test
        {"test_crash", ltest_crash},

		{"set_persist_data", set_persist_data},
		{"get_persist_data", get_persist_data},

        {NULL, NULL}
    };

    luaL_newlib(L, l);
    return 1;
}

