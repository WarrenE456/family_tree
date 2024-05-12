#include "headers/ntree.h"
#include "headers/io.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define FPS 4

/* TODO
 * Event polling
 * Menu for init traits
 * Sex linked traits
 * Multiple traits
 * Death
*/

typedef enum {
	PAUSED = 0,
	PLAYING,
} Mode;

void* pollEvents(void* args) {
	char* input = (char*)args;
	while (1) *input = getchar();
}

int main(void)
{
	Person female, male;
	Allele alleles[2] = { DOMINANT, RECESSIVE };
	PersonFrom(&female, 0, FEMALE, alleles);
	PersonFrom(&male, 0, MALE, alleles);

	Ntree tree;
	NtreeNew(&tree, female);
	NtreeMary(&tree, male);

	double deltaYears = 13;
	double yearsPassed = 0;
	Mode mode = PLAYING;

	char input;
	pthread_t eventsThread;
	pthread_create(&eventsThread, NULL, pollEvents, &input);

	bool gameShouldContinue = true;

	rawBegin();
	while (gameShouldContinue) {
		if (mode == PLAYING)
		{
			time_t start = clock();
			rawEnd();
			system("clear");
			printf("Years passed: %.1f\n\n", yearsPassed);
			NtreeDisplayHead(&tree);
			rawBegin();

			NtreeUpdate(&tree, deltaYears / FPS);
			yearsPassed += deltaYears / FPS;

			do {
				char temp = input; // Ensure that input does not change mid switch-case by making a copy
				switch (temp) {
					case 'q':
						gameShouldContinue = false;
						break;
				}
			} while ((double)(clock() - start) / CLOCKS_PER_SEC < 1.0 / FPS);
		}
		if (mode == PAUSED)
		{

			rawEnd();
			system("clear");
			printf("Years passed: %.1f\n\n", yearsPassed);
			NtreeDisplayHead(&tree);
			rawBegin();
			
			char temp = input; // Ensure that input does not change mid switch-case by making a copy
			switch (temp) {
				case 'q':
					gameShouldContinue = false;
					break;
			}
		}
	}

	pthread_cancel(eventsThread);
	rawEnd();
	return 0;
}
