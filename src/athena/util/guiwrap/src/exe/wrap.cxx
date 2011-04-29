#include <windows.h>
#include <stdio.h>
#include <list>
#include <assert.h>

#define DEFAULT_PERIOD (1*1000)
#define DIALOG_WCLASS "#32770"
#define DEFAULT_FAIL (1)
#define DEFAULT_SUCC (0)

class CFindInfo {
    struct CFindItem {
        char* title;
        char* wclass;

        CFindItem(char* t, char* w):title(_strdup(t)),wclass(_strdup(w)) {}
        ~CFindItem() { free(title); free(wclass); }
    };
    std::list<CFindItem*> items;
    typedef std::list<CFindItem*>::iterator iter_t;
    DWORD m_count;

public:
    bool found;
    DWORD pid;
    DWORD sendkey_code;
    bool noclip;

    CFindInfo():found(false),pid(0) {}
    ~CFindInfo() {
        for (iter_t i = items.begin(); i != items.end(); i++) {
            CFindItem* item = *i;
            *i = 0;
            delete item;
        }
    }
    void add(char* title, char* wclass) {
        items.push_back(new CFindItem(title, wclass));
    }
    bool match(char* title, char* wclass) {
        for (iter_t i = items.begin(); i != items.end(); i++) {
            CFindItem* item = *i;
            if (!strcmp(title, item->title) &&
                !strcmp(wclass, item->wclass))
            {
                return true;
            }
        }
        return false;
    }
    void inc() {
        m_count++;
    }
    DWORD count() {
        return m_count;
    }
};

void
print_clipboard()
{
    DWORD rc = 0;
    HANDLE hClip = 0;
    LPTSTR text = 0;
    bool clip_open = false;

    if (!OpenClipboard(0)) {
        rc = GetLastError();
        goto cleanup;
    }

    clip_open = true;

    hClip = GetClipboardData(CF_TEXT);
    if (!hClip) {
        rc = GetLastError();
        goto cleanup;
    }

    text = (LPTSTR) GlobalLock(hClip);

    fprintf(stderr,
            "--- START CLIPBOARD ---\n%s\n--- END CLIPBOARD ---\n", text);
    fflush(0);

 cleanup:
    if (text)
    {
        GlobalUnlock(hClip);
    }
    if (clip_open) {
        CloseClipboard();
    }
    if (rc) {
        fprintf(stderr,
                "ERROR: Error while trying to print clipboard (%u)\n", rc);
    }
}

BOOL CALLBACK
EnumWindowsProc(
    HWND hwnd,
    LPARAM lParam
    )
{
    char title[1024];
    char wclass[1024];
    DWORD pid;

    CFindInfo* pInfo = (CFindInfo*)lParam;

    title[0] = 0;
    wclass[0] = 0;

    GetWindowText(hwnd, title, sizeof(title));
    GetClassName(hwnd, wclass, sizeof(wclass));

    title[sizeof(title)-1] = 0;
    wclass[sizeof(wclass)-1] = 0;

    GetWindowThreadProcessId(hwnd, &pid);

    if (pInfo->pid == pid && pInfo->match(title, wclass))
    {
        bool is_dialog = !strcmp(DIALOG_WCLASS, wclass);
        pInfo->found = true;
        pInfo->inc();
        fprintf(stderr,
                "ERROR: The wrappee popped up \"%s\"\n", title);
        fflush(0);
        if ((is_dialog) && !(pInfo->noclip))
        {
            SendMessage(hwnd, WM_COPY, 0, 0);
        }

	if (pInfo->sendkey_code)
	{
		keybd_event(pInfo->sendkey_code, 0, 0, 0);
		keybd_event(pInfo->sendkey_code, 0, KEYEVENTF_KEYUP, 0);
	}
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        if ((is_dialog) && !(pInfo->noclip))
        {
            print_clipboard();
        }
        return FALSE;
    }
    return TRUE;
}

