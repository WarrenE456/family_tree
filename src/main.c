#include "headers/ntree.h"
#include "headers/io.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define FPS 4

/* TODO
 * Add more menu
 * Menu for init. traits
 * Menu for phenotype
 * Fix random number chaning num of children when deltaYear changed
 * Screen buffer
 * Sex linked traits
 * Multiple traits
 * Death
*/

typedef enum {
	PAUSED = 0,
	PLAYING,
	STATS,
} Mode;

void* pollEvents(void* args) {
	char* input = (char*)args;
	while (1) {
		*input = getchar();
	}
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

	double deltaYears = 3;
	double yearsPassed = 0;
	Mode mode = PLAYING;

	int input;
	pthread_t eventsThread;
	pthread_create(&eventsThread, NULL, pollEvents, &input);

	int cursor = 0;
	bool gameShouldContinue = true;

	rawBegin();
	while (gameShouldContinue) {
		if (mode == PLAYING)
		{
			time_t start = clock();
			rawEnd();
			system("clear");
			printf("CONTROLS: q(quit), SPACE-BAR(pause)\n");
			printf("[PLAYING]\n");
			printf("Years passed: %.1f\n\n", yearsPassed);
			NtreeHeadDisplay(&tree, -1);
			rawBegin();

			NtreeUpdate(&tree, deltaYears / FPS);
			yearsPassed += deltaYears / FPS;

			do {
				char temp = input; // Ensure that input does not change mid switch-case by making a copy
				switch (temp) {
					case 'q':
						gameShouldContinue = false;
					break;
					case ' ':
						mode = PAUSED;
						cursor = 0;
					break;
				}
				input = -1;
			} while ((double)(clock() - start) / CLOCKS_PER_SEC < 1.0 / FPS);
		}
		else if (mode == PAUSED)
		{
			rawEnd();
			system("clear");
			printf("CONTROLS: q(quit), SPACE-BAR(resume), i(inspect), j(go-up), k(go-down)\n");
			printf("[PAUSED]\n");
			printf("Years passed: %.1f\n\n", yearsPassed);
			unsigned numMembers = NtreeHeadDisplay(&tree, cursor);
			rawBegin();

			bool shouldContinueInputLoop = true;
			while (shouldContinueInputLoop)
			{
				char temp = input; // Ensure that input does not change mid switch-case by making a copy
				switch (temp) {
					case 'q':
						shouldContinueInputLoop = false;
						gameShouldContinue = false;
						input = -1;
					break;
					case ' ':
						shouldContinueInputLoop = false;
						mode = PLAYING;
						input = -1;
					break;
					case 'i':
						shouldContinueInputLoop = false;
						mode = STATS;
						input = -1;
					break;
					case 'j':
						shouldContinueInputLoop = false;
						++cursor;
						input = -1;
					break;
					case 'k':
						shouldContinueInputLoop = false;
						--cursor;
						input = -1;
					break;
				}
			}

			cursor = cursor > 0 ? cursor : 0;
			cursor = cursor < numMembers ? cursor : numMembers - 1;
		}
		else if (mode == STATS)
		{
			Ntree family = NtreeHeadGetFamily(&tree, cursor);
			rawEnd();
			system("clear");
			printf("CONTROLS: q(quit), b(back), i(inspect), j(go-up), k(go-down)\n");
			printf("%c%c\n", (family.blood.alleles[0] == DOMINANT) ? 'A' : 'a', (family.blood.alleles[1] == DOMINANT) ? 'A' : 'a');
			rawBegin();

			bool shouldContinueInputLoop = true;
			while (shouldContinueInputLoop)
			{
				char temp = input; // Ensure that input does not change mid switch-case by making a copy
				switch (temp) {
					case 'q':
						shouldContinueInputLoop = false;
						gameShouldContinue = false;
						input = -1;
					case 'b':
						shouldContinueInputLoop = false;
						mode = PAUSED;
						input = -1;
					break;
				}
			}

		}
	}

	pthread_cancel(eventsThread);
	rawEnd();
	return 0;
}
