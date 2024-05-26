#ifndef NTREE_H
#define NTREE_H

#include "stdbool.h"
#include <stdlib.h>

typedef enum {
    RECESSIVE = 1,
    DOMINANT = 2,
} Allele;

typedef enum {
    RECESSIVE_X = 1,
    DOMINANT_X = 2,
    Y = 3,
} AlleleSexlinked;

typedef struct {
    char* dominant;   
    char* recessive;   
    bool isDominant;
    int alleles[2];
} Trait;

void TraitFrom(Trait* self, char* dominant, char* recessive, int allele1, int allele2);

typedef struct {
    unsigned lenX;
    unsigned reservedX;
    Trait* xLinked;

    unsigned lenAuto;
    unsigned reservedAuto;
    Trait* autosomal;
} Traits;

void TraitsNew(Traits* self);
void TraitsAppend(Traits* self, Trait newTrait, bool isAutosomal);

typedef enum {
    MALE = 0,
    FEMALE = 1,
} Sex;

typedef struct {
    double age;
    Sex sex;
    Traits traits;
} Person;

void PersonFrom(Person *self, unsigned age, Sex sex, Traits traits);

typedef struct {
    unsigned len;
    unsigned reserved;
    bool *data;
} Flags;

void FlagsNew(Flags *self);
void FlagsChangeFlag(Flags *self, bool flag, unsigned index);

typedef struct Ntree Ntree;

typedef struct {
    unsigned len;
    unsigned reserved;
    Ntree *data;
} Offspring;

void OffspringNew(Offspring *self);
void OffspringAppend(Offspring *self, Ntree newCouple);

typedef struct Ntree {
    bool married;
    Person blood, spouse;
    Offspring nextGen;

} Ntree;

void NtreeNew(Ntree *self, Person blood);
void NtreeMary(Ntree *self, Person spouse);
void NtreeHaveChild(Ntree *self);
unsigned NtreeHeadDisplay(Ntree *self, int cursor);
void NtreeUpdate(Ntree *self, double deltaYears);
Ntree NtreeHeadGetFamily(Ntree *self, unsigned index);
void NtreeDisplayFamily(Ntree *self);

#endif