struct opts_t {
    bool kill;
    DWORD max_count;
    DWORD max_run;
    DWORD max_wait;
    DWORD period;
    DWORD fail_code;
    DWORD succ_code;
    DWORD sendkey_code;
    bool report_match_as_non_success;
    bool noclip;

    opts_t():kill(false),max_count(0),max_run(0),max_wait(0),
             period(DEFAULT_PERIOD),fail_code(DEFAULT_FAIL),succ_code(DEFAULT_SUCC),sendkey_code(0),
             report_match_as_non_success(false),noclip(false) {}
};

int
parse_args(
    int argc,
    char* argv[],
    CFindInfo& fi,
    char* cmd,
    int size,
    opts_t& opts
    )
{
    int i;
    bool found = false;
    for (i = 1; (i < argc) && !found; i++)
    {
        if (!strcmp(argv[i], "--exec"))
        {
            found = true;
            continue;
        }
        else if (!strcmp(argv[i], "--kill"))
        {
            opts.kill = true;
        }
        else if (!strcmp(argv[i], "--noclip"))
        {
            opts.noclip = true;
        }
        else if (!strcmp(argv[i], "--matchaserror"))
        {
            opts.report_match_as_non_success = true;
        }
        else if (!strcmp(argv[i], "--maxcount"))
        {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing --maxcount parameter\n");
                fflush(0);
                return 1;
            }
            opts.max_count = atoi(argv[i+1]);
            i++;
        }
        else if (!strcmp(argv[i], "--maxrun"))
        {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing --maxrun parameter\n");
                fflush(0);
                return 1;
            }
            opts.max_run = atoi(argv[i+1]) * 1000;
            i++;
        }
        else if (!strcmp(argv[i], "--maxwait"))
        {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing --maxwait parameter\n");
                fflush(0);
                return 1;
            }
            opts.max_wait = atoi(argv[i+1]) * 1000;
            i++;
        }
        else if (!strcmp(argv[i], "--failure"))
        {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing --failure parameter\n");
                fflush(0);
                return 1;
            }
            opts.fail_code = atoi(argv[i+1]);
            i++;
        }
        else if (!strcmp(argv[i], "--maxwait"))
        {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing --success parameter\n");
                fflush(0);
                return 1;
            }
            opts.succ_code = atoi(argv[i+1]);
            i++;
        }
        else if (!strcmp(argv[i], "--sendkey"))
        {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing --sendkey parameter\n");
                fflush(0);
                return 1;
            }
            opts.sendkey_code = atoi(argv[i+1]);
            i++;
        }
        else if (!strcmp(argv[i], "--period"))
        {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing --period parameter\n");
                fflush(0);
                return 1;
            }
            opts.period = atoi(argv[i+1]) * 1000;
            i++;
        } else {
            if ((argc - i) < 2)
            {
                fprintf(stderr, "ERROR: missing window class\n");
                fflush(0);
                return 1;
            }
            fi.add(argv[i], argv[i+1]);
            i++;
        }
    }
    if ((opts.max_run < 0) ||
        (opts.max_count < 0) ||
        (opts.max_wait < 0) ||
        (opts.period < 0))
    {
        fprintf(stderr, "ERROR: Options values must be greater than zero\n");
        fflush(0);
        return 1;
    }
    if (!found) {
        fprintf(stderr, "ERROR: No --exec directive\n");
        fflush(0);
        return 1;
    }
    if (i < argc) {
        char* p = cmd;
        for (; i < argc; i++)
        {
            if (strchr(argv[i], '"'))
            {
                fprintf(stderr, "ERROR: "
                        "Cannot have double-quote (\") in argument: %s\n",
                       argv[i]);
                fflush(0);
                return 1;
            }
            if (strchr(argv[i], ' '))
            {
                p += _snprintf(p, size - (p - cmd), "\"%s\" ", argv[i]);
            } else {
                p += _snprintf(p, size - (p - cmd), "%s ", argv[i]);
            }
        }
        p[-1] = 0;
    }
    return 0;
}

