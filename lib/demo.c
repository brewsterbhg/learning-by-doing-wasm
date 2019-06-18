#include <emscripten.h>
#include <stdio.h>
#include <string.h>

EM_JS(void, jsFunction, (int n), {
    console.log("Call from EM_JS: " + n);
});

int main()
{
    printf("Wasm Ready!\n");
    return 1;
}

int getNum()
{
    return 22;
}

char *greet(char *name)
{
    char *greeting = "Hello ";
    strcat(greeting, name);
    return greeting;
}

void runScript()
{
    emscripten_run_script_string("console.log('Hello from C!')");
}

void runAsyncScript()
{
    emscripten_async_run_script("console.log('Hello from C - ASYNC!')", 2000);
}

void runScriptFromJS()
{
    int jsVal = emscripten_run_script_int("getNum()");

    printf("Value from getNum(): %d\n", jsVal);
}

void runEMJSScript()
{
    jsFunction(182);
}