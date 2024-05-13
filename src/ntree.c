#include "headers/ntree.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define AVERAGE_AMOUNT_CHILDREN 2.4
#define INTEGRAL_EPOCH 128
#define PI 3.14159

void PersonFrom(Person *self, unsigned age, Sex sex, Allele *alleles, Traits traits) {
    self->age = age;
    self->sex = sex;
    self->alleles[0] = alleles[0];
    self->alleles[1] = alleles[1];
    self->traits = traits;
}

void FlagsNew(Flags *self) {
    self->reserved = 16;
    self->data = (bool *)malloc(sizeof(bool) * self->reserved);
}

void FlagsChangeFlag(Flags *self, bool flag, unsigned index) {
    while (index > self->reserved)
        self->data = (bool *)realloc(self->data, (self->reserved *= 2));
    self->data[index] = flag;
}

void OffspringNew(Offspring *self) {
    self->len = 0;
    self->reserved = 16;
    self->data = (Ntree *)malloc(sizeof(Ntree) * self->reserved);
}

void OffspringAppend(Offspring *self, Ntree newCouple) {
    ++self->len;
    if (self->len > self->reserved) {
        self->reserved *= 2;
        self->data = (Ntree *)realloc(self->data, sizeof(Ntree) * self->reserved);
    }
    self->data[self->len - 1] = newCouple;
}

void NtreeNew(Ntree *self, Person blood) {
    self->blood = blood;
    OffspringNew(&self->nextGen);
    self->married = false;
}

void NtreeMary(Ntree *self, Person spouse) {
    self->married = true;
    self->spouse = spouse;
}

// Robert Jenkins' 96 bit Mix Function
unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
    a = a - b;
    a = a - c;
    a = a ^ (c >> 13);
    b = b - c;
    b = b - a;
    b = b ^ (a << 8);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 13);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 12);
    b = b - c;
    b = b - a;
    b = b ^ (a << 16);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 5);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 3);
    b = b - c;
    b = b - a;
    b = b ^ (a << 10);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 15);
    return c;
}

static int randomNumber(int min, int max) {
    unsigned long seed = mix(clock(), time(NULL), getpid());
    srand(seed);
    return rand() % (max - min) + min;
}

void NtreeHaveChild(Ntree *self) {
    Allele femaleAllele = randomNumber(0, 2);
    Allele maleAllele = randomNumber(0, 2);

    Allele newGenotype[2];
    if (maleAllele == DOMINANT) {
        newGenotype[0] = maleAllele;
        newGenotype[1] = femaleAllele;
    } else {
        newGenotype[0] = femaleAllele;
        newGenotype[1] = maleAllele;
    }

    Traits childsTraits;
    for (int i = 0; i < self->blood.traits.lenAuto; ++i)
    {
        int genotype[2];
        genotype[0] = self->blood.traits.autosomal[i].alleles[randomNumber(0, 1)];
        genotype[1] = self->blood.traits.autosomal[i].alleles[randomNumber(0, 1)];
        Trait trait; TraitFrom(&trait, self->blood.traits.autosomal[i].dominant,
                               self->blood.traits.autosomal[i].recessive,
                               genotype[0], genotype[1]);
        TraitsAppend(&childsTraits, trait, true);
    }

    unsigned sexChromosomeFromMom = randomNumber(0, 2);
    unsigned sexChromosomeFromDad = randomNumber(0, 2);
    for (int i = 0; i < self->blood.traits.lenX; ++i)
    {
        int genotype[2];
        genotype[0] = self->blood.traits.xLinked[i].alleles[sexChromosomeFromMom];
        genotype[1] = self->blood.traits.xLinked[i].alleles[sexChromosomeFromMom];
        Trait trait; TraitFrom(&trait, self->blood.traits.xLinked[i].dominant,
                               self->blood.traits.xLinked[i].recessive,
                               genotype[0], genotype[1]);
        TraitsAppend(&childsTraits, trait, false);
    }

    Sex sex;
    if (self->blood.traits.lenX == 0)
        sex = randomNumber(0, 2);
    else
        sex = (sexChromosomeFromDad == Y) ? MALE : FEMALE;

    Person newChild;
    PersonFrom(&newChild, 0, sex, newGenotype, childsTraits);
    Ntree newFamily;
    NtreeNew(&newFamily, newChild);

    OffspringAppend(&self->nextGen, newFamily);
}

