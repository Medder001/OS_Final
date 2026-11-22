// If you're reading this, I am PROBABLY still on vacation and will work on this when I can.
// Have a Happy Thanksgiving!!!
#include <stdio.h>
//Add definitions here
#define MAX_COUNT 1000000000


void countByHundred(){
	// TODO: Decide how the timer interupt should work here
	int currCount = 0;
	while(currCount < MAX_COUNT){
		currCount = currCount + 100;
		if((currCount % 1000) == 0){
			printf("%i is divisible by 1000", currCount); //TODO:  Substitute this for write() when it is created
		}
		if((currCount / 100000) == 0){
			// TODO: Decide how to yield in a process
			//yield();
		}
	}
	//TODO: Uncomment proceeding line when exit is decided.
	//exit();
}
