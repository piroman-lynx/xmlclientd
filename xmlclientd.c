#include "master.h"
#include "client.h"
#include "debug.h"

int main(int argc, char* argv[])
{
    debug("Starting!\n");
    //console_start();
    master_start_process(0, 0);
}
