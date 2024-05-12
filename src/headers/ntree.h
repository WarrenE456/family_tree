#ifndef NTREE_H
#define NTREE_H

#include "stdbool.h"
#include <stdlib.h>

typedef enum {
	DOMINANT = 0,
	RECESSIVE = 1,
} Allele;

typedef enum {
	MALE = 0,
	FEMALE = 1,
} Sex;

typedef struct {
	double age;
	Sex sex;
	Allele alleles[2];	
} Person;

void PersonFrom(Person* self, unsigned age, Sex sex, Allele* alleles);

typedef struct {
	unsigned len;
	unsigned reserved;
	bool* data;
} Flags;

void FlagsNew(Flags* self);
void FlagsChangeFlag(Flags* self, bool flag, unsigned index);

typedef struct Ntree Ntree;

typedef struct {
	unsigned len;
	unsigned reserved;
	Ntree* data;
} Offspring;

void OffspringNew(Offspring* self);
void OffspringAppend(Offspring* self, Ntree newCouple);

typedef struct Ntree
{
	bool married;
	Person blood, spouse;
	Offspring nextGen;
	
} Ntree;

void NtreeNew(Ntree* self, Person blood);
void NtreeMary(Ntree* self, Person spouse);
void NtreeHaveChild(Ntree* self);
void NtreeDisplayHead(Ntree* self);
void NtreeUpdate(Ntree* self, double deltaYears);

#endif