static unsigned NtreeDisplay(Ntree *self, unsigned depth, Flags *isLast,
                             int num, int cursor) {
    for (int i = 1; i < depth; ++i) {
        if (isLast->data[i]) {
            printf("    ");
        } else {
            printf("│   ");
        }
    }
    if (depth > 0) {
        if (isLast->data[depth] == true) {
            printf("└──%c%c", self->blood.alleles[0], self->blood.alleles[1]);
        } else {
            printf("├──%c%c", self->blood.alleles[0], self->blood.alleles[1]);
        }
    }
    for (int i = 0; i < 2; ++i) {
        switch (self->blood.alleles[i]) {
            case DOMINANT:
                printf("A");
                break;
            case RECESSIVE:
                printf("a");
                break;
        }
    }
    if (num == cursor)
        printf(" <");
    printf("\n");

    unsigned numMembers = 1;
    FlagsChangeFlag(isLast, false, depth + 1);
    for (int i = 0; i < self->nextGen.len; ++i) {
        if (i == self->nextGen.len - 1) {
            FlagsChangeFlag(isLast, true, depth + 1);
        }
        numMembers += NtreeDisplay(&self->nextGen.data[i], depth + 1, isLast,
                                   num + numMembers, cursor);
    }
    return numMembers;
}

unsigned NtreeHeadDisplay(Ntree *self, int cursor) {
    Flags isLast;
    FlagsNew(&isLast);
    FlagsChangeFlag(&isLast, true, 0);
    return NtreeDisplay(self, 0, &isLast, 0, cursor);
}

static const double standardDeviation = 3.3;
static const double meanChildBirthAge = 28.0;

static double childBirthNormalDistribution(double x) {
    return AVERAGE_AMOUNT_CHILDREN *
    exp((-pow(x - meanChildBirthAge, 2.0)) /
        (2.0 * pow(standardDeviation, 2.0))) /
    (standardDeviation * sqrt(2 * PI));
}

static double changeToHaveChild(double startingAge, double endingAge) {
    double chance = 0;
    double dx = (endingAge - startingAge) / INTEGRAL_EPOCH;
    for (int i = 0; i < INTEGRAL_EPOCH; ++i) {
        chance += childBirthNormalDistribution(startingAge + i * dx) * dx;
    }
    return chance;
}

static bool fate(double probability) {
    if (randomNumber(0, 10000) < (int)(probability * 10000))
        return true;
    return false;
}
void NtreeUpdate(Ntree *self, double deltaYears) {
    for (int i = 0; i < self->nextGen.len; ++i) {
        NtreeUpdate(&self->nextGen.data[i], deltaYears);
    }

    if (self->married) {
        double startingAge =
            (self->blood.sex == FEMALE) ? self->blood.age : self->spouse.age;
        self->blood.age += deltaYears;
        self->spouse.age += deltaYears;

        if (fate(changeToHaveChild(startingAge, startingAge + deltaYears)))
            NtreeHaveChild(self);

        // srand(time(NULL));
        // int randomnum = rand() % (int)(20.0 * (1.0/deltaYears));
        // if (randomnum == 0) {
        // 	NtreeHaveChild(self);
        // }
    }
    if (!self->married) {
        self->blood.age += deltaYears;

        int randomnum = rand() % (int)(20.0 * (1.0 / deltaYears));
        if (randomnum == 0) {
            self->married = true;
            Allele genotype[2] = {randomNumber(0, 2), randomNumber(0, 2)};
            Person spouse;
            PersonFrom(&spouse, self->blood.age, !self->blood.sex, genotype, self->blood.traits);
            NtreeMary(self, spouse);
        }
    }
}

