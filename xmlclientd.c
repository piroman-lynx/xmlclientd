#include "master.h"
#include "client.h"
#include "debug.h"

int main(int argc, char* argv[])
{
    debug("Starting!\n");
    console_start();
    //master_start_process(argc, argv);
    int i;
    for (i=0; i<3; i++){
	client_start_process(argc, argv);
    }
}