int
main(
    int argc,
    char* argv[]
    )
{
    char cmd[1024];
    CFindInfo fi;
    int error = 0;
    opts_t opts;

    error = parse_args(argc, argv, fi, cmd, sizeof(cmd), opts);
    if (error)
    {
        fprintf(stderr, "usage: "
                "%s [opts] [title window-class] ... --exec command-line\n"
                "  --maxcount N      kill process on N windows matched\n"
                "  --kill            kill process on first window matched (same as --maxcount 1)\n"
                "  --maxrun SECONDS  kill process after SECONDS run time\n"
                "  --maxwait SECONDS kill process after SECONDS wait time\n"
                "  --period SECONDS  period to use for checking windows\n"
                "  --failure VALUE   return VALUE on failure error\n"
                "  --success VALUE   interpret command-line returning VALUE as success\n"
                "  --matchaserror    return an error code if any windows were matched\n"
		"  --sendkey CODE    sends virtual key CODE to a matched window before closing\n" 
		"  --noclip          don't print out clipboard contents\n", 
                argv[0]);
        fprintf(stderr, "  example: %s \"Wise for Windows Installer\" #32770 "
                "--exec wfwi /c test.wsi\n", argv[0]);
        fflush(0);
        return error;
    }

    fprintf(stderr, "WRAPPING: (%s)\n", cmd);
    fflush(0);

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        DWORD error = GetLastError();
        fprintf(stderr, "ERROR: Could not create process (error %u)\n", error);
        fflush(0);
        return opts.fail_code;
    }

    fi.pid = pi.dwProcessId;
    fi.sendkey_code = opts.sendkey_code;
    fi.noclip = opts.noclip;
    DWORD start = GetTickCount() / 1000;
    DWORD last = start;

    bool bTriedToKill = false;
    DWORD wait;
    do {
        wait = WaitForSingleObject(pi.hProcess, opts.period);
        DWORD now = GetTickCount() / 1000;
        if (wait == WAIT_TIMEOUT) {
            if (!EnumWindows(EnumWindowsProc, (LPARAM)&fi) &&
                !fi.found)
            {
                DWORD error = GetLastError();
                fprintf(stderr, "WARNING: EnumWindows() (error %u)\n", error);
                fflush(0);
            }
            if ((opts.max_run && ((now - start) >= opts.max_run)) ||
                (opts.max_wait && ((now - last) >= opts.max_wait)) ||
                (opts.max_count && (fi.count() >= opts.max_count)) ||
                (fi.found && opts.kill)
                )
            {
                bTriedToKill = true;
                if (TerminateProcess(pi.hProcess, opts.fail_code))
                {
                    fprintf(stderr, "INFO: Killed process\n");
                    fflush(0);
                    break;
                } else {
                    DWORD error = GetLastError();
                    fprintf(stderr,
                            "ERROR: Cannot terminate process (error %u)\n",
                            error);
                    fflush(0);
                }
            }
        }
        last = now;
    } while (wait == WAIT_TIMEOUT);

    DWORD dwExitCode = 0;
    DWORD got_code_error = 0;

    if (!GetExitCodeProcess(pi.hProcess, &dwExitCode)) {
        got_code_error = GetLastError();
        fprintf(stderr, "ERROR: Could not get exit code (%u)\n",
                got_code_error);
        fflush(0);
        // XXX - fixup, just in case we don't get an error code...
        // this should not happen, but just in case...
        assert(got_code_error);
        if (!got_code_error) got_code_error = 1;
    }

    if (fi.found) {
        fprintf(stderr, "ERROR: "
                "The wrappee popped up one or more windows\n");
        fflush(0);
        if (!got_code_error && dwExitCode==opts.succ_code) {
            fprintf(stderr, "ERROR: "
                    "The wrappee did not return an error code\n");
            fflush(0);
        }
    }

    if (bTriedToKill) return opts.fail_code;
    if (got_code_error) return opts.fail_code;
    if ((fi.found) && (opts.report_match_as_non_success) && (dwExitCode==opts.succ_code))
        return opts.fail_code;
    return dwExitCode;

}
