#include "logging.c"


int main(int argc, char *argv[])
{
	ASSERT(argc >= 2, "Must pass at least one parameter.  %d given.", argc-1);
	XINFO("I like jam %d", argc);
	INFO("He llikes jam %d", argc);
	WARN("NO jam here! %d", argc);
	ERROR("MY JAM!! %d", argc);
	ABORT("Goodbye");
}