typedef struct {
    Ntree *foundFamily;
    int numMembersOfBranch;
} GetPersonResult;

static GetPersonResult NtreeGetFamily(Ntree *self, unsigned int index) {
    typedef GetPersonResult Result;
    unsigned numberOfPeopleInBranch = 0;

    if (index - numberOfPeopleInBranch == 0) {
        Result result = {self, -1};
        return result;
    }
    ++numberOfPeopleInBranch;

    for (int i = 0; i < self->nextGen.len; ++i) {
        Result result =
            NtreeGetFamily(&self->nextGen.data[i], index - numberOfPeopleInBranch);
        if (result.foundFamily)
            return result;
        numberOfPeopleInBranch += result.numMembersOfBranch;
    }

    Result finalResult = {NULL, numberOfPeopleInBranch};
    return finalResult;
}

Ntree NtreeHeadGetFamily(Ntree *self, unsigned index) {
    Ntree family = *NtreeGetFamily(self, index).foundFamily;
    return family;
}

void NtreeDisplayFamily(Ntree *const self) {
    printf("Married: %s\n\n", self->married ? "true" : "false");

    printf("Related by blood:\n"
           "	Sex: %s\n"
           "	Age: %.1f\n"
           "	Genotype: %c%c\n",
           (self->blood.sex == MALE) ? "male" : "female", self->blood.age,
           (self->blood.alleles[0] == DOMINANT) ? 'A' : 'a',
           (self->blood.alleles[1] == DOMINANT) ? 'A' : 'a');

    if (self->married) {
        printf("Related by blood:\n"
               "	Sex: %s\n"
               "	Age: %.1f\n"
               "	Genotype: %c%c\n",
               (self->spouse.sex == MALE) ? "male" : "female", self->spouse.age,
               (self->spouse.alleles[0] == DOMINANT) ? 'A' : 'a',
               (self->spouse.alleles[1] == DOMINANT) ? 'A' : 'a');
    }

    printf("Offspring:\n");
    for (int i = 0; i < self->nextGen.len; ++i) {
        Person child = self->nextGen.data[i].blood;
        printf("%d) %s %.1f; %c%c\n", i + 1,
               (child.sex == MALE) ? "Male" : "Female", child.age,
               (child.alleles[0] == DOMINANT) ? 'A' : 'a',
               (child.alleles[0] == DOMINANT) ? 'A' : 'a');
    }
}

static const unsigned IS_DOMINANT = 2;
void TraitFrom(Trait* self, char* dominant, char* recessive, int allele1, int allele2)
{
    self->dominant = dominant;
    self->recessive = recessive;

    if (allele1 % IS_DOMINANT == 0 || allele2 == Y) {
        self->alleles[0] = allele1;
        self->alleles[1] = allele2;
    }
    else {
        self->alleles[1] = allele1;
        self->alleles[0] = allele2;
    }

    self->isDominant = false;
    if (allele1 % IS_DOMINANT == 0 || allele2 % IS_DOMINANT == 0) self->isDominant = true;
}

void TraitsNew(Traits* self)
{
    self->lenX = 0, self->lenAuto = 0;
    self->reservedX = 16, self->reservedAuto = 16;
    
    self->xLinked = malloc(sizeof(Trait) * self->reservedX);
    self->autosomal = malloc(sizeof(Trait) * self->reservedAuto);
}

void TraitsAppend(Traits *self, Trait newTrait, bool isAutosomal)
{
    if (isAutosomal) {
        ++self->lenAuto;
        if (self->lenAuto > self->reservedAuto) {
            self->reservedAuto *= 2;
            self->autosomal = (Trait*)realloc(self->autosomal, self->reservedAuto);
        }
        self->autosomal[self->lenAuto - 1] = newTrait;
    }
    else {
        ++self->lenX;
        if (self->lenX > self->reservedX) {
            self->reservedX *= 2;
            self->xLinked = (Trait*)realloc(self->xLinked, self->reservedX);
        }
        self->xLinked[self->lenX - 1] = newTrait;
    }
}
