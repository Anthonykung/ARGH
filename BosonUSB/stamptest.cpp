#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

int main(){
	struct stat file_stats;
	stat("hey.txt", &file_stats);
	printf("timestamp: %ld , %s", file_stats.st_mtime, ctime(&file_stats.st_mtime));

return 0;
}
